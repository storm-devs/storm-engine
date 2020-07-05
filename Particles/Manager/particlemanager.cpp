#include "particlemanager.h"
#include "..\..\common_h\dx8render.h"
#include "..\..\common_h\vapi.h"

#include "..\datacache\datacache.h"
#include "..\geomcache\geomcache.h"
#include "..\system\particlesystem\particlesystem.h"
#include "..\service\particleservice.h"

#include "..\system\particleprocessor\nameparser.h"
#include "..\icommon\iemitter.h"
#include "..\system\datasource\datastring.h"


DWORD GraphRead = 0;

ParticleManager::ParticleManager (ParticleService* service) : IParticleManager (service), DeleteQuery(_FL_), Systems(_FL_), EnumUsedGeom(_FL_)
{
	pService = service;
	ShowStat = false;
	MDL_Processor = NEW ModelProcessor(this);
	BB_Processor = NEW BillBoardProcessor; 
	GlobalDelete = false;
	TimeFromLastStatUpdate = 100.0f;
	pRS = (VDX8RENDER*)api->CreateService("DX8Render");
	Assert (pRS != NULL);

	pDataCache = NEW DataCache(this);
	pGeomCache = NEW GeomCache;

	pProjectTexture = -1;
	pProjectTextureNormalMap = -1;
}

ParticleManager::~ParticleManager ()
{
	DeleteAllSystems ();
	if (pProjectTexture >= 0)
	{
		pRS->TextureRelease(pProjectTexture);
	}
  pProjectTexture = -1;

	if (pProjectTextureNormalMap)
	{
		pRS->TextureRelease(pProjectTextureNormalMap);
	}
	pProjectTextureNormalMap = -1;


	delete pDataCache;
	delete pGeomCache;
	delete BB_Processor;
	delete MDL_Processor;
	pService->RemoveManagerFromList(this);
}

bool ParticleManager::Release ()
{
	delete this;
	return true;
}

VDX8RENDER* ParticleManager::Render ()
{
	return pRS;
}


//Установить текстуру проекта
void ParticleManager::SetProjectTexture (const char* FileName)
{
	if (pProjectTexture)
	{
		pRS->TextureRelease(pProjectTexture);
		pProjectTexture = -1;
	}

	if (pProjectTextureNormalMap)
	{
		pRS->TextureRelease(pProjectTextureNormalMap);
		pProjectTextureNormalMap = -1;
	}

	pProjectTexture = pRS->TextureCreate(FileName);

	string a;
	a.GetFileTitle(FileName);
	a += "nm";
	pProjectTextureNormalMap = pRS->TextureCreate(a.c_str());


	TextureName = FileName;

	
}

const char* ParticleManager::GetProjectTextureName ()
{
	return TextureName.GetBuffer();
}

//Открыть проект 
bool ParticleManager::OpenProject (const char* FileName)
{
  CloseProject ();
	ShortProjectName = FileName;

	string LongFileName = "resource\\particles\\";
	LongFileName += FileName;
	LongFileName.AddExtention(".prj");

	
	INIFILE* IniFile = api->fio->OpenIniFile((char*)LongFileName.GetBuffer());
	if (!IniFile)
	{
		api->Trace("Can't find project '%s'", LongFileName.GetBuffer());
		return false;
	}

	static char IniStringBuffer[8192];

	//Устанавливаем текстуру проекта...
	IniFile->ReadString("Textures", "MainTexture", IniStringBuffer, 8192, "none");
	//api->Trace("Manager use texture: %s", IniStringBuffer);
	SetProjectTexture (IniStringBuffer);

/*
	//Загружаем модельки в кэш
	for (int n = 0; n < 9999; n++)
	{
		string Section;
		Section.Format("Model_%04d", n);
		bool ReadSuccess = IniFile->ReadString("ModelsCache", (char*)Section.GetBuffer(), IniStringBuffer, 8192, "none");
		if (!ReadSuccess) break;

		//api->Trace("Cache geom: %s", IniStringBuffer);
		pGeomCache->CacheModel(IniStringBuffer);

	}
*/

	//Загружаем данные 
	for (int n = 0; n < 9999; n++)
	{
		string Section;
		Section.Format("System_%04d", n);
		bool ReadSuccess = IniFile->ReadString("Manager", (char*)Section.GetBuffer(), IniStringBuffer, 8192, "none");
		if (!ReadSuccess) break;
		pDataCache->CacheSystem(IniStringBuffer);
	}

	delete IniFile;

	//FIX ME !
	//Если будет асинхронная загрузка это неверно...
	CreateGeomCache();

	return true;
}

//Получить имя проекта 
const char* ParticleManager::GetProjectFileName ()
{
	return ShortProjectName.GetBuffer();
}


//Закрыть проект 
void ParticleManager::CloseProject ()
{
	BB_Processor->Clear ();
	MDL_Processor->Clear ();
	DeleteAllSystems ();
	if (pProjectTexture)
	{
		pRS->TextureRelease(pProjectTexture);
	}

	if (pProjectTextureNormalMap)
	{
		pRS->TextureRelease(pProjectTextureNormalMap);
	}
	pProjectTextureNormalMap = -1;

	pProjectTexture = -1;
	pDataCache->ResetCache();
	pGeomCache->ResetCache();
}
 
//Удалить из списка ресурсов (системная)
void ParticleManager::RemoveResource (IParticleSystem* pResource)
{
	if (GlobalDelete) return;
	for (DWORD n = 0; n < Systems.Size(); n++)
	{
		if (Systems[n].pSystem == pResource)
		{
			Systems.ExtractNoShift(n);
			return;
		}
	}
}

//Исполнить партиклы 
void ParticleManager::Execute (float DeltaTime)
{
	GraphRead = 0;
	ActiveSystems = 0;
	ActiveEmitters = 0;
	ActiveBillboardParticles = 0;
	ActiveModelParticles = 0;

	//pRS->Clear(0, NULL, D3DCLEAR_STENCIL | D3DCLEAR_TARGET |D3DCLEAR_ZBUFFER, 0xFF404080, 1.0f, 0);

	DWORD TicksTime, ProcessTime;
	RDTSC_B(TicksTime);
	ProcessTime = TicksTime;

	ActiveSystems = Systems.Size();
	for (DWORD n = 0; n < Systems.Size(); n++)
	{
		ActiveEmitters += Systems[n].pSystem->Execute(DeltaTime);
	}
	BB_Processor->Process(DeltaTime);
	MDL_Processor->Process(DeltaTime);
	RDTSC_E(ProcessTime);

	ActiveBillboardParticles = BB_Processor->GetCount();
	ActiveModelParticles = MDL_Processor->GetCount();
	pRS->TextureSet(0, pProjectTexture);
	pRS->TextureSet(1, pProjectTexture);
	pRS->TextureSet(3, pProjectTextureNormalMap);
//	pRS->SetTexture(0, pProjectTexture);
//	pRS->SetTexture(1, pProjectTexture);
	BB_Processor->Draw();
	MDL_Processor->Draw();

	RDTSC_E(TicksTime);

	TimeFromLastStatUpdate += DeltaTime;
	if (TimeFromLastStatUpdate > 1.0f)
	{
		TimeFromLastStatUpdate = 0.0f;
		nowTickTime = TicksTime;
		nowUpdateTime = ProcessTime;
	}

	if (ShowStat)
	{
		/*
		IFont* pSysFont = pRS->GetSystemFont ();
		float Width = pSysFont->GetLength("Graph read count - %d", GraphRead);
		pSysFont->Release();
		*/

		D3DVIEWPORT8 ViewPort;
		pRS->GetViewport(&ViewPort);
		RS_SPRITE spr[4];
		float x1 = -1.0f;
		float x2 = ((220.0f / (float)ViewPort.Width) * 2) - 1.0f;
		float y1 = 1.0f - ((16.0f / (float)ViewPort.Height)*2.0f);
		float y2 = 1.0f - ((150.0f / (float)ViewPort.Height)* 2.0f);
		spr[0].vPos = CVECTOR(x1, y1, 0.2f);
		spr[1].vPos = CVECTOR(x2, y1, 0.2f);
		spr[2].vPos = CVECTOR(x2, y2, 0.2f);
		spr[3].vPos = CVECTOR(x1, y2, 0.2f);
		pRS->DrawSprites(spr, 1, "dbgInfoSprite");

/*
		pRS->Print(0, 16, 0xFFFFFFFF, "Systems - %d", ActiveSystems);
		pRS->Print(0, 32, 0xFFFFFFFF, "Emitters - %d", ActiveEmitters);
		pRS->Print(0, 48, 0xFFFFFFFF, "Billboards - %d", ActiveBillboardParticles);
		pRS->Print(0, 64, 0xFFFFFFFF, "Models - %d", ActiveModelParticles);
		pRS->Print(0, 80, 0xFFFFFFFF, "Total time - %d", nowTickTime);
		pRS->Print(0, 96, 0xFFFFFFFF, "Update time - %d", nowUpdateTime);
		pRS->Print(0, 112, 0xFFFFFFFF, "Graph read count - %d", GraphRead);
*/

		if (GraphRead != 0)
		{
			float AverageReadTime = (float)nowUpdateTime / (float)GraphRead;
//			pRS->Print(0, 128, 0xFFFFFFFF, "Average read time - %3.2f", AverageReadTime);
		}
	}

	if (api->Controls->GetDebugAsyncKeyState(VK_F3) < 0 && api->Controls->GetDebugAsyncKeyState(VK_CONTROL) < 0)
	{
		ShowStat = !ShowStat;
		Sleep(100);
	}

	for (n = 0; n < DeleteQuery.Size(); n++)
	{
		DeleteQuery[n]->Release();
	}

	DeleteQuery.DelAll();
}

//Узнать доступна система или нет 
bool ParticleManager::IsSystemAvailable (const char* FileName)
{
	if (pDataCache->GetParticleSystemDataSource (FileName)) return true;
	return false;
}

//Получить глобальную текстуру проекта 
long ParticleManager::GetProjectTexture ()
{
	return pProjectTexture;
}
 
 
//Создать партикловую систему из файла (файл должен быть в проекте!!!!!)
IParticleSystem* ParticleManager::CreateParticleSystemEx (const char* FileName, const char* File, int Line)
{
	DataSource* pDataSource = pDataCache->GetParticleSystemDataSource (FileName);
	if (!pDataSource)
	{
		api->Trace("Particle system '%s' can't loading. Reason: Not found in cache", FileName);
		return NULL;
	}

//	api->Trace("Create system '%s'", FileName);
	ParticleSystem* pSys = CreateParticleSystemFromDataSource (pDataSource);
	pSys->SetName(FileName);
	return pSys;
}

void ParticleManager::DeleteAllSystems ()
{
	GlobalDelete = true;
	for (DWORD n = 0; n < Systems.Size(); n++)
	{
		Systems[n].pSystem->Release();
	}

	Systems.DelAll();
	GlobalDelete = false;
}

ParticleSystem* ParticleManager::CreateParticleSystemFromDataSource (DataSource* pDataSource)
{
	ParticleSystem* pSys = NEW ParticleSystem(this);
	pSys->CreateFromDataSource(pDataSource);

	SystemDesc SysDesc;
	SysDesc.pSystem = pSys;
	Systems.Add(SysDesc);

	return pSys;
}

BillBoardProcessor* ParticleManager::GetBBProcessor ()
{
	return BB_Processor;
}

ModelProcessor* ParticleManager::GetMDLProcessor ()
{
	return MDL_Processor;
}


//Создать пустую партикловую систему, для редактора...
IParticleSystem* ParticleManager::CreateEmptyParticleSystemEx  (const char* FileName, int Line)
{
	//NEED WRITE !!!
	return NULL;
}

bool ParticleManager::ValidateSystem (IParticleSystem* pSystem)
{
	for (DWORD n = 0; n < Systems.Size(); n++)
	{
		if (Systems[n].pSystem == pSystem) return true;
	}
	return false;
}

GEOS* ParticleManager::GetModel (const char* FileName)
{
	return pGeomCache->GetModel(FileName);
}

DWORD ParticleManager::GetCreatedSystemCount ()
{
	return Systems.Size();
}

ParticleSystem* ParticleManager::GetCreatedSystemByIndex (DWORD Index)
{
	return Systems[Index].pSystem;
}

bool ParticleManager::ReadyForUse ()
{
	return true;
}

void ParticleManager::DefferedDelete (ParticleSystem* pSys)
{
	DeleteQuery.Add(pSys);
}

void ParticleManager::Editor_UpdateCachedData ()
{
	for (DWORD n = 0; n < Systems.Size(); n++)
	{
		Systems[n].pSystem->Editor_UpdateCachedData ();
	}

}

DWORD ParticleManager::GetProjectSystemCount ()
{
	return pDataCache->GetCachedCount();
}

const char* ParticleManager::GetProjectSystemName (DWORD Index)
{
	return pDataCache->GetCachedNameByIndex(Index);
}

const char* ParticleManager::GetFirstGeomName (const char* FileName)
{
	IteratorIndex = 0;
	GeomNameParser Parser;
	EnumUsedGeom.DelAll ();
	DataSource* pDataSource = pDataCache->GetParticleSystemDataSource (FileName);
	if (!pDataSource) return NULL;

	DWORD count = pDataSource->GetEmitterCount();

	for (DWORD n = 0; n < count; n++)
	{
		DataSource::EmitterDesc* pDesc = pDataSource->GetEmitterDesc(n);
		for (DWORD i = 0; i < pDesc->Particles.Size(); i++)
		{
			DataString* pModelNames = pDesc->Particles[i].Fields.FindString(PARTICLE_GEOM_NAMES);
			if (!pModelNames) continue;
			
			Parser.Tokenize(pModelNames->GetValue());

      DWORD TCount = Parser.GetTokensCount();
			if (TCount > 0)
			{
				for (DWORD j = 0; j < TCount; j++)
				{
					const char* GeomName = Parser.GetTokenByIndex(j);
					if (!FindInEnumUsedGeom(GeomName)) EnumUsedGeom.Add(string(GeomName));
				}
			}
		}
		
	}

	if (EnumUsedGeom.Size() == 0) return NULL;
	const char* FirstName = EnumUsedGeom[IteratorIndex].GetBuffer();
	IteratorIndex++;
	return FirstName;
	
}

const char* ParticleManager::GetNextGeomName ()
{
	if (IteratorIndex >= EnumUsedGeom.Size()) return NULL;
	const char* FirstName = EnumUsedGeom[IteratorIndex].GetBuffer();
	IteratorIndex++;
	return FirstName;
}

bool ParticleManager::FindInEnumUsedGeom (const char* GeomName)
{
	for (DWORD n = 0; n < EnumUsedGeom.Size(); n++)
	{
		const char* StoredGeomName = EnumUsedGeom[n].GetBuffer();
		if (stricmp (StoredGeomName, GeomName) == 0) return true;
	}
	return false;
}

void ParticleManager::CreateGeomCache ()
{
	pGeomCache->ResetCache();
	DWORD SystemCount = pDataCache->GetCachedCount ();
	for (DWORD n = 0; n < SystemCount; n++)
	{
		const char* pSystemName = pDataCache->GetCachedNameByIndex(n);
		const char* GeomName = NULL;
		GeomName = GetFirstGeomName(pSystemName);

		while (GeomName)
		{
			//api->Trace("Cache geom %s", GeomName);
			pGeomCache->CacheModel(GeomName);
			GeomName = GetNextGeomName();
		}
	}
}

void ParticleManager::WriteSystemCache (const char* FileName)
{
	DataSource* pDataSource = pDataCache->GetParticleSystemDataSource (FileName);
	if (!pDataSource)
	{
		api->Trace("Particle system '%s' can't save. Reason: Not found in cache", FileName);
		return;
	}

	MemFile pMemSave;
	pMemSave.OpenWrite(1048576);
	pDataSource->Write(&pMemSave);
	
	string LongFileName = "resource\\particles\\";
	LongFileName+=FileName;
	LongFileName.AddExtention(".xps");
/*
	IWrite* pFile = pFS->Write(LongFileName.GetBuffer(), iw_create_always, _FL_);
	pFile->Write(pMemSave.GetBuffer(), pMemSave.GetLength());
	pFile->Release();
*/

	pMemSave.Close();
	api->Trace("Particle system '%s' saved.", FileName);
}

void ParticleManager::WriteSystemCacheAs (const char* FileName, const char* NewName)
{
	DataSource* pDataSource = pDataCache->GetParticleSystemDataSource (FileName);
	if (!pDataSource)
	{
		api->Trace("Particle system '%s' can't save. Reason: Not found in cache", FileName);
		return;
	}

	MemFile pMemSave;
	pMemSave.OpenWrite(1048576);
	pDataSource->Write(&pMemSave);
/*
	IWrite* pFile = pFS->Write(NewName, iw_create_always, _FL_);
	pFile->Write(pMemSave.GetBuffer(), pMemSave.GetLength());
	pFile->Release();
*/

	pMemSave.Close();
	api->Trace("Particle system '%s' saved.", NewName);
}

void ParticleManager::WriteSystemCache (const char* FileName, MemFile* pMemFile)
{
	DataSource* pDataSource = pDataCache->GetParticleSystemDataSource (FileName);
	if (!pDataSource)
	{
		api->Trace("Particle system '%s' can't save. Reason: Not found in cache", FileName);
		return;
	}

	pDataSource->Write(pMemFile);
}

void ParticleManager::LoadSystemCache (const char* FileName, MemFile* pMemFile)
{
	DataSource* pDataSource = pDataCache->GetParticleSystemDataSource (FileName);
	if (!pDataSource)
	{
		api->Trace("Particle system '%s' can't load. Reason: Not found in cache", FileName);
		return;
	}

	pDataSource->Destroy();
	pDataSource->Load(pMemFile);

	CacheReloaded ();
}

void ParticleManager::CacheReloaded ()
{
	DWORD n = 0;
	array<CacheReloadedInfo>	UsedSystems(_FL_);
	for (n = 0; n < Systems.Size(); n++)
	{
		CacheReloadedInfo Info;
		Info.Name = Systems[n].pSystem->GetName();
		Info.AutoDeleted = Systems[n].pSystem->IsAutoDeleted();
		Systems[n].pSystem->GetTransform(Info.matWorld);

		UsedSystems.Add(Info);
	}


	DeleteAllSystems ();

	BB_Processor->Clear();
	MDL_Processor->Clear();


	for (n = 0; n < UsedSystems.Size(); n++)
	{
		IParticleSystem* pSystem = CreateParticleSystemEx (UsedSystems[n].Name.GetBuffer(), __FILE__, __LINE__);
		pSystem->AutoDelete(UsedSystems[n].AutoDeleted);
		pSystem->SetTransform(UsedSystems[n].matWorld);
	}
}


FieldList* ParticleManager::Editor_CreatePointEmitter (const char* SystemName, const char* EmitterName)
{
	DataSource* pDataSource = pDataCache->GetParticleSystemDataSource (SystemName);
	if (!pDataSource) return NULL;

	FieldList* pData = pDataSource->CreateEmptyPointEmitter(EmitterName);

	CacheReloaded ();

	return pData;
}

FieldList* ParticleManager::Editor_CreateBillBoardParticle (const char* SystemName, const char*EmitterName, const char* ParticleName)
{
	DataSource* pDataSource = pDataCache->GetParticleSystemDataSource (SystemName);
	if (!pDataSource) return NULL;

	FieldList* pData = pDataSource->CreateBillBoardParticle (ParticleName, EmitterName);

	CacheReloaded ();

	return pData;
}

FieldList* ParticleManager::Editor_CreateModelParticle (const char* SystemName, const char*EmitterName, const char* ParticleName)
{
	DataSource* pDataSource = pDataCache->GetParticleSystemDataSource (SystemName);
	if (!pDataSource) return NULL;

	FieldList* pData = pDataSource->CreateModelParticle (ParticleName, EmitterName);

	CacheReloaded ();

	return pData;
}

void ParticleManager::DeletePointEmitter (const char* SystemName, IEmitter* pEmitter)
{
	DataSource* pDataSource = pDataCache->GetParticleSystemDataSource (SystemName);
	if (!pDataSource) return;

	FieldList* pEmitterData = pEmitter->GetData();

	pDataSource->DeletePointEmitter (pEmitterData);

	CacheReloaded ();
}

void ParticleManager::DeleteBillboard (const char* SystemName, IEmitter* pEmitter, FieldList* pParticles)
{
	DataSource* pDataSource = pDataCache->GetParticleSystemDataSource (SystemName);
	if (!pDataSource) return;

	FieldList* pEmitterData = pEmitter->GetData();

	pDataSource->DeleteBillboard (pEmitterData, pParticles);

	CacheReloaded ();
}

void ParticleManager::DeleteModel (const char* SystemName, IEmitter* pEmitter, FieldList* pParticles)
{
	DataSource* pDataSource = pDataCache->GetParticleSystemDataSource (SystemName);
	if (!pDataSource) return;

	FieldList* pEmitterData = pEmitter->GetData();

	pDataSource->DeleteModel (pEmitterData, pParticles);

	CacheReloaded ();
}

void ParticleManager::OpenDefaultProject ()
{
	CloseProject ();
	ShortProjectName = "default";

	SetProjectTexture ("particles_list.tga");

	
	HANDLE foundFile;
	WIN32_FIND_DATA	findData;
	if ((foundFile = api->fio->_FindFirstFile("resource\\particles\\*.xps", &findData)) != INVALID_HANDLE_VALUE)
	{
		do
		{
			//api->Trace("Cache system - %s", findData.cFileName);
			pDataCache->CacheSystem(findData.cFileName);
		}	while (api->fio->_FindNextFile(foundFile, &findData) == TRUE);
	}

	CreateGeomCache();
}