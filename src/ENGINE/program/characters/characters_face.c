string	sEnManFaces = "30,35,10,6,32,33,42,43";
string	sFrManFaces = "19,27,29,31,32,33,43,43";
string	sSpManFaces = "25,37,38,44,7,32,33,42,43";
string	sHoManFaces = "11,26,28,36,32,33,42,43";
string	sPoManFaces = "12,30,39,32,33,42,43";
string	sPiManFaces = "0,1,13,14,2,23,24,3,40,5";

void SetRandomFaceToCharacter(ref rCharacter)
{
	string sFaceID = "1";
	if (rCharacter.sex == "man")
	{
		switch (sti(rCharacter.nation))
		{
			case ENGLAND:	
				sFaceID = GetRandSubString(sEnManFaces); 
			break;
			case FRANCE:	
				sFaceID = GetRandSubString(sFrManFaces); 
			break;
			case SPAIN:		
				sFaceID = GetRandSubString(sSpManFaces); 
				break;
			case PIRATE:	
				sFaceID = GetRandSubString(sPiManFaces); 
				break;
			case HOLLAND:	
				sFaceID = GetRandSubString(sHoManFaces); 
				break;
		}
	}
	else
	{
		sFaceID = "16";
	}
	
	rCharacter.FaceId = sti(sFaceID);
}

void FaceMaker(aref rCharacter)
{
	switch (rCharacter.model)
	{

		case "beatrice":		rCharacter.FaceId = 0; break;
		case "beatriceA":		rCharacter.FaceId = 0; break;
		case "devlin":			rCharacter.FaceId = 1; break;

		case "barmen_1":		rCharacter.FaceId = 2; break;
		case "barmen_2":		rCharacter.FaceId = 3; break;
		case "barmen_3":		rCharacter.FaceId = 4; break;
		case "barmen_4":		rCharacter.FaceId = 5; break;
		case "barmen_5":		rCharacter.FaceId = 6; break;
		case "barmen_6":		rCharacter.FaceId = 7; break;
		case "barmen_7":		rCharacter.FaceId = 8; break;
		case "barmen_8":		rCharacter.FaceId = 9; break;
		case "barmen_9":		rCharacter.FaceId = 10; break;
		case "barmen_10":		rCharacter.FaceId = 11; break;
		case "barmen_11":		rCharacter.FaceId = 12; break;
		case "barmen_12":		rCharacter.FaceId = 13; break;
		case "barmen_13":		rCharacter.FaceId = 14; break;
		case "barmen_14":		rCharacter.FaceId = 15; break;
		case "barmen_15":		rCharacter.FaceId = 16; break;
		case "barmen_16":		rCharacter.FaceId = 17; break;
		case "officer_1":		rCharacter.FaceId = 18; break;
		case "officer_2":		rCharacter.FaceId = 19; break;
		case "officer_3":		rCharacter.FaceId = 20; break;
		case "officer_4":		rCharacter.FaceId = 21; break;
		case "officer_5":		rCharacter.FaceId = 22; break;
		case "officer_6":		rCharacter.FaceId = 23; break;
		case "officer_7":		rCharacter.FaceId = 24; break;
		case "officer_8":		rCharacter.FaceId = 25; break;
		case "officer_9":		rCharacter.FaceId = 26; break;
		case "officer_10":		rCharacter.FaceId = 27; break;
		case "officer_11":		rCharacter.FaceId = 28; break;
		case "officer_12":		rCharacter.FaceId = 29; break;
		case "officer_13":		rCharacter.FaceId = 30; break;
		case "officer_14":		rCharacter.FaceId = 31; break;
		case "officer_15":		rCharacter.FaceId = 32; break;
		case "officer_16":		rCharacter.FaceId = 33; break;
		case "officer_17":		rCharacter.FaceId = 34; break;
		case "officer_18":		rCharacter.FaceId = 35; break;
		case "officer_19":		rCharacter.FaceId = 36; break;
		case "officer_20":		rCharacter.FaceId = 37; break;
		case "citiz_1":			rCharacter.FaceId = 38; break;
		case "citiz_2":			rCharacter.FaceId = 39; break;
		case "citiz_3":			rCharacter.FaceId = 40; break;
		case "citiz_4":			rCharacter.FaceId = 41; break;
		case "citiz_5":			rCharacter.FaceId = 42; break;
		case "citiz_6":			rCharacter.FaceId = 43; break;
		case "citiz_7":			rCharacter.FaceId = 44; break;
		case "citiz_8":			rCharacter.FaceId = 45; break;
		case "citiz_9":			rCharacter.FaceId = 46; break;
		case "citiz_10":		rCharacter.FaceId = 47; break;
		case "citiz_11":		rCharacter.FaceId = 48; break;
		case "citiz_12":		rCharacter.FaceId = 49; break;
		case "girl_1":			rCharacter.FaceId = 50; break;
		case "girl_2":			rCharacter.FaceId = 51; break;
		case "girl_3":			rCharacter.FaceId = 52; break;
		case "girl_4":			rCharacter.FaceId = 53; break;
		case "girl_5":			rCharacter.FaceId = 54; break;
		case "girl_6":			rCharacter.FaceId = 55; break;
		case "girl_7":			rCharacter.FaceId = 56; break;
	    case "girl_8":			rCharacter.FaceId = 57; break;
		case "head_pir":		rCharacter.FaceId = 58; break;
		case "huber_eng":		rCharacter.FaceId = 59; break;
		case "huber_fra":		rCharacter.FaceId = 60; break;
		case "huber_spa":		rCharacter.FaceId = 61; break;
		case "huber_hol":		rCharacter.FaceId = 62; break;
		case "lady_1":			rCharacter.FaceId = 63; break;
		case "lady_2":			rCharacter.FaceId = 64; break;
		case "lady_3":			rCharacter.FaceId = 65; break;
		case "lady_4":			rCharacter.FaceId = 66; break;
		case "lady_5":			rCharacter.FaceId = 67; break;
		case "lady_6":			rCharacter.FaceId = 68; break;
		case "off_eng_1":		rCharacter.FaceId = 69; break;
		case "off_eng_2":		rCharacter.FaceId = 70; break;
		case "off_fra_1":		rCharacter.FaceId = 71; break;
		case "off_fra_2":		rCharacter.FaceId = 72; break;
		case "off_spa_1":		rCharacter.FaceId = 73; break;
		case "off_spa_2":		rCharacter.FaceId = 74; break;
		case "off_hol_1":		rCharacter.FaceId = 75; break;
		case "off_hol_2":		rCharacter.FaceId = 76; break;
		case "pirate_1":		rCharacter.FaceId = 77; break;
		case "pirate_2":		rCharacter.FaceId = 78; break;
		case "pirate_3":		rCharacter.FaceId = 79; break;
		case "pirate_4":		rCharacter.FaceId = 80; break;
		case "pirate_5":		rCharacter.FaceId = 81; break;
		case "pirate_6":		rCharacter.FaceId = 82; break;
		case "pirate_7":		rCharacter.FaceId = 83; break;
		case "pirate_8":		rCharacter.FaceId = 84; break;
		case "pirate_9":		rCharacter.FaceId = 85; break;
		case "pirate_10":		rCharacter.FaceId = 86; break;
		case "priest_1":		rCharacter.FaceId = 87; break;
		case "priest_2":		rCharacter.FaceId = 88; break;
		case "priest_3":		rCharacter.FaceId = 89; break;
		case "priest_4":		rCharacter.FaceId = 90; break;
		case "shipowner_1":		rCharacter.FaceId = 91; break;
		case "shipowner_2":		rCharacter.FaceId = 92; break;
		case "shipowner_3":		rCharacter.FaceId = 93; break;
		case "shipowner_4":		rCharacter.FaceId = 94; break;
		case "shipowner_5":		rCharacter.FaceId = 95; break;
		case "shipowner_6":		rCharacter.FaceId = 96; break;
		case "shipowner_7":		rCharacter.FaceId = 97; break;
		case "shipowner_8":		rCharacter.FaceId = 98; break;
		case "shipowner_9":		rCharacter.FaceId = 99; break;
		case "shipowner_10":	rCharacter.FaceId = 100; break;
		case "shipowner_11":	rCharacter.FaceId = 101; break;
		case "shipowner_12":	rCharacter.FaceId = 102; break;
		case "shipowner_13":	rCharacter.FaceId = 103; break;
		case "shipowner_14":	rCharacter.FaceId = 104; break;
		case "shipowner_15":	rCharacter.FaceId = 105; break;
		case "shipowner_16":	rCharacter.FaceId = 106; break;
		case "smuggler_boss":	rCharacter.FaceId = 107; break;
		case "sold_hol_1":		rCharacter.FaceId = 108; break;
		case "sold_hol_2":		rCharacter.FaceId = 109; break;
		case "sold_hol_3":		rCharacter.FaceId = 110; break;
		case "sold_hol_4":		rCharacter.FaceId = 111; break;
		case "sold_hol_5":		rCharacter.FaceId = 112; break;
		case "sold_hol_6":		rCharacter.FaceId = 113; break;
		case "sold_hol_7":		rCharacter.FaceId = 114; break;
		case "sold_hol_8":		rCharacter.FaceId = 115; break;
		case "sold_eng_1":		rCharacter.FaceId = 116; break;
		case "sold_eng_2":		rCharacter.FaceId = 117; break;
		case "sold_eng_3":		rCharacter.FaceId = 118; break;
		case "sold_eng_4":		rCharacter.FaceId = 119; break;
		case "sold_eng_5":		rCharacter.FaceId = 120; break;
		case "sold_eng_6":		rCharacter.FaceId = 121; break;
		case "sold_eng_7":		rCharacter.FaceId = 122; break;
		case "sold_eng_8":		rCharacter.FaceId = 123; break;
		case "sold_fra_1":		rCharacter.FaceId = 124; break;
		case "sold_fra_2":		rCharacter.FaceId = 125; break;
		case "sold_fra_3":		rCharacter.FaceId = 126; break;
		case "sold_fra_4":		rCharacter.FaceId = 127; break;
		case "sold_fra_5":		rCharacter.FaceId = 128; break;
		case "sold_fra_6":		rCharacter.FaceId = 129; break;
		case "sold_fra_7":		rCharacter.FaceId = 130; break;
		case "sold_fra_8":		rCharacter.FaceId = 131; break;
		case "sold_spa_1":		rCharacter.FaceId = 131; break;
		case "sold_spa_2":		rCharacter.FaceId = 133; break;
		case "sold_spa_3":		rCharacter.FaceId = 134; break;
		case "sold_spa_4":		rCharacter.FaceId = 135; break;
		case "sold_spa_5":		rCharacter.FaceId = 136; break;
		case "sold_spa_6":		rCharacter.FaceId = 137; break;
		case "sold_spa_7":		rCharacter.FaceId = 138; break;
		case "sold_spa_8":		rCharacter.FaceId = 139; break;
		case "trader_1":		rCharacter.FaceId = 140; break;
		case "trader_2":		rCharacter.FaceId = 141; break;
		case "trader_3":		rCharacter.FaceId = 142; break;
		case "trader_4":		rCharacter.FaceId = 143; break;
		case "trader_5":		rCharacter.FaceId = 144; break;
		case "trader_6":		rCharacter.FaceId = 145; break;
		case "trader_7":		rCharacter.FaceId = 146; break;
		case "trader_8":		rCharacter.FaceId = 147; break;
		case "trader_9":		rCharacter.FaceId = 148; break;
		case "trader_10":		rCharacter.FaceId = 149; break;
		case "trader_11":		rCharacter.FaceId = 150; break;
		case "trader_12":		rCharacter.FaceId = 151; break;
		case "trader_13":		rCharacter.FaceId = 152; break;
		case "trader_14":		rCharacter.FaceId = 153; break;
		case "trader_15":		rCharacter.FaceId = 154; break;
		case "trader_16":		rCharacter.FaceId = 155; break;
		case "usurer_1":		rCharacter.FaceId = 156; break;
		case "usurer_2": 		rCharacter.FaceId = 157; break;
		case "usurer_3":		rCharacter.FaceId = 158; break;
		case "usurer_4":		rCharacter.FaceId = 159; break;
		case "usurer_5":		rCharacter.FaceId = 160; break;
		case "usurer_6":		rCharacter.FaceId = 161; break;
		case "usurer_7":		rCharacter.FaceId = 162; break;
		case "usurer_8":		rCharacter.FaceId = 163; break;
		case "whore_1":			rCharacter.FaceId = 164; break;
		case "whore_2":			rCharacter.FaceId = 165; break;
		case "whore_3":			rCharacter.FaceId = 166; break;
		case "whore_4":			rCharacter.FaceId = 167; break;
		case "mushketer_1":		rCharacter.FaceId = 80; break;
		case "mushketer_2":		rCharacter.FaceId = 77; break;
		case "mushketer_3":		rCharacter.FaceId = 81; break;
		case "mushketer_4":		rCharacter.FaceId = 79; break;
		case "mushketer_5":		rCharacter.FaceId = 78; break;
		case "eng_mush_1":		rCharacter.FaceId = 116; break;
		case "eng_mush_2":		rCharacter.FaceId = 118; break;
		case "eng_mush_3":		rCharacter.FaceId = 123; break;
		case "hol_mush_1":		rCharacter.FaceId = 108; break;
		case "hol_mush_2":		rCharacter.FaceId = 109; break;
		case "hol_mush_3":		rCharacter.FaceId = 115; break;
		case "fra_mush_1":		rCharacter.FaceId = 124; break;
		case "fra_mush_2":		rCharacter.FaceId = 128; break;
		case "fra_mush_3":		rCharacter.FaceId = 130; break;
		case "spa_mush_1":		rCharacter.FaceId = 131; break;
		case "spa_mush_2":		rCharacter.FaceId = 133; break;
		case "spa_mush_3":		rCharacter.FaceId = 139; break;
        // к ПКМ моделям
		case "Corsair1_2":      rCharacter.FaceId = 198; break;
		case "AnnaDeLeiva":		rCharacter.FaceId = 199; break;
		case "Albermal":		rCharacter.FaceId = 200; break;
		case "Corsair1_3":		rCharacter.FaceId = 201; break;
		case "capitan":		    rCharacter.FaceId = 202; break;
		case "pirat00":		    rCharacter.FaceId = 203; break;
		case "MorrisWillams":   rCharacter.FaceId = 204; break;
		case "ThomasLinch":     rCharacter.FaceId = 205; break;
		case "Korsar1":         rCharacter.FaceId = 206; break;
		case "mummy":           rCharacter.FaceId = 207; break;
		case "liza":            rCharacter.FaceId = 208; break;
		case "bocman":          rCharacter.FaceId = 209; break;
		// новые модельки
		case "BigPirate":		rCharacter.FaceId = 210; break;
		case "chard":			rCharacter.FaceId = 211; break;
		case "chard2":			rCharacter.FaceId = 212; break;
		case "chard3":			rCharacter.FaceId = 213; break;
		case "Espinosa":		rCharacter.FaceId = 214; break;
		case "Hero1":			rCharacter.FaceId = 215; break;
		case "Isterling":		rCharacter.FaceId = 216; break;
		case "Morgan":			rCharacter.FaceId = 217; break;
		case "SpaOfficer1":		rCharacter.FaceId = 218; break;
		case "SpaOfficer2":		rCharacter.FaceId = 219; break;
		case "Youngman":		rCharacter.FaceId = 220; break;
		case "Blood5":			rCharacter.FaceId = 221; break;
		case "blood_bomj":		rCharacter.FaceId = 221; break;
	}
}

//Создадим вражеских фантомов
string GetRandSkelModel()
{
	string emodel = "mummy";

	switch (rand(3))
    {
        case 0: emodel = "Skel1";   break;
        case 1: emodel = "Skel2";   break;
        case 2: emodel = "Skel3";   break;
        case 3: emodel = "Skel4";   break;
    }
    return emodel;
}

// boal 22.04.04 выбор модели зависимо от типа
void SetCaptanModelByEncType(ref Chref, string sFantomType)
{
    string ModelPirate = "Albermal"; // значит баг

	if (sti(Chref.nation) == PIRATE) sFantomType = "pirate"; // иначе баг
	
    switch (sFantomType)
	{
		case "trade":
            ModelPirate = "trader_" + (rand(15) + 1);
		break;
		case "war":
			ModelPirate = "off_" + NationShortName(sti(Chref.nation)) + "_" + (rand(1) + 1);
		break;
		case "pirate":
            ModelPirate = "officer_" + (rand(19) + 1);
		break;
	}
	Chref.model = ModelPirate;
	FaceMaker(Chref);
}

void SetModelPirate(ref ChrefPirate)
{
    string ModelPirate;
    
    ModelPirate = "Pirate_" + (rand(9) + 1);
    
    ChrefPirate.model = ModelPirate;
    FaceMaker(ChrefPirate);
}
