#include "CameraDialog.h"
#include "..\common_h\model.h"

INTERFACE_FUNCTION
CREATE_CLASS(CameraDialog)

#define HDISP1	1.5f
#define FOV				2.25f

#include "..\common_h\matrix.h"
PLANE planes[6];
void FindPlanes(const CMatrix &view, const CMatrix &proj);

CameraDialog::CameraDialog()
{
	pos = 0.0f;
	ang = 0.0f;
	fov = FOV;
	//ShowCursor(false);

	track = 0;
	track = 0;
	time = 0.0f;
}

CameraDialog::~CameraDialog()
{
	_CORE_API->FreeService("coll");
//	ShowCursor(true);
	if(track)	delete track;
}

bool CameraDialog::Init()
{
	GUARD(CAMERA::CAMERA())

	rs = (VDX8RENDER *)_CORE_API->CreateService("dx8render");
	if(!rs)	_THROW("No service: dx8render");

	col = (COLLIDE *)_CORE_API->CreateService("coll");
	if(!col)	_THROW("No service: collide");

	_CORE_API->LayerAdd("execute",GetID(),2);
	//_CORE_API->LayerAdd("realize",GetID(),1);
	
	UNGUARD
	return true;
}

inline float LerpAng(float a, float b, float k)
{
	static const float PI = 3.1415926535f;


	if(a<0)	a += PI*2.0f;
	if(b<0)	b += PI*2.0f;

	float delta = b - a;

	if(delta < -PI) delta = 2*PI + delta;
	if(delta >  PI) delta =-2*PI + delta;

	return a + k*delta;//*/

	/*/Приведём углы к диапакзону -PI..PI
	a = PI*((a*(1.0f/PI)) - long(a*(1.0f/PI)));
	b = PI*((b*(1.0f/PI)) - long(b*(1.0f/PI)));
	//Если разница <= PI то просто интерполируем
	if(fabsf(b - a) <= PI) return a + k*(b - a);
	//Приведём к диапазону 0..2*PI или 0..-2*PI
	if(b < 0) b += 2.0f*PI; else b -= 2.0f*PI;
	//Проинтерполируем линейно
	return a + k*(b - a);
	//*/
}

void CameraDialog::Execute(dword Delta_Time)
{
	if(track==0)	return;

	float dtime = 1.0f/float(_CORE_API->EngineFps());
	time += Delta_Time*0.001f;
	//dtime = 0.001f*_CORE_API->GetFloatDeltaTime();

	CVECTOR prevPos = pos;
	CVECTOR prevAng = ang;

	CVECTOR perPos, perAng;
	MODEL *mdl = (MODEL*)_CORE_API->GetEntityPointer(&person);
	if(mdl==0)	return;

	CMatrix perMtx = mdl->mtx;

	ATTRIBUTES *atr = api->Entity_GetAttributeClass(&personId,"act.type");
	if(atr==0)	perMtx.Pos().y += HDISP1;
	else
	{
		const char *mdlType = atr->GetThisAttr();
		if(mdlType==0)	perMtx.Pos().y += HDISP1;
		else
			if(strcmpi(mdlType, "sit")==0)	perMtx.Pos().y += 1.2f;
			else	if(strcmpi(mdlType, "gov")==0)	perMtx.Pos().y += 1.0f;
						else	perMtx.Pos().y += HDISP1;
	}

	//--------------------------------------------------------------------------------

	float FPS = 15.0f;
	long fr = long(floorf(FPS*time))%(frames-1);
	float u = FPS*time - floorf(FPS*time);

	pos.x = -(track[fr].pos.x*(1.0f-u) + track[fr+1].pos.x*u);
	pos.y = (track[fr].pos.y*(1.0f-u) + track[fr+1].pos.y*u);
	pos.z = (track[fr].pos.z*(1.0f-u) + track[fr+1].pos.z*u);

	ang.x = -(LerpAng(track[fr].ang.x, track[fr+1].ang.x, u));
	ang.y = -(LerpAng(track[fr].ang.y, track[fr+1].ang.y, u)) - 3.1415926535897932384f;
	ang.z = -(LerpAng(track[fr].ang.z, track[fr+1].ang.z, u));

	CMatrix cm = CMatrix(ang.x, ang.y, ang.z, pos.x, pos.y, pos.z) * perMtx;
	rs->SetCamera(cm.Pos(), cm.Pos() + cm.Vz(), CVECTOR(0.0f, 1.0f, 0.0f));
}


void CameraDialog::Realize(dword Delta_Time)
{
	MODEL *mdl = (MODEL*)_CORE_API->GetEntityPointer(&person);
	if(mdl==0)	return;
	//-------------------------------------------------------
	static bool inited = false;
	static ENTITY_ID sMod[2];
	if(!inited)
	{
		_CORE_API->CreateEntity(&sMod[0], "modelr");
		_CORE_API->LayerAdd("execute",sMod[0],100);
		_CORE_API->LayerAdd("realize",sMod[0],100);
		_CORE_API->Send_Message(sMod[0], "ls", MSG_MODEL_LOAD_GEO, "sabel");
		_CORE_API->CreateEntity(&sMod[1], "modelr");
		_CORE_API->LayerAdd("execute",sMod[1],100);
		_CORE_API->LayerAdd("realize",sMod[1],100);
		_CORE_API->Send_Message(sMod[1], "ls", MSG_MODEL_LOAD_GEO, "sabel");
		inited = true;
	}
	//draw locators
	NODE *root = mdl->GetNode(0);
	long sti = -1;
	long idSiber = root->geo->FindName("fall");
	if((sti = root->geo->FindLabelN(sti+1, idSiber))>-1)
	{
		Animation *ani = mdl->GetAnimation();
		CMatrix *bones = &ani->GetAnimationMatrix(0);

		GEOS::LABEL lb;
		root->geo->GetLabel(sti, lb);
		CMatrix mt;
		mt.Vx() = CVECTOR(lb.m[0][0], lb.m[0][1], lb.m[0][2]);
		mt.Vy() = CVECTOR(lb.m[1][0], lb.m[1][1], lb.m[1][2]);
		mt.Vz() = CVECTOR(lb.m[2][0], lb.m[2][1], lb.m[2][2]);
		mt.Pos() = CVECTOR(lb.m[3][0], lb.m[3][1], lb.m[3][2]);

		CMatrix bn = bones[lb.bones[0]];
		CMatrix mbn = mt*bn;
		mbn.Pos().x *= -1.0f;
		mbn.Vx().x *= -1.0f;
		mbn.Vy().x *= -1.0f;
		mbn.Vz().x *= -1.0f;
		CMatrix perMtx = mbn*mdl->mtx;
		MODEL *smdl = (MODEL*)_CORE_API->GetEntityPointer(&sMod[0]);
		smdl->mtx = perMtx;
	}

	sti = -1;
	idSiber = root->geo->FindName("sabel");
	if((sti = root->geo->FindLabelN(sti+1, idSiber))>-1)
	{
		Animation *ani = mdl->GetAnimation();
		CMatrix *bones = &ani->GetAnimationMatrix(0);

		GEOS::LABEL lb;
		root->geo->GetLabel(sti, lb);
		CMatrix mt;
		mt.Vx() = CVECTOR(lb.m[0][0], lb.m[0][1], lb.m[0][2]);
		mt.Vy() = CVECTOR(lb.m[1][0], lb.m[1][1], lb.m[1][2]);
		mt.Vz() = CVECTOR(lb.m[2][0], lb.m[2][1], lb.m[2][2]);
		mt.Pos() = CVECTOR(lb.m[3][0], lb.m[3][1], lb.m[3][2]);

		CMatrix bn = bones[lb.bones[0]];
		CMatrix mbn = mt*bn;
		mbn.Pos().x *= -1.0f;
		mbn.Vx().x *= -1.0f;
		mbn.Vy().x *= -1.0f;
		mbn.Vz().x *= -1.0f;
		CMatrix perMtx = mbn*mdl->mtx;
		MODEL *smdl = (MODEL*)_CORE_API->GetEntityPointer(&sMod[1]);
		smdl->mtx = perMtx;
	}

}


#include <stdio.h>
dword _cdecl CameraDialog::ProcessMessage(MESSAGE &msg)
{
	switch(msg.Long())
	{
		//get entity id of person
		case 0:
			time = 0.0f;
			person = msg.EntityID();
			personId = msg.EntityID();
		break;
		//get camera track name
		case 1:
		{
			time = 0.0f;
			char trackName[256];
			msg.String(sizeof trackName, trackName);
			if(track!=0)	delete track;

			char fname[256];
			strcpy(fname, "resource\\animation\\");
			strcat(fname, trackName);
			strcat(fname, ".cam");
			//loading animation
			HANDLE cam = _CORE_API->fio->_CreateFile(fname);
			long nbytes = _CORE_API->fio->_SetFilePointer(cam, 0, 0, FILE_END);
			_CORE_API->fio->_SetFilePointer(cam, 0, 0, FILE_BEGIN);
			frames = nbytes/6/sizeof(float);
			track = NEW ANIFRAME[frames];
			_CORE_API->fio->_ReadFile(cam, track, nbytes, 0);
			_CORE_API->fio->_CloseHandle(cam);
		}
		break;
	}
	return 0;
}

void FindPlanes(const CMatrix &view, const CMatrix &proj)
{
	CVECTOR v[4];
	//left
	v[0].x = proj.m[0][0]; v[0].y = 0.0f; v[0].z = 1.0f;
	//right
	v[1].x = -proj.m[0][0]; v[1].y = 0.0f; v[1].z = 1.0f;		
	//top
	v[2].x = 0.0f; v[2].y = -proj.m[1][1]; v[2].z = 1.0f;	
	//bottom	
	v[3].x = 0.0f; v[3].y = proj.m[1][1]; v[3].z = 1.0f;
	v[0] = !v[0];
	v[1] = !v[1];
	v[2] = !v[2];
	v[3] = !v[3];

	CVECTOR pos;

	pos.x = -view.m[3][0]*view.m[0][0] - view.m[3][1]*view.m[0][1] - view.m[3][2]*view.m[0][2];
	pos.y = -view.m[3][0]*view.m[1][0] - view.m[3][1]*view.m[1][1] - view.m[3][2]*view.m[1][2];
	pos.z = -view.m[3][0]*view.m[2][0] - view.m[3][1]*view.m[2][1] - view.m[3][2]*view.m[2][2];

	planes[0].Nx = v[0].x*view.m[0][0] + v[0].y*view.m[0][1] + v[0].z*view.m[0][2];
	planes[0].Ny = v[0].x*view.m[1][0] + v[0].y*view.m[1][1] + v[0].z*view.m[1][2];
	planes[0].Nz = v[0].x*view.m[2][0] + v[0].y*view.m[2][1] + v[0].z*view.m[2][2];

	planes[1].Nx = v[1].x*view.m[0][0] + v[1].y*view.m[0][1] + v[1].z*view.m[0][2];
	planes[1].Ny = v[1].x*view.m[1][0] + v[1].y*view.m[1][1] + v[1].z*view.m[1][2];
	planes[1].Nz = v[1].x*view.m[2][0] + v[1].y*view.m[2][1] + v[1].z*view.m[2][2];

	planes[2].Nx = v[2].x*view.m[0][0] + v[2].y*view.m[0][1] + v[2].z*view.m[0][2];
	planes[2].Ny = v[2].x*view.m[1][0] + v[2].y*view.m[1][1] + v[2].z*view.m[1][2];
	planes[2].Nz = v[2].x*view.m[2][0] + v[2].y*view.m[2][1] + v[2].z*view.m[2][2];

	planes[3].Nx = v[3].x*view.m[0][0] + v[3].y*view.m[0][1] + v[3].z*view.m[0][2];
	planes[3].Ny = v[3].x*view.m[1][0] + v[3].y*view.m[1][1] + v[3].z*view.m[1][2];
	planes[3].Nz = v[3].x*view.m[2][0] + v[3].y*view.m[2][1] + v[3].z*view.m[2][2];

	planes[0].Nx = -planes[0].Nx;	planes[0].Ny = -planes[0].Ny;	planes[0].Nz = -planes[0].Nz;
	planes[1].Nx = -planes[1].Nx;	planes[1].Ny = -planes[1].Ny;	planes[1].Nz = -planes[1].Nz;
	planes[2].Nx = -planes[2].Nx;	planes[2].Ny = -planes[2].Ny;	planes[2].Nz = -planes[2].Nz;
	planes[3].Nx = -planes[3].Nx;	planes[3].Ny = -planes[3].Ny;	planes[3].Nz = -planes[3].Nz;

	planes[0].D = (pos.x*planes[0].Nx + pos.y*planes[0].Ny + pos.z*planes[0].Nz);
	planes[1].D = (pos.x*planes[1].Nx + pos.y*planes[1].Ny + pos.z*planes[1].Nz);
	planes[2].D = (pos.x*planes[2].Nx + pos.y*planes[2].Ny + pos.z*planes[2].Nz);
	planes[3].D = (pos.x*planes[3].Nx + pos.y*planes[3].Ny + pos.z*planes[3].Nz);

}
