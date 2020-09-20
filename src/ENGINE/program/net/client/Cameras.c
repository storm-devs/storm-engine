object	NCShipCamera, NCDeckCamera;

void NetClient_DeleteCamerasEnvironment()
{
	DeleteClass(&NCDeckCamera);
	DeleteClass(&NCShipCamera);

	DelEventHandler("NetClient_DeckCameraUpdate", "NetClient_DeckCameraUpdate");
	DelEventHandler("NetClient_SetDieCameraEvent", "NetClient_SetDieCameraEvent");
}

void NetClient_CreateCamerasEnvironment(int iMsg)
{
	// for init
	NCDeckCamera.Perspective = 1.285;

	NCDeckCamera.Server = false;
	NCShipCamera.Server = false;
	CreateEntity(&NCDeckCamera, "NetDeckCamera");
	CreateEntity(&NCShipCamera, "NetShipCamera");

	LayerAddObject("net_execute", &NCDeckCamera, iClientShipPriorityExecute - 5);
	LayerAddObject("net_realize", &NCDeckCamera, 1000000);

	LayerAddObject("net_execute", &NCShipCamera, iClientShipPriorityExecute + 5);
	LayerAddObject("net_realize", &NCShipCamera, 1000000);

	// DeckCamera paramerets
	NCDeckCamera.Perspective = 1.285;
	NCDeckCamera.ZoomPerspective = 0.2;
	NCDeckCamera.SensivityDistance = 0.01;
	NCDeckCamera.SensivityHeightAngle = 0.0015;
	NCDeckCamera.SensivityAzimuthAngle = 0.0015;
	NCDeckCamera.StepUp = 0.5;
	NCDeckCamera.StepMin = 0.01;
	NCDeckCamera.h_max = 2.0;
	NCDeckCamera.h_min = 0.5;
	NCDeckCamera.h_step = 0.1;
	NCDeckCamera.h_def = 1.6;
	NCDeckCamera.MaxAngleX = 1.5;
	NCDeckCamera.MinAngleX = -1.5;
	NCDeckCamera.RockingX = 0.8;
	NCDeckCamera.RockingZ = 0.8;
	NCDeckCamera.CurrentTargetRelation = 0;

	NCDeckCamera.Crosshair.Texture = "crosshair\crosshair.tga";
	NCDeckCamera.Crosshair.Size = 0.05;
	NCDeckCamera.Crosshair.Technique = "Crosshair";
	NCDeckCamera.Crosshair.SubTexX = 1;
	NCDeckCamera.Crosshair.SubTexY = 1;
	NCDeckCamera.Crosshair.Colors.Default = argb(0, 255, 255, 255);
	NCDeckCamera.Crosshair.Colors.Enemy = argb(0, 255, 0, 0);
	NCDeckCamera.Crosshair.Colors.Friend = argb(0, 0, 255, 0);
	NCDeckCamera.Crosshair.Colors.Neutral = argb(0, 128, 128, 128);

	NCDeckCamera.Telescope.Texture = "Eye_BestTub.tga";
	NCDeckCamera.Telescope.Technique = "Telescope";

	NCDeckCamera.Pos.x = 0.0;
	NCDeckCamera.Pos.y = 0.0;
	NCDeckCamera.Pos.z = 0.0;
	NCDeckCamera.Dir.x = 0.0;
	NCDeckCamera.Dir.y = 1.0;
	NCDeckCamera.Dir.z = 0.0;
	NCDeckCamera.IsCanFire = 0;

	// ShipCamera paramerets
	NCShipCamera.Perspective = 1.285;
	NCShipCamera.SensivityDistance = 30.0;
	NCShipCamera.SensivityHeightAngle = 0.02;
	NCShipCamera.SensivityHeightAngleOnShip = 0.005;
	NCShipCamera.SensivityAzimuthAngle = 0.04;
	NCShipCamera.MaxAngleX = 0.07;
	NCShipCamera.MinAngleX = -1.4;
	NCShipCamera.Distance = 69.0;
	NCShipCamera.MinDistance = 25.0;
	NCShipCamera.MaxDistance = 90.0;
	NCShipCamera.MinHeightOnSea = 1.0;
	NCShipCamera.MaxHeightOnShip = 16.0;
	NCShipCamera.InvertMouseX = 1.0;
	NCShipCamera.InvertMouseY = -1.0;

	SetEventHandler("NetClient_DeckCameraUpdate", "NetClient_DeckCameraUpdate", 0);
	PostEvent("NetClient_DeckCameraUpdate", 100);

	NetClient_CamerasSetNetID(wClientID);
	NetClient_CamerasSetActive(true);
	NetClient_SetDeckCameraOn();
}

void NetClient_CamerasSetNetID(int wNetID)
{
	NCDeckCamera.NetID = wNetID;
	NCShipCamera.NetID = wNetID;
}

void NetClient_DeckCameraUpdate()
{
	if (sti(NCDeckCamera.On) && sti(NCDeckCamera.Active))
	{
		int iSMsg = NMCreate();
		NMAddClientID(iSMsg, wClientID);
		NMAddByte(iSMsg, NC_CAMERA);
		NMAddByte(iSMsg, NSC_DECK_CAMERA_UPDATE);
		NMAddFloat(iSMsg, stf(NCDeckCamera.Dir.x));
		NMAddFloat(iSMsg, stf(NCDeckCamera.Dir.y));
		NMAddFloat(iSMsg, stf(NCDeckCamera.Dir.z));
		NetClient_SendMessage(iSMsg, false);
		NMDelete(iSMsg);
	}

	PostEvent("NetClient_DeckCameraUpdate", 250);
}

void NetClient_OnCamera(int iSMsg)
{
	int iSubCode = NMGetByte(iSMsg);

	switch (iSubCode)
	{
		case NSC_DECK_CAMERA_UPDATE: 
			NCDeckCamera.IsCanFire = NMGetByte(iSMsg);
			NCDeckCamera.CurrentTargetRelation = NMGetByte(iSMsg);
		break;
	}
}

void NetClient_CamerasSetActive(bool bActive)
{
	NCDeckCamera.Active = bActive;
	NCShipCamera.Active = bActive;
}

void NetClient_SetDieCameraEvent()
{
	DelEventHandler("NetClient_SetDieCameraEvent", "NetClient_SetDieCameraEvent");

	int wKillerID = GetEventData();
	NCShipCamera.NetID = wKillerID;
}

void NetClient_SetDieCamera(int wKillerID)
{
	SetEventHandler("NetClient_SetDieCameraEvent", "NetClient_SetDieCameraEvent", 0);
	PostEvent("NetClient_SetDieCameraEvent", 10000, "l", wKillerID);
	
	NetClient_SetShipCameraOn();
}

void NetClient_SetDeckCameraOn()
{
	NCShipCamera.On = false;
	NCDeckCamera.On = true;
}

void NetClient_SetShipCameraOn()
{
	NCDeckCamera.On = false;
	NCShipCamera.On = true;
}

void NetClient_CameraSwitch()
{
	ref rClient = NetClient_GetClient(wClientID);

	if (NetClient_IsClientDead(rClient))
	{
		DelEventHandler("NetClient_SetDieCameraEvent", "NetClient_SetDieCameraEvent");
		NetClient_SetShipCameraOn();
		// find next ClientID
		return;
	}

	if (sti(NCDeckCamera.On))
		{ 
			Telescope_Off();
			NetClient_SetShipCameraOn(); 
		}
	else
		{ NetClient_SetDeckCameraOn(); }
}
