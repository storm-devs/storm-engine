object	AIBalls;
aref	Grapes, Knippels, Balls, Bombs;

// ------------------- Current Ball Info -------------------
// AIBalls.CurrentBallType = GOOD_BALLS, GOOD_KNIPPELS, ... etc
// AIBalls.CurrentBallCannonType = CANNON_TYPE_CULVERINE_LBS8, CANNON_TYPE_CULVERINE_LBS12, ... etc
// AIBalls.CurrentBallDistance = distance from start
// AIBalls.CurrentMaxBallDistance = max distance for balls

void DeleteBallsEnvironment()
{
	DeleteClass(&AIBalls);

	DelEventHandler(BALL_WATER_HIT, "Ball_WaterHitEvent");
	DelEventHandler(BALL_ISLAND_HIT, "Ball_IslandHit");
	DelEventHandler(BALL_FLY_UPDATE, "Ball_OnFlyUpdate");
	DelEventHandler(BALL_FORT_HIT, "Ball_FortHit");
	DelEventHandler(BALL_FLY_NEAR_CAMERA, "Ball_FlyNearCamera");
}

void CreateBallsEnvironment()
{
	CreateEntity(&AIBalls, "AIBalls");
	LayerAddObject(SEA_EXECUTE, &AIBalls, -1);
	LayerAddObject(SEA_REALIZE, &AIBalls, 65532);

	AIBalls.CurrentBallCannonType = -1;
	AIBalls.CurrentBallDistance = 0.0;
	AIBalls.CurrentMaxBallDistance = 0.0;
	AIBalls.BallFlySoundDistance = 15.0;
	AIBalls.BallFlySoundStereoMultiplyer = 2.0;

	AIBalls.SpeedMultiply = 3.0;
	AIBalls.Texture = "AllBalls.tga";
	AIBalls.SubTexX = 2;
	AIBalls.SubTexY = 2;

	makearef(Grapes,AIBalls.Balls.Grapes);
	makearef(Knippels,AIBalls.Balls.Knippels);
	makearef(Balls,AIBalls.Balls.Balls);
	makearef(Bombs,AIBalls.Balls.Bombs);

	// Bombs
	Bombs.SubTexIndex = 0;		Bombs.Size = 0.3;		Bombs.GoodIndex = GOOD_BOMBS;
	Bombs.Particle = "bomb_smoke";

	// Grapes
	Grapes.SubTexIndex = 1;		Grapes.Size = 0.2;		Grapes.GoodIndex = GOOD_GRAPES;

	// Balls
	Balls.SubTexIndex = 2;		Balls.Size = 0.2;		Balls.GoodIndex = GOOD_BALLS;

	// Knippels
	Knippels.SubTexIndex = 3;	Knippels.Size = 0.2;	Knippels.GoodIndex = GOOD_KNIPPELS;

	AIBalls.isDone = 1;

	// cheat - fire from camera
	AIBalls.FireBallFromCamera = true;

	SetEventHandler(BALL_WATER_HIT, "Ball_WaterHitEvent", 0);
	SetEventHandler(BALL_ISLAND_HIT, "Ball_IslandHit", 0);
	SetEventHandler(BALL_FLY_UPDATE, "Ball_OnFlyUpdate", 0);
	SetEventHandler(BALL_FORT_HIT, "Ball_FortHit", 0);
	SetEventHandler(BALL_FLY_NEAR_CAMERA, "Ball_FlyNearCamera", 0);
}

void Ball_FlyNearCamera()
{
	float x = GetEventData();
	float y = GetEventData();
	float z = GetEventData();

	Play3DSound("fly_ball", x, y, z);
}

int ballNumber;

void Ball_AddBall(aref aCharacter, float fX, float fY, float fZ, float fSpeedV0, float fDirAng, float fHeightAng, float fCannonDirAng, float fMaxFireDistance)
{
	int iCannonType = sti(aCharacter.Ship.Cannons.Type);
	ref rCannon = GetCannonByType(iCannonType);
	float fCannonHeightMultiply = stf(rCannon.HeightMultiply);

	EntityUpdate(0);
	AIBalls.CannonType = iCannonType;
	AIBalls.x = fX;
	AIBalls.y = fY;
	AIBalls.z = fZ;
	AIBalls.CharacterIndex    = aCharacter.Index;
	AIBalls.Type = Goods[sti(aCharacter.Ship.Cannons.Charge.Type)].Name;
	AIBalls.HeightMultiply    = fCannonHeightMultiply;
	AIBalls.SizeMultiply      = rCannon.SizeMultiply;
	AIBalls.TimeSpeedMultiply = rCannon.TimeSpeedMultiply;
	AIBalls.MaxFireDistance   = fMaxFireDistance;
	
	float fTempDispersionY = Degree2Radian(15.0);
	float fTempDispersionX = Degree2Radian(5.0);

	//float fDamage2Cannons = 100.0;

    float fAccuracy = 1.2 - stf(aCharacter.TmpSkill.Accuracy);

	float fCannons = stf(aCharacter.TmpSkill.Cannons)*10;

	fCannons = 15.0 + MOD_SKILL_ENEMY_RATE - fCannons;

	if (fCannons > 0.0 && RealShips[sti(aCharacter.ship.type)].BaseName != "fort") // fix
	{
		if (fCannons > rand(100))
		{
            fCannons = (rand(4) + 2.0*(1.65 - stf(aCharacter.TmpSkill.Cannons))) * 10;
			SendMessage(&AISea, "laffff", AI_MESSAGE_CANNONS_BOOM_CHECK, aCharacter, fCannons, fx, fy, fz);  // fDamage2Cannons  там много делителей, потому много
		}
	}

	float fK = Bring2Range(0.5, 1.2, 0.2, 1.2, fAccuracy);
	
	AIBalls.Dir = fDirAng + fK * fTempDispersionY * (frnd() - 0.5);
	AIBalls.SpdV0 = fSpeedV0 + fAccuracy * (10.0 * fTempDispersionY) * (frnd() - 0.5);
	AIBalls.Ang = fHeightAng + fAccuracy * (fTempDispersionX) * (frnd() - 0.5);

	AIBalls.Event = "";

	EntityUpdate(1);
	AIBalls.Add = "";

	string sParticleName = "cancloud_fire";		// if (sti(aCharacter.ship.type) < SHIP_CORVETTE)

	if (iCannonType == CANNON_TYPE_CANNON_LBS48)
		{ sParticleName = "Bombard"; }
	else
	{
		if (sti(aCharacter.ship.type) >= SHIP_CORVETTE)	{ sParticleName = "cancloud_fire_big"; }
	}
	//if (rand(1) == 0) // boal оптимизация дыма
	CreateParticleSystem(sParticleName, fX, fY, fZ, -fHeightAng - (fCannonHeightMultiply - 1.0) * 0.1, fDirAng, 0.0, 5);
	Play3DSound(rCannon.Sound, fX, fY, fZ);
}

void Ball_WaterHitEvent()
{
	int		iCharacterIndex;
	float	x, y, z, vx, vy, vz;

	iCharacterIndex = GetEventData();
	x = GetEventData();
	y = GetEventData();
	z = GetEventData();

	if (sti(AIBalls.CurrentBallCannonType) >= 0)
	{
		ref rCannon = GetCannonByType(sti(AIBalls.CurrentBallCannonType));

		if (sti(rCannon.BigBall))	{ CreateParticleSystem("splash_big", X, Y, Z, 0.0, 0.0, 0.0, 5); }
		else						{ CreateParticleSystem("splash", X, Y, Z, 0.0, 0.0, 0.0, 5); }
	}
	else
	{
		CreateParticleSystem("splash", X, Y, Z, 0.0, 0.0, 0.0, 5);
	}

	Play3DSound("ball_splash", x, y, z);
}

void Ball_FortHit()
{
	int		iCharacterIndex;
	float	x, y, z;

	iCharacterIndex = GetEventData();

	x = GetEventData();
	y = GetEventData();
	z = GetEventData();

	if (rand(4) == 1) CreateParticleSystem("blast", x, y, z, 0.0, 0.0, 0.0, 0); // boal fix
	SendMessage(&AIFort, "llfff", AI_MESSAGE_FORT_HIT, iCharacterIndex, x, y, z);
}

void Ball_IslandHit()
{
	int		iCharacterIndex;
	float	x, y, z;

	iCharacterIndex = GetEventData();

	x = GetEventData();
	y = GetEventData();
	z = GetEventData();

	if (rand(2) == 1) CreateParticleSystem("blast", x, y, z, 0.0, 0.0, 0.0, 0); // boal fix

	//Ship_SetLightsOff(&Characters[1], 15.0, true, true, false);
}

void Ball_OnFlyUpdate()
{
	int charIndex = GetEventData();
	int ballAlive = GetEventData();
	float x = GetEventData();
	float y = GetEventData();
	float z = GetEventData();
	float lx = GetEventData();
	float ly = GetEventData();
	float lz = GetEventData();
	SendMessage(&SeaOperator, "lalffffff", MSG_SEA_OPERATOR_BALL_UPDATE, &Characters[charIndex], ballAlive, x, y, z, lx, ly, lz);
}