#include "..\..\..\common_h\vmodule_api.h"
#include "fieldlist.h"
#include "databool.h"
#include "datacolor.h"
#include "datafloat.h"
#include "datagraph.h"
#include "dataposition.h"
#include "datauv.h"
#include "datastring.h"



FieldList::FieldList () : Fields(_FL_)
{
}

FieldList::~FieldList ()
{
	//DelAll ();
}

void FieldList::Load (MemFile* File)
{
	DWORD DataFieldsCount = 0;
	File->ReadType(DataFieldsCount);

	for (DWORD n = 0; n < DataFieldsCount; n++)
	{
		FieldType fldType = FIELD_UNKNOWN;
		File->ReadType(fldType);

		switch (fldType)
		{
			case FIELD_BOOL:
			{
				//api->Trace ("Particles info: BOOL field");
				CreateBoolField (File);
				break;
			}
			case FIELD_FLOAT:
			{
				//api->Trace ("Particles info: FLOAT field");
				CreateFloatField (File);
				break;
			}
			case FIELD_GRAPH:
			{
				//api->Trace ("Particles info: GRAPH field");
				CreateGraphField (File);
				break;
			}
			case FIELD_POSITION:
			{
				//api->Trace ("Particles info: POSITION field");
				CreatePositionField (File);
				break;
			}
			case FIELD_STRING:
			{
				//api->Trace ("Particles info: STRING field");
				CreateStringField (File);
				break;
			}
			case FIELD_UV:
			{
				//api->Trace ("Particles info: UV field");
				CreateUVField (File);
				break;
			}
			case FIELD_COLOR:
			{
				//api->Trace ("Particles info: COLOR field");
				CreateColorField (File);
				break;
			}
			default:
			{
				_THROW ("Particles: Unknown field type !!!!");
			}
		}	//switch
	} // for all fileds
}



void FieldList::CreateEmptyBoolField (const char* Name, bool def_value)
{
	DataBool* Field = NEW DataBool;
	Field->SetName(Name);
	Field->SetValue(def_value);

	FieldDesc pDesc;
	pDesc.MarkForDelete = false;
	pDesc.Name = Field->GetName();
	pDesc.HashValue = string::HashNoCase(pDesc.Name.GetBuffer());
	pDesc.pPointer = Field;
	pDesc.Type = FIELD_BOOL;
	Fields.Add(pDesc);
}

void FieldList::CreateEmptyFloatField (const char* Name, float def_value)
{
	DataFloat* Field = NEW DataFloat;
	Field->SetName(Name);
	Field->SetValue(def_value);

	FieldDesc pDesc;
	pDesc.MarkForDelete = false;
	pDesc.Name = Field->GetName();
	pDesc.HashValue = string::HashNoCase(pDesc.Name.GetBuffer());
	pDesc.pPointer = Field;
	pDesc.Type = FIELD_FLOAT;
	Fields.Add(pDesc);
}

void FieldList::CreateEmptyGraphField (const char* Name, float def_value_min, float def_value_max)
{
	DataGraph* Field = NEW DataGraph;
	Field->SetName(Name);
	Field->SetDefaultValue(def_value_max, def_value_min);

	FieldDesc pDesc;
	pDesc.MarkForDelete = false;
	pDesc.Name = Field->GetName();
	pDesc.HashValue = string::HashNoCase(pDesc.Name.GetBuffer());
	pDesc.pPointer = Field;
	pDesc.Type = FIELD_GRAPH;
	Fields.Add(pDesc);
}

void FieldList::CreateEmptyPositionField (const char* Name, const Vector& def_value)
{
	DataPosition* Field = NEW DataPosition;
	Field->SetName(Name);
	Field->SetValue(def_value);

	FieldDesc pDesc;
	pDesc.MarkForDelete = false;
	pDesc.Name = Field->GetName();
	pDesc.HashValue = string::HashNoCase(pDesc.Name.GetBuffer());
	pDesc.pPointer = Field;
	pDesc.Type = FIELD_POSITION;
	Fields.Add(pDesc);
}

void FieldList::CreateEmptyStringField (const char* Name, const char* def_value)
{
	DataString* Field = NEW DataString;
	Field->SetName(Name);
	Field->SetValue(def_value);

	FieldDesc pDesc;
	pDesc.MarkForDelete = false;
	pDesc.Name = Field->GetName();
	pDesc.HashValue = string::HashNoCase(pDesc.Name.GetBuffer());
	pDesc.pPointer = Field;
	pDesc.Type = FIELD_STRING;
	Fields.Add(pDesc);

}

void FieldList::CreateEmptyUVField (const char* Name)
{
	DataUV* Field = NEW DataUV;
	Field->SetName(Name);
	Field->SetValues(&Vector4(0.0f, 0.0f, 1.0f, 1.0f), 1);

	FieldDesc pDesc;
	pDesc.MarkForDelete = false;
	pDesc.Name = Field->GetName();
	pDesc.HashValue = string::HashNoCase(pDesc.Name.GetBuffer());
	pDesc.pPointer = Field;
	pDesc.Type = FIELD_UV;
	Fields.Add(pDesc);
}

void FieldList::CreateEmptyColorField (const char* Name, DWORD def_value)
{
	DataColor* Field = NEW DataColor;
	Field->SetName(Name);
	ColorVertex defVal[2];
	defVal[0].MinValue = def_value;
	defVal[0].MaxValue = def_value;
	defVal[1].MinValue = def_value;
	defVal[1].MaxValue = def_value;
	Field->SetValues(defVal, 2);

	FieldDesc pDesc;
	pDesc.MarkForDelete = false;
	pDesc.Name = Field->GetName();
	pDesc.HashValue = string::HashNoCase(pDesc.Name.GetBuffer());
	pDesc.pPointer = Field;
	pDesc.Type = FIELD_COLOR;
	Fields.Add(pDesc);
}


void FieldList::CreateBoolField (MemFile* pMemFile)
{
	DataBool* Field = NEW DataBool;
	Field->Load(pMemFile);

	FieldDesc pDesc;
	pDesc.MarkForDelete = false;
	pDesc.Name = Field->GetName();
	pDesc.HashValue = string::HashNoCase(pDesc.Name.GetBuffer());
	pDesc.pPointer = Field;
	pDesc.Type = FIELD_BOOL;
	Fields.Add(pDesc);
}

void FieldList::CreateFloatField (MemFile* pMemFile)
{
	DataFloat* Field = NEW DataFloat;
	Field->Load(pMemFile);

	FieldDesc pDesc;
	pDesc.MarkForDelete = false;
	pDesc.Name = Field->GetName();
	pDesc.HashValue = string::HashNoCase(pDesc.Name.GetBuffer());
	pDesc.pPointer = Field;
	pDesc.Type = FIELD_FLOAT;
	Fields.Add(pDesc);
}

void FieldList::CreateGraphField (MemFile* pMemFile)
{
	DataGraph* Field = NEW DataGraph;
	Field->Load(pMemFile);

	FieldDesc pDesc;
	pDesc.MarkForDelete = false;
	pDesc.Name = Field->GetName();
	pDesc.HashValue = string::HashNoCase(pDesc.Name.GetBuffer());
	pDesc.pPointer = Field;
	pDesc.Type = FIELD_GRAPH;
	Fields.Add(pDesc);
}

void FieldList::CreatePositionField (MemFile* pMemFile)
{
	DataPosition* Field = NEW DataPosition;
	Field->Load(pMemFile);

	FieldDesc pDesc;
	pDesc.MarkForDelete = false;
	pDesc.Name = Field->GetName();
	pDesc.HashValue = string::HashNoCase(pDesc.Name.GetBuffer());
	pDesc.pPointer = Field;
	pDesc.Type = FIELD_POSITION;
	Fields.Add(pDesc);
}

void FieldList::CreateStringField (MemFile* pMemFile)
{
	DataString* Field = NEW DataString;
	Field->Load(pMemFile);

	FieldDesc pDesc;
	pDesc.MarkForDelete = false;
	pDesc.Name = Field->GetName();
	pDesc.HashValue = string::HashNoCase(pDesc.Name.GetBuffer());
	pDesc.pPointer = Field;
	pDesc.Type = FIELD_STRING;
	Fields.Add(pDesc);
}

void FieldList::CreateUVField (MemFile* pMemFile)
{
	DataUV* Field = NEW DataUV;
	Field->Load(pMemFile);

	FieldDesc pDesc;
	pDesc.MarkForDelete = false;
	pDesc.Name = Field->GetName();
	pDesc.HashValue = string::HashNoCase(pDesc.Name.GetBuffer());
	pDesc.pPointer = Field;
	pDesc.Type = FIELD_UV;
	Fields.Add(pDesc);
}

void FieldList::CreateColorField (MemFile* pMemFile)
{
	DataColor* Field = NEW DataColor;
	Field->Load(pMemFile);

	FieldDesc pDesc;
	pDesc.MarkForDelete = false;
	pDesc.Name = Field->GetName();
	pDesc.HashValue = string::HashNoCase(pDesc.Name.GetBuffer());
	pDesc.pPointer = Field;
	pDesc.Type = FIELD_COLOR;
	Fields.Add(pDesc);
}

void FieldList::DelAll ()
{
	for (DWORD n = 0; n < Fields.Size(); n++)
	{
		DeleteFieldData (Fields[n]);
	} // loop

	Fields.DelAll();
}


DataColor* FieldList::FindColor (const char* AttrName)
{
	DWORD SearchHash = string::HashNoCase(AttrName);
	for (DWORD n = 0; n < Fields.Size(); n++)
	{
		if (Fields[n].Type == FIELD_COLOR)
		{
			if (SearchHash == Fields[n].HashValue)
			{
				if (stricmp (Fields[n].Name.GetBuffer(), AttrName) == 0)
				{
					return ((DataColor*)Fields[n].pPointer);
				}
			}
		}
	}

	return NULL;
}

DataBool* FieldList::FindBool (const char* AttrName)
{
	DWORD SearchHash = string::HashNoCase(AttrName);
	for (DWORD n = 0; n < Fields.Size(); n++)
	{
		if (Fields[n].Type == FIELD_BOOL)
		{
			if (SearchHash == Fields[n].HashValue)
			{
				if (stricmp (Fields[n].Name.GetBuffer(), AttrName) == 0)
				{
					return ((DataBool*)Fields[n].pPointer);
				}
			}
		}
	}

	return NULL;
}

DataFloat* FieldList::FindFloat (const char* AttrName)
{
	DWORD SearchHash = string::HashNoCase(AttrName);
	for (DWORD n = 0; n < Fields.Size(); n++)
	{
		if (Fields[n].Type == FIELD_FLOAT)
		{
			if (SearchHash == Fields[n].HashValue)
			{
				if (stricmp (Fields[n].Name.GetBuffer(), AttrName) == 0)
				{
					return ((DataFloat*)Fields[n].pPointer);
				}
			}
		}
	}

	return NULL;
}

DataGraph* FieldList::FindGraph (const char* AttrName)
{
	DWORD SearchHash = string::HashNoCase(AttrName);

	for (DWORD n = 0; n < Fields.Size(); n++)
	{
		if (Fields[n].Type == FIELD_GRAPH)
		{
			if (SearchHash == Fields[n].HashValue)
			{
				if (stricmp (Fields[n].Name.GetBuffer(), AttrName) == 0)
				{
					return ((DataGraph*)Fields[n].pPointer);
				}
			}
		}
	}

	return NULL;
}

DataString* FieldList::FindString (const char* AttrName)
{
	DWORD SearchHash = string::HashNoCase(AttrName);

	for (DWORD n = 0; n < Fields.Size(); n++)
	{
		if (Fields[n].Type == FIELD_STRING)
		{
			if (SearchHash == Fields[n].HashValue)
			{
				if (stricmp (Fields[n].Name.GetBuffer(), AttrName) == 0)
				{
					return ((DataString*)Fields[n].pPointer);
				}
			}
		}
	}

	return NULL;
}

DataPosition* FieldList::FindPosition (const char* AttrName)
{
	DWORD SearchHash = string::HashNoCase(AttrName);

	for (DWORD n = 0; n < Fields.Size(); n++)
	{
		if (Fields[n].Type == FIELD_POSITION)
		{
			if (SearchHash == Fields[n].HashValue)
			{
				if (stricmp (Fields[n].Name.GetBuffer(), AttrName) == 0)
				{
					return ((DataPosition*)Fields[n].pPointer);
				}
			}
		}
	}

	return NULL;
}

DataUV* FieldList::FindUV (const char* AttrName)
{
	DWORD SearchHash = string::HashNoCase(AttrName);

	for (DWORD n = 0; n < Fields.Size(); n++)
	{
		if (Fields[n].Type == FIELD_UV)
		{
			if (SearchHash == Fields[n].HashValue)
			{
				if (stricmp (Fields[n].Name.GetBuffer(), AttrName) == 0)
				{
					return ((DataUV*)Fields[n].pPointer);
				}
			}
		}
	}
	return NULL;
}

FieldList::FieldDesc* FieldList::FindField (const char* Name)
{
	DWORD SearchHash = string::HashNoCase(Name);

	for (DWORD n = 0; n < Fields.Size(); n++)
	{
			if (SearchHash == Fields[n].HashValue)
			{
				if (stricmp (Fields[n].Name.GetBuffer(), Name) == 0)
				{
					return &Fields[n];
				}
			}
	}
	return NULL;
}

float FieldList::GetFloat (const char* AttrName, float def_value)
{
	DataFloat* pFind = FindFloat(AttrName);
	if (!pFind) return def_value;
	return pFind->GetValue();
}

int FieldList::GetFloatAsInt (const char* AttrName, int def_value)
{
	float val = GetFloat(AttrName, (float)def_value);
	return (int)val;
}


bool FieldList::GetBool (const char* AttrName, bool def_value)
{
	DataBool* pFind = FindBool(AttrName);
	if (!pFind) return def_value;
	return pFind->GetValue();
}

const char* FieldList::GetString (const char* AttrName, const char* def_value)
{
	DataString* pFind = FindString(AttrName);
	if (!pFind) return def_value;
	return pFind->GetValue();
}

const Vector& FieldList::GetPosition (const char* AttrName, const Vector& def_value)
{
	DataPosition* pFind = FindPosition(AttrName);
	if (!pFind) return def_value;
	return pFind->GetValue();
}

float FieldList::GetGraphVal (const char* AttrName, float Time, float LifeTime, float K_Rand, float def_value)
{
	DataGraph* pFind = FindGraph(AttrName);
	if (!pFind) return def_value;
	return pFind->GetValue(Time, LifeTime, K_Rand);
}

float FieldList::GetRandomGraphVal (const char* AttrName, float Time, float LifeTime, float def_value)
{
	DataGraph* pFind = FindGraph(AttrName);
	if (!pFind) return def_value;
	return pFind->GetRandomValue(Time, LifeTime);
}

DWORD FieldList::GetFieldCount ()
{
	return Fields.Size();
}

const FieldList::FieldDesc& FieldList::GetFieldByIndex (DWORD Index)
{
	return Fields[Index];
}


void FieldList::Convert (DataDescripion* pDataDescriptor)
{
	for (DWORD i = 0; i < Fields.Size(); i++)
	{
		Fields[i].MarkForDelete = true;
	}

	DWORD NeedFieldsCount = pDataDescriptor->GetFieldCount();

	for (DWORD n = 0; n < NeedFieldsCount; n++)
	{
		const char* NeedFieldName = pDataDescriptor->GetFieldName(n);
		FieldType NeedFieldType = pDataDescriptor->GetFieldType(n);
		FieldDesc* pDesc = FindField (NeedFieldName);
		bool FieldExist = false;
		if (pDesc)
		{
			if (pDesc->Type == NeedFieldType)
			{
				//Если нашли поле и оно нужного типа, убираем пометку об удалении
				pDesc->MarkForDelete = false;
				FieldExist = true;
			}
		}

		//Если поле не существует, надо его создать...
		if (!FieldExist)
		{
			switch (NeedFieldType)
			{
			case FIELD_COLOR:
				CreateEmptyColorField(NeedFieldName, 0xFFFFA030);
				break;
			case FIELD_BOOL:
				CreateEmptyBoolField(NeedFieldName, false);
				break;
			case FIELD_FLOAT:
				CreateEmptyFloatField(NeedFieldName, 0.0f);
				break;
			case FIELD_GRAPH:
				CreateEmptyGraphField(NeedFieldName, 0.0f, 0.0f);
				break;
			case FIELD_POSITION:
				CreateEmptyPositionField(NeedFieldName, Vector(0.0f));
				break;
			case FIELD_UV:
				CreateEmptyUVField(NeedFieldName);
				break;
			case FIELD_STRING:
				CreateEmptyStringField(NeedFieldName, "###");
				break;
			default:
				THROW ("Unknown Field type !!!");
			}
		}
	}

	for (i = 0; i < Fields.Size(); i++)
	{
		if (Fields[i].MarkForDelete)
		{
			DeleteFieldData (Fields[i]);
			Fields.ExtractNoShift(i);
			i--;
		}
	}

}

void FieldList::DeleteFieldData (const FieldList::FieldDesc& pData)
{
	FieldType fldType = pData.Type;

	switch (fldType)
	{
	case FIELD_BOOL:
		{
			delete ((DataBool*)pData.pPointer);
			break;
		}
	case FIELD_FLOAT:
		{
			delete ((DataFloat*)pData.pPointer);
			break;
		}
	case FIELD_GRAPH:
		{
			delete ((DataGraph*)pData.pPointer);
			break;
		}
	case FIELD_POSITION:
		{
			delete ((DataPosition*)pData.pPointer);
			break;
		}
	case FIELD_STRING:
		{
			delete ((DataString*)pData.pPointer);
			break;
		}
	case FIELD_UV:
		{
			delete ((DataUV*)pData.pPointer);
			break;
		}
	case FIELD_COLOR:
		{
			delete ((DataColor*)pData.pPointer);
			break;
		}
	default:
		{
			_THROW ("Particles: Try delete unknown field type !!!!");
		}
	}	//switch
}


void FieldList::Write (MemFile* File)
{
	DWORD DataFieldsCount = Fields.Size();
	File->WriteType(DataFieldsCount);


	for (DWORD n = 0; n < DataFieldsCount; n++)
	{
		File->WriteType(Fields[n].Type);

		switch (Fields[n].Type)
		{
			case FIELD_BOOL:
				{
					DataBool* pBoolField = (DataBool*)Fields[n].pPointer;
					pBoolField->Write (File);
					break;
				}
			case FIELD_FLOAT:
				{
					DataFloat* pFloatField = (DataFloat*)Fields[n].pPointer;
					pFloatField->Write (File);
					break;
				}
			case FIELD_GRAPH:
				{
					DataGraph* pGraphField = (DataGraph*)Fields[n].pPointer;
					pGraphField->Write (File);
					break;
				}
			case FIELD_POSITION:
				{
					DataPosition* pPositionField = (DataPosition*)Fields[n].pPointer;
					pPositionField->Write (File);
					break;
				}
			case FIELD_STRING:
				{
					DataString* pStringField = (DataString*)Fields[n].pPointer;
					pStringField->Write (File);
					break;
				}
			case FIELD_UV:
				{
					DataUV* pUVField = (DataUV*)Fields[n].pPointer;
					pUVField->Write (File);
					break;
				}
			case FIELD_COLOR:
				{
					DataColor* pColorField = (DataColor*)Fields[n].pPointer;
					pColorField->Write (File);
					break;
				}
			default:
				{
					_THROW ("Particles: Unknown field type !!!!");
				}
		}	//switch
	} // loop

}

