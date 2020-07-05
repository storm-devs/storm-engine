object NSBalls;

int iServerBallIndex;
int iServerCurrentBallIndex;

void NetServer_DeleteBallsEnvironment()
{
	DelEventHandler("NetServer_OnCannonFire", "NetServer_OnCannonFire");
	DelEventHandler("NetServer_OnBallWaterHit", "NetServer_OnBallWaterHit");

	DeleteClass(&NSBalls);
}

float NetServer_GetSpeedModifier()
{
	return (stf(NetServer.GameSpeed) + 1.0) / 2.0;
}

void NetServer_CreateBallsEnvironment(int iSMsg)
{
	aref	Grapes, Knippels, Balls, Bombs;

	NSBalls.Server = true;
	CreateEntity(&NSBalls, "NetBalls");

	NSBalls.SpeedMultiply = 2.0 * NetServer_GetSpeedModifier();
	NSBalls.Texture = "AllBalls.tga";
	NSBalls.SubTexX = 2;
	NSBalls.SubTexY = 2;

	NSBalls.CurrentBallDistance = 0.0;

	makearef(Grapes, NSBalls.Balls.Grapes);
	makearef(Knippels, NSBalls.Balls.Knippels);
	makearef(Balls, NSBalls.Balls.Balls);
	makearef(Bombs, NSBalls.Balls.Bombs);

	Bombs.SubTexIndex = 0;		Bombs.Size = 0.3;		Bombs.GoodIndex = Net_GetGoodIndex("Bombs");		Bombs.Particle = "bomb_smoke";
	Grapes.SubTexIndex = 1;		Grapes.Size = 0.2;		Grapes.GoodIndex = Net_GetGoodIndex("Grapes");
	Balls.SubTexIndex = 2;		Balls.Size = 0.2;		Balls.GoodIndex = Net_GetGoodIndex("Balls");
	Knippels.SubTexIndex = 3;	Knippels.Size = 0.2;	Knippels.GoodIndex = Net_GetGoodIndex("Knippels");

	NSBalls.IsDone = true;

	LayerAddObject("net_execute", &NSBalls, -1);

	NMAddFloat(iSMsg, stf(NSBalls.SpeedMultiply));

	SetEventHandler("NetServer_OnCannonFire", "NetServer_OnCannonFire", 0);
	SetEventHandler("NetServer_OnBallWaterHit", "NetServer_OnBallWaterHit", 0);
}

void NetServer_OnCannonFire()
{
	aref arClient = GetEventData();
	//int wFireClientID = GetEventData();
	float x = GetEventData();
	float y = GetEventData();
	float z = GetEventData();
	float fSpeedV0 = GetEventData();
	float fDirAng = GetEventData();
	float fHeightAng = GetEventData();
	float fCannonDirAng = GetEventData();

	ref rBall = Net_GetGoodByIndex(sti(arClient.Ship.Cannons.Charge.Type));
	int iBallType = sti(rBall.Index);

	EntityUpdate(0);

	NSBalls.x = x;
	NSBalls.y = y;
	NSBalls.z = z;
	NSBalls.iBallIndex = iServerBallIndex;
	NSBalls.ClientID = arClient.index;
	NSBalls.Type = rBall.Name;

	float fAccuracy = 1.2 - stf(arClient.Skills.Accuracy) / NETSKILL_MAX;
	float fK = Bring2Range(0.4, 1.2, 0.2, 1.2, fAccuracy);

	float fTempDispersionY = Degree2Radian(20.0);
	float fTempDispersionX = Degree2Radian(10.0);

	fDirAng = fDirAng + fK * fTempDispersionY * (frnd() - 0.5);
	fSpeedV0 = fSpeedV0;// + fAccuracy * (10.0 * fTempDispersionY) * (frnd() - 0.5);
	fHeightAng = fHeightAng + fAccuracy * (fTempDispersionX) * (frnd() - 0.5);

	NSBalls.Dir = fDirAng;
	NSBalls.SpdV0 = fSpeedV0;
	NSBalls.Ang = fHeightAng;

	EntityUpdate(1);
	NSBalls.ServerAdd = "";

	int iSMsg = NMCreate();
	NMAddByte(iSMsg, NC_BALL);
	NMAddByte(iSMsg, NSC_BALL_ADD);
	NMAddWord(iSMsg, iServerBallIndex);
	NMAddDword(iSMsg, iServerTime);
	NMAddByte(iSMsg, sti(rBall.Index));
	NMAddFloat(iSMsg, x);
	NMAddFloat(iSMsg, y);
	NMAddFloat(iSMsg, z);
	NMAddFloat(iSMsg, fSpeedV0);
	NMAddFloat(iSMsg, fDirAng);
	NMAddFloat(iSMsg, fHeightAng);
	NetServer_SendMessage(DST_ALL, iSMsg, false);
	NMDelete(iSMsg);

	iServerBallIndex++;
}

void NetServer_OnBallWaterHit()
{
	int wFireClientID = GetEventData();
	float x = GetEventData();
	float y = GetEventData();
	float z = GetEventData();

	ref rClient = NetServer_GetClient(wFireClientID);
	rClient.Stat.Misses = sti(rClient.Stat.Misses) + 1;

	int iSMsg = NMCreate();
	NMAddByte(iSMsg, NC_BALL);
	NMAddByte(iSMsg, NSC_BALL_HIT_WATER);
	NMAddWord(iSMsg, iServerCurrentBallIndex);
	NMAddFloat(iSMsg, x);
	NMAddFloat(iSMsg, y);
	NMAddFloat(iSMsg, z);
	NetServer_SendMessage(DST_ALL, iSMsg, false);
	NMDelete(iSMsg);
}
