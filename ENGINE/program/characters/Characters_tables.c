void InitCharactersTables()
{
	InitMoraleTable();
}

void InitMoraleTable()
{
	MoraleTable[MORALET_TREACHEROUS]	= "Treacherous";
	MoraleTable[MORALET_AWFUL]			= "Awful";
	MoraleTable[MORALET_LOW]			= "Low";
	MoraleTable[MORALET_POOR]			= "Poor";
	MoraleTable[MORALET_NORMAL]			= "Normal";
	MoraleTable[MORALET_ELATED]			= "Elated";
	MoraleTable[MORALET_GOOD]			= "Good";
	MoraleTable[MORALET_HIGH]			= "High";
	MoraleTable[MORALET_EXCELLENT]		= "Excellent";
	MoraleTable[MORALET_HEROIC]			= "Heroic";

	LoyalityTable[MORALET_TREACHEROUS]	= "LoyalityMinimum";
	LoyalityTable[MORALET_AWFUL]		= "LoyalityAwful";
	LoyalityTable[MORALET_LOW]			= "LoyalityLow";
	LoyalityTable[MORALET_POOR]			= "LoyalityPoor";
	LoyalityTable[MORALET_NORMAL]		= "LoyalityNormal";
	LoyalityTable[MORALET_ELATED]		= "LoyalityElated";
	LoyalityTable[MORALET_GOOD]			= "LoyalityGood";
	LoyalityTable[MORALET_HIGH]			= "LoyalityHigh";
	LoyalityTable[MORALET_EXCELLENT]	= "LoyalityExcellent";
	LoyalityTable[MORALET_HEROIC]		= "LoyalityMaximum";
}
// boal это явно старый код - оставляю, тк не мое, но можено потереть - нет таких лиц
/*void InitFaceDescribeTable()
{
	int i;
	for(i=0; i<FACETABLE_SIZE; i++)
	{
		FaceDescribeTable[i].FaceIconName = "face"+(i+1);
		FaceDescribeTable[i].FaceIconGroup = "FACEICON";
		FaceDescribeTable[i].FacePortrateFileName = "interfaces\portraits\face_"+(i+1)+".tga";
	}
}*/
