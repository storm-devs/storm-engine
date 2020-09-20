void NetClient_DeleteBallsEnvironment()
{
	DeleteClass(&NCBalls);

	DelEventHandler("NetClient_BallFlyNearCamera", "NetClient_BallFlyNearCamera");
}

void NetClient_CreateBallsEnvironment(int iMsg)
{
	aref	Grapes, Knippels, Balls, Bombs;

	NCBalls.Server = false;
	CreateEntity(&NCBalls, "NetBalls");

	NCBalls.SpeedMultiply = NMGetFloat(iMsg);
	NCBalls.Texture = "AllBalls.tga";
	NCBalls.SubTexX = 2;
	NCBalls.SubTexY = 2;

	NCBalls.BallFlySoundDistance = 5.0;
	NCBalls.BallFlySoundStereoMultiplyer = 1.0;

	makearef(Grapes, NCBalls.Balls.Grapes);
	makearef(Knippels, NCBalls.Balls.Knippels);
	makearef(Balls, NCBalls.Balls.Balls);
	makearef(Bombs, NCBalls.Balls.Bombs);

	Bombs.SubTexIndex = 0;		Bombs.Size = 0.3;		Bombs.GoodIndex = Net_GetGoodIndex("Bombs");		Bombs.Particle = "bomb_smoke";
	Grapes.SubTexIndex = 1;		Grapes.Size = 1.0;		Grapes.GoodIndex = Net_GetGoodIndex("Grapes");
	Balls.SubTexIndex = 2;		Balls.Size = 0.2;		Balls.GoodIndex = Net_GetGoodIndex("Balls");
	Knippels.SubTexIndex = 3;	Knippels.Size = 0.2;	Knippels.GoodIndex = Net_GetGoodIndex("Knippels");

	NCBalls.IsDone = true;

	LayerAddObject("net_execute", &NCBalls, -1);
	LayerAddObject("net_realize", &NCBalls, 65532);

	SetEventHandler("NetClient_BallFlyNearCamera", "NetClient_BallFlyNearCamera", 0);
}

void NetClient_BallFlyNearCamera()
{
	float x = GetEventData();
	float y = GetEventData();
	float z = GetEventData();

	Play3DSound("fly_ball", x, y, z);
}

void NetClient_OnBall(int iMsg)
{
	float x, y, z;
	int iSubCode = NMGetByte(iMsg);
	int iBallIndex = NMGetWord(iMsg);

	switch (iSubCode) 
	{
		case NSC_BALL_ADD:
			NetClient_OnBallAdd(iMsg, iBallIndex);
		break;
		case NSC_BALL_DEL:
			NCBalls.ClientDel = iBallIndex;
		break;
		case NSC_BALL_HIT_WATER:
			NetClient_OnBallHitWater(iMsg, iBallIndex);
		break;
		case NSC_BALL_HIT_SAIL:
			NetClient_OnBallHitSail(iMsg, iBallIndex);
		break;
		case NSC_BALL_HIT_HULL:
			NetClient_OnBallHitHull(iMsg, iBallIndex);
		break;
		case NSC_BALL_HIT_MAST:
			NetClient_OnBallHitMast(iMsg, iBallIndex);
		break;
		case NSC_BALL_HIT_FORT:
			NetClient_OnBallHitFort(iMsg, iBallIndex);
		break;
		case NSC_BALL_HIT_ISLAND:
			NetClient_OnBallHitIsland(iMsg, iBallIndex);
		break;
	}
}

void NetClient_OnBallAdd(int iMsg, int iBallIndex)
{
	EntityUpdate(0);

	NCBalls.Time = NMGetDword(iMsg);
	int iBallType = NMGetByte(iMsg);
	float x = NMGetFloat(iMsg);
	float y = NMGetFloat(iMsg);
	float z = NMGetFloat(iMsg);
	NCBalls.x = x;
	NCBalls.y = y;
	NCBalls.z = z;

	NCBalls.SpdV0 = NMGetFloat(iMsg);
	NCBalls.Dir = NMGetFloat(iMsg);
	NCBalls.Ang = NMGetFloat(iMsg);

	NCBalls.Index = iBallIndex;

	ref rBall = Net_GetGoodByIndex(iBallType);
	NCBalls.Type = rBall.Name;

	EntityUpdate(1);

	CreateParticleSystem("cancloud_fire", x, y, z, -stf(NCBalls.Ang), stf(NCBalls.Dir), 0.0, 5);
	Play3DSound("cannon_fire", x, y, z);

	NCBalls.ClientAdd = "";
}

void NetClient_OnBallHitWater(int iMsg, int iBallIndex)
{
	float x = NMGetFloat(iMsg); 
	float y = NMGetFloat(iMsg);	
	float z = NMGetFloat(iMsg);

	CreateParticleSystem("splash", x, y, z, 0.0, 0.0, 0.0, 5);	
	Play3DSound("ball_splash", x, y, z);

	NCBalls.ClientDel = iBallIndex;
}

void NetClient_OnBallHitSail(int iMsg, int iBallIndex)
{
	int wHitID = NMGetWord(iMsg);	// FIX-ME
	int bSailIdx = NMGetByte(iMsg);
	float x = NMGetFloat(iMsg); 
	float y = NMGetFloat(iMsg);	
	float z = NMGetFloat(iMsg);
	// SendMessage 2 sail
	Play3DSound("sail_damage", x, y, z);
}

void NetClient_OnBallHitIsland(int iMsg, int iBallIndex)
{
	NCBalls.ClientDel = iBallIndex;
}

void NetClient_OnBallHitFort(int iMsg, int iBallIndex)
{
	float x = NMGetFloat(iMsg); 
	float y = NMGetFloat(iMsg);	
	float z = NMGetFloat(iMsg);

	CreateBlast(x, y, z);
	Play3DSound("ship_explosion", x, y, z);

	NCBalls.ClientDel = iBallIndex;
}

void NetClient_OnBallHitMast(int iMsg, int iBallIndex)
{
	int wOurClientID = NMGetClientID(iMsg);
	int wBallClientID = NMGetClientID(iMsg);
	float x = NMGetFloat(iMsg);
	float y = NMGetFloat(iMsg);
	float z = NMGetFloat(iMsg);
	int iDamageType = NMGetByte(iMsg);
	int iMastNum = NMGetByte(iMsg);
	
	NCBalls.ClientDel = iBallIndex;
}

void NetClient_OnBallHitHull(int iMsg, int iBallIndex)
{
	int wOurClientID = NMGetClientID(iMsg);
	int wBallClientID = NMGetClientID(iMsg);

	int iBallType = NMGetByte(iMsg);
	ref rGood = Net_GetGoodByIndex(iBallType);

	float x = NMGetFloat(iMsg); 
	float y = NMGetFloat(iMsg);	
	float z = NMGetFloat(iMsg);

	switch (rGood.Name)
	{
		case "Balls":
			Play3DSound("ball2bort", x, y, z);
			CreateParticleSystem("blast", x, y, z, 0.0, 0.0, 0.0, 0);
			break;
		case "Grapes":
			Play3DSound("grapes2bort", x, y, z);
			CreateParticleSystem("blast", x, y, z, 0.0, 0.0, 0.0, 0);
			break;
		case "Knippels":
			Play3DSound("knippel2bort", x, y, z);
			CreateParticleSystem("blast", x, y, z, 0.0, 0.0, 0.0, 0);
			break;
		case "Bombs":
			Play3DSound("bomb2bort", x, y, z);
			CreateParticleSystem("blast", x, y, z, 0.0, 0.0, 0.0, 0);
			break;
	}

	bool bSeriousBoom = NMGetByte(iMsg);
	bool bFirePlaceActivate = NMGetByte(iMsg);

	if (bFirePlaceActivate)
	{
		int iFirePlaceIndex = NMGetWord(iMsg);
		int iStartFireTime = NMGetDword(iMsg);
		float fTotalFireTime = NMGetFloat(iMsg);

		PostEvent("NetClient_ShipActivateFirePlace", iStartFireTime - iClientTime, "llf", wOurClientID, iFirePlaceIndex, fTotalFireTime);
		if (wBallClientID == NET_FORTCLIENT)
		{
			NetClient_ChatAddGameMessage("Net_FireActivation", rgbGameMessage, 2, wBallClientID, wOurClientID);
		}
	}

	// if critical hit
	if (bSeriousBoom)
	{
		// fort can do it
		NetClient_ChatAddGameMessage("Net_CriticalHit", rgbGameMessage, 2, wBallClientID, wOurClientID);
	}

	if (bSeriousBoom)
	{
		NetClient_ShipSeriousBoom(x, y, z);
	}

	NCBalls.ClientDel = iBallIndex;
}

