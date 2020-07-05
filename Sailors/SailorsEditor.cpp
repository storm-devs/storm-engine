
#include "SailorsEditor.h"

#include "..\ENGINE\PROGRAM\SEA_AI\script_defines.h"
#include "..\common_h\math3d\matrix.h"



SailorsEditor :: SailorsEditor()
{
	cameraAng = 0.0f;

	cameraAng.x= PI/4;
	cameraAng.y= -PI/3+PI;

	cameraTo=  CVECTOR(0.0f,5,0.0f);
	cameraPos= CVECTOR(0.0f,30.0f,0.0f);
};



SailorsEditor :: ~SailorsEditor()
{
	api->DeleteEntity(sailors);	
	api->DeleteEntity(shipID);
};


bool SailorsEditor :: Init()
{
	rs = (VDX8RENDER *) _CORE_API->CreateService("dx8render");

	bool result= api->CreateEntity(&sailors, "Sailors");


	_CORE_API->LayerCreate("execute", true, false);
	_CORE_API->LayerSetFlags("execute", LRFLAG_EXECUTE);
	_CORE_API->LayerAdd("execute", GetID(), 0);

	api->LayerCreate("editor_realize", true, false);
	api->LayerSetFlags("editor_realize", LRFLAG_REALIZE);
	api->LayerAdd("editor_realize", GetID(), 100000);

	LoadFromIni("SailorsEditor.ini");

	api->CreateEntity(&shipID,"MODELR");
	api->Send_Message(shipID,"ls",MSG_MODEL_LOAD_GEO, _shipName);

	api->LayerAdd("editor_realize", shipID, 100000);
	model = (MODEL*)api->GetEntityPointer(&shipID);



	model->mtx.BuildMatrix(CVECTOR(0.0f),CVECTOR(0.0f, 0.0f, 0.0f));

	long ctrl = api->Controls->CreateControl("DeltaMouseH");
	api->Controls->MapControl(ctrl, 256);
	ctrl = api->Controls->CreateControl("DeltaMouseV");
	api->Controls->MapControl(ctrl, 257);


	menu.sailrs= (Sailors *)api->GetEntityPointer(&sailors);

	menu.sailrs->editorMode= true;

	api->Send_Message(sailors,"lis",AI_MESSAGE_ADD_SHIP,shipID,"");

	menu.Update(menu.sailrs->shipWalk[0].sailorsPoints);

	return result;
};


void SailorsEditor :: Execute(dword dltTime)
{
	SetCamera(dltTime);
	menu.OnKeyPress(menu.sailrs->shipWalk[0].sailorsPoints);

	if (GetAsyncKeyState(VK_ESCAPE)< 0)
	ExitProcess(0);
};


void SailorsEditor :: Realize(dword dltTime)
{
	menu.Draw(rs, menu.sailrs->shipWalk[0].sailorsPoints);

	//rs->Print(rs->GetCurFont(), D3DCOLOR_XRGB(100,100,100) ,700, 10,"%f" , 1000.0f/float(dltTime));

	if (menu.blocked== 1 || menu.blocked== 2)
    menu.sailrs->shipWalk[0].sailorsPoints.Draw(rs, (menu.selected== 1 && menu.blocked== 1));

	if (menu.blocked< 1)
		menu.sailrs->shipWalk[0].sailorsPoints.Draw_(rs, (menu.selected== 1 && menu.blocked== 1));
};


void SailorsEditor :: SetCamera(dword &dltTime){

	CONTROL_STATE cs;
	api->Controls->GetControlState("DeltaMouseV",cs);
	cameraAng.x += -cs.fValue*0.001f;
	if(cameraAng.x < 0.01f) cameraAng.x = 0.01f;
	if(cameraAng.x > 1.57f)  cameraAng.x = 1.57f;

	api->Controls->GetControlState("DeltaMouseH",cs);
	cameraAng.y += cs.fValue*0.001f;



	CVECTOR pos;

	CMatrix mtx(cameraAng);
	pos= mtx*cameraPos;

	float speed= 0;
	if(GetAsyncKeyState(VK_LBUTTON)< 0) speed= -0.1f;
	if(GetAsyncKeyState(VK_RBUTTON)< 0) speed=  0.1f;


	cameraPos.y += speed*(dltTime/10.0f);
	rs->SetCamera(cameraTo+ pos, cameraTo,CVECTOR(0.0f,1.0f,0.0f));



	if(GetAsyncKeyState(0x57)< 0){

		cameraTo.x-= sin(cameraAng.y)*dltTime/50.0f;
		cameraTo.z-= cos(cameraAng.y)*dltTime/50.0f;
	};

	if(GetAsyncKeyState(0x53)< 0){

		cameraTo.x+= sin(cameraAng.y)*dltTime/50.0f;
		cameraTo.z+= cos(cameraAng.y)*dltTime/50.0f;
	};

	if(GetAsyncKeyState(0x41)< 0){

		cameraTo.x+= sin(cameraAng.y+ PI/2)*dltTime/50.0f;
		cameraTo.z+= cos(cameraAng.y+ PI/2)*dltTime/50.0f;
	};

	if(GetAsyncKeyState(0x44)< 0){

		cameraTo.x-= sin(cameraAng.y+ PI/2)*dltTime/50.0f;
		cameraTo.z-= cos(cameraAng.y+ PI/2)*dltTime/50.0f;
	};

	menu.cameraAng= cameraAng;
	menu.cameraPos= cameraTo;

	menu.dltTime= dltTime;
};


void SailorsEditor :: LoadFromIni(string fileName)
{
	char param[256];

	INIFILE *pIni = fio->OpenIniFile(fileName);

	if( !pIni ) {
		api->Trace( "Sailors : Can`t open '%s'", fileName);
		return;
	}

	pIni->ReadString("PATH","ship",param,sizeof(param)-1); 
	_shipName= param;

	pIni->ReadString("PATH","filename_save",param,sizeof(param)-1); 
	menu._fileName_save= param;
	
	pIni->ReadString("PATH","filename_load",param,sizeof(param)-1); 
	menu._fileName_load= param;

	delete pIni;
};