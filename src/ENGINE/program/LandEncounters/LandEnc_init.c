int InitLandEncTemplates()
{
	int n;

	n = 0;

	LandEncTemplate[n].id = "port";
	LandEncTemplate[n].quantity = 0;

	LandEncTemplate[n].enc1.type = "ENC_BANDITS";
	LandEncTemplate[n].enc1.chance = 0.2;
	LandEncTemplate[n].enc1.StartTime = 22;
	LandEncTemplate[n].enc1.EndTime = 6;

	LandEncTemplate[n].enc2.type = "ENC_POORMAN";
	LandEncTemplate[n].enc2.chance = 0.1;
	LandEncTemplate[n].enc2.StartTime = 0;
	LandEncTemplate[n].enc2.EndTime = 24;

	LandEncTemplate[n].enc3.type = "ENC_LATEWALKER";
	LandEncTemplate[n].enc3.chance = 0.2;
	LandEncTemplate[n].enc3.StartTime = 22;
	LandEncTemplate[n].enc3.EndTime = 6;

	LandEncTemplate[n].enc4.type = "ENC_PATROL";
	LandEncTemplate[n].enc4.chance = 0.1;
	LandEncTemplate[n].enc4.StartTime = 22;
	LandEncTemplate[n].enc4.EndTime = 6;

	LandEncTemplate[n].enc5.type = "ENC_DEADMAN";
	LandEncTemplate[n].enc5.chance = 0.1;
	LandEncTemplate[n].enc5.StartTime = 22;
	LandEncTemplate[n].enc5.EndTime = 6;

	n = n + 1;

//---------------------------------------------------------------------------------------------

	LandEncTemplate[n].id = "town";
	LandEncTemplate[n].quantity = 0;


	LandEncTemplate[n].enc1.type = "ENC_BANDITS";
	LandEncTemplate[n].enc1.chance = 0.2;
	LandEncTemplate[n].enc1.StartTime = 22;
	LandEncTemplate[n].enc1.EndTime = 6;

	LandEncTemplate[n].enc2.type = "ENC_POORMAN";
	LandEncTemplate[n].enc2.chance = 0.1;
	LandEncTemplate[n].enc2.StartTime = 0;
	LandEncTemplate[n].enc2.EndTime = 24;

	LandEncTemplate[n].enc3.type = "ENC_LATEWALKER";
	LandEncTemplate[n].enc3.chance = 0.2;
	LandEncTemplate[n].enc3.StartTime = 22;
	LandEncTemplate[n].enc3.EndTime = 6;

	LandEncTemplate[n].enc4.type = "ENC_PATROL";
	LandEncTemplate[n].enc4.chance = 0.1;
	LandEncTemplate[n].enc4.StartTime = 22;
	LandEncTemplate[n].enc4.EndTime = 6;

	LandEncTemplate[n].enc5.type = "ENC_DEADMAN";
	LandEncTemplate[n].enc5.chance = 0.1;
	LandEncTemplate[n].enc5.StartTime = 22;
	LandEncTemplate[n].enc5.EndTime = 6;

	LandEncTemplate[n].enc6.type = "ENC_RAPERS";
	LandEncTemplate[n].enc6.chance = 0.2;
	LandEncTemplate[n].enc6.StartTime = 22;
	LandEncTemplate[n].enc6.EndTime = 6;

	n = n + 1;

//---------------------------------------------------------------------------------------------

	LandEncTemplate[n].id = "jungle";
	LandEncTemplate[n].quantity = 1;


	LandEncTemplate[n].enc1.type = "ENC_RAIDERS";
	LandEncTemplate[n].enc1.chance = 0.2;
	LandEncTemplate[n].enc1.StartTime = 0;
	LandEncTemplate[n].enc1.EndTime = 24;

	LandEncTemplate[n].enc2.type = "ENC_RAPERS";
	LandEncTemplate[n].enc2.chance = 0.1;
	LandEncTemplate[n].enc2.StartTime = 0;
	LandEncTemplate[n].enc2.EndTime = 24;

	n = n + 1;

//---------------------------------------------------------------------------------------------

	LandEncTemplate[n].id = "seashore";
	LandEncTemplate[n].quantity = 0;

	LandEncTemplate[n].enc1.type = "ENC_SMUGGLERS";
	LandEncTemplate[n].enc1.chance = 0.2;
	LandEncTemplate[n].enc1.StartTime = 0;
	LandEncTemplate[n].enc1.EndTime = 24;

	LandEncTemplate[n].enc2.type = "ENC_CONVICTS";
	LandEncTemplate[n].enc2.chance = 0.2;
	LandEncTemplate[n].enc2.StartTime = 0;
	LandEncTemplate[n].enc2.EndTime = 24;

	LandEncTemplate[n].enc3.type = "ENC_SURVIVOR";
	LandEncTemplate[n].enc3.chance = 0.2;
	LandEncTemplate[n].enc3.StartTime = 22;
	LandEncTemplate[n].enc3.EndTime = 6;

	n = n + 1;


//---------------------------------------------------------------------------------------------

	LandEncTemplate[n].id = "tavern";
	LandEncTemplate[n].quantity = 1;

	LandEncTemplate[n].enc1.type = "ENC_OFFICER";
	LandEncTemplate[n].enc1.chance = 0.2;
	LandEncTemplate[n].enc1.StartTime = 0;
	LandEncTemplate[n].enc1.EndTime = 24;

	return n;

}


int InitLandEncounters()
{
	int n;

	n = 0;

	LandEncounters[n].id = "ENC_RAIDERS";

	LandEncounters[n].StartingQuest1 = "LandEnc_RaidersLogin";
	LandEncounters[n].StartingQuest1.QuestType = "Location";
	LandEncounters[n].StartingQuest2 = "LandEnc_RaidersStart";
	LandEncounters[n].StartingQuest2.QuestType  = "Locator";

	LAi_group_Register("ENC_RAIDERS_GROUP");

	LandEncounters[n].char1.modelQuantity = 14;
	LandEncounters[n].char1.model1 = "sailor1";
	LandEncounters[n].char1.model1.ani = "man";
	LandEncounters[n].char1.model2 = "sailor2";
	LandEncounters[n].char1.model2.ani = "man";
	LandEncounters[n].char1.model3 = "sailor3";
	LandEncounters[n].char1.model3.ani = "man";
	LandEncounters[n].char1.model4 = "sailor4";
	LandEncounters[n].char1.model4.ani = "man";
	LandEncounters[n].char1.model5 = "chelovek";
	LandEncounters[n].char1.model5.ani = "man";
	LandEncounters[n].char1.model6 = "chelovek1";
	LandEncounters[n].char1.model6.ani = "man";
	LandEncounters[n].char1.model7 = "fisherman";
	LandEncounters[n].char1.model7.ani = "man";
	LandEncounters[n].char1.model8 = "fisherman1";
	LandEncounters[n].char1.model8.ani = "man";
	LandEncounters[n].char1.model9 = "man5_1";
	LandEncounters[n].char1.model9.ani = "man";
	LandEncounters[n].char1.model9 = "bocman4";
	LandEncounters[n].char1.model9.ani = "man";
	LandEncounters[n].char1.model10 = "sailor5";
	LandEncounters[n].char1.model10.ani = "man";
	LandEncounters[n].char1.model11 = "Mask";
	LandEncounters[n].char1.model11.ani = "man";
	LandEncounters[n].char1.model12 = "Mask_2";
	LandEncounters[n].char1.model12.ani = "man";
	LandEncounters[n].char1.model13 = "Mask_3";
	LandEncounters[n].char1.model13.ani = "man";
	LandEncounters[n].char1.model14 = "Mask_4";
	LandEncounters[n].char1.model14.ani = "man";
	LandEncounters[n].char1.cont = true;
	LandEncounters[n].char1.pistol = true;
	LandEncounters[n].char1.sword = true;
	LandEncounters[n].char1.type = "warrior";
	LandEncounters[n].char1.Dialog = "Enc_Raiders_dialog.c";


	LandEncounters[n].char2.modelQuantity = 14;
	LandEncounters[n].char2.model1 = "sailor1";
	LandEncounters[n].char2.model1.ani = "man";
	LandEncounters[n].char2.model2 = "sailor2";
	LandEncounters[n].char2.model2.ani = "man";
	LandEncounters[n].char2.model3 = "sailor3";
	LandEncounters[n].char2.model3.ani = "man";
	LandEncounters[n].char2.model4 = "sailor4";
	LandEncounters[n].char2.model4.ani = "man";
	LandEncounters[n].char2.model5 = "chelovek";
	LandEncounters[n].char2.model5.ani = "man";
	LandEncounters[n].char2.model6 = "chelovek1";
	LandEncounters[n].char2.model6.ani = "man";
	LandEncounters[n].char2.model7 = "fisherman";
	LandEncounters[n].char2.model7.ani = "man";
	LandEncounters[n].char2.model8 = "fisherman1";
	LandEncounters[n].char2.model8.ani = "man";
	LandEncounters[n].char2.model9 = "man5_1";
	LandEncounters[n].char2.model9.ani = "man";
	LandEncounters[n].char2.model9 = "bocman4";
	LandEncounters[n].char2.model9.ani = "man";
	LandEncounters[n].char2.model10 = "sailor5";
	LandEncounters[n].char2.model10.ani = "man";
	LandEncounters[n].char2.model11 = "Mask";
	LandEncounters[n].char2.model11.ani = "man";
	LandEncounters[n].char2.model12 = "Mask_2";
	LandEncounters[n].char2.model12.ani = "man";
	LandEncounters[n].char2.model13 = "Mask_3";
	LandEncounters[n].char2.model13.ani = "man";
	LandEncounters[n].char2.model14 = "Mask_4";
	LandEncounters[n].char2.model14.ani = "man";
	LandEncounters[n].char2.cont = true;
	LandEncounters[n].char2.pistol = true;
	LandEncounters[n].char2.sword = true;
	LandEncounters[n].char2.type = "warrior";
	LandEncounters[n].char2.Dialog = "Enc_Raiders_dialog.c";


	LandEncounters[n].char3.modelQuantity = 14;
	LandEncounters[n].char3.model1 = "sailor1";
	LandEncounters[n].char3.model1.ani = "man";
	LandEncounters[n].char3.model2 = "sailor2";
	LandEncounters[n].char3.model2.ani = "man";
	LandEncounters[n].char3.model3 = "sailor3";
	LandEncounters[n].char3.model3.ani = "man";
	LandEncounters[n].char3.model4 = "sailor4";
	LandEncounters[n].char3.model4.ani = "man";
	LandEncounters[n].char3.model5 = "chelovek";
	LandEncounters[n].char3.model5.ani = "man";
	LandEncounters[n].char3.model6 = "chelovek1";
	LandEncounters[n].char3.model6.ani = "man";
	LandEncounters[n].char3.model7 = "fisherman";
	LandEncounters[n].char3.model7.ani = "man";
	LandEncounters[n].char3.model8 = "fisherman1";
	LandEncounters[n].char3.model8.ani = "man";
	LandEncounters[n].char3.model9 = "man5_1";
	LandEncounters[n].char3.model9.ani = "man";
	LandEncounters[n].char3.model9 = "bocman4";
	LandEncounters[n].char3.model9.ani = "man";
	LandEncounters[n].char3.model10 = "sailor5";
	LandEncounters[n].char3.model10.ani = "man";
	LandEncounters[n].char3.model11 = "Mask";
	LandEncounters[n].char3.model11.ani = "man";
	LandEncounters[n].char3.model12 = "Mask_2";
	LandEncounters[n].char3.model12.ani = "man";
	LandEncounters[n].char3.model13 = "Mask_3";
	LandEncounters[n].char3.model13.ani = "man";
	LandEncounters[n].char3.model14 = "Mask_4";
	LandEncounters[n].char3.model14.ani = "man";
	LandEncounters[n].char3.cont = true;
	LandEncounters[n].char3.pistol = true;
	LandEncounters[n].char3.sword = true;
	LandEncounters[n].char3.type = "warrior";
	LandEncounters[n].char3.Dialog = "Enc_Raiders_dialog.c";


	LandEncounters[n].char4.modelQuantity = 14;
	LandEncounters[n].char4.model1 = "sailor1";
	LandEncounters[n].char4.model1.ani = "man";
	LandEncounters[n].char4.model2 = "sailor2";
	LandEncounters[n].char4.model2.ani = "man";
	LandEncounters[n].char4.model3 = "sailor3";
	LandEncounters[n].char4.model3.ani = "man";
	LandEncounters[n].char4.model4 = "sailor4";
	LandEncounters[n].char4.model4.ani = "man";
	LandEncounters[n].char4.model5 = "chelovek";
	LandEncounters[n].char4.model5.ani = "man";
	LandEncounters[n].char4.model6 = "chelovek1";
	LandEncounters[n].char4.model6.ani = "man";
	LandEncounters[n].char4.model7 = "fisherman";
	LandEncounters[n].char4.model7.ani = "man";
	LandEncounters[n].char4.model8 = "fisherman1";
	LandEncounters[n].char4.model8.ani = "man";
	LandEncounters[n].char4.model9 = "man5_1";
	LandEncounters[n].char4.model9.ani = "man";
	LandEncounters[n].char4.model9 = "bocman4";
	LandEncounters[n].char4.model9.ani = "man";
	LandEncounters[n].char4.model10 = "sailor5";
	LandEncounters[n].char4.model10.ani = "man";
	LandEncounters[n].char4.model11 = "Mask";
	LandEncounters[n].char4.model11.ani = "man";
	LandEncounters[n].char4.model12 = "Mask_2";
	LandEncounters[n].char4.model12.ani = "man";
	LandEncounters[n].char4.model13 = "Mask_3";
	LandEncounters[n].char4.model13.ani = "man";
	LandEncounters[n].char4.model14 = "Mask_4";
	LandEncounters[n].char4.model14.ani = "man";
	LandEncounters[n].char4.cont = false;
	LandEncounters[n].char4.pistol = true;
	LandEncounters[n].char4.sword = true;
	LandEncounters[n].char4.type = "warrior";
	LandEncounters[n].char4.Dialog = "Enc_Raiders_dialog.c";

	n = n + 1;

//----------------------------------------------------------------------------------------------

	LandEncounters[n].id = "ENC_RAPERS";
	LandEncounters[n].QuestType = "location";
	LandEncounters[n].StartingQuest = "LandEnc_RapersStart";
	LAi_group_Register("ENC_RAPERS_GROUP");

	LandEncounters[n].char1.modelQuantity = 9;
	LandEncounters[n].char1.model1 = "towngirl1";
	LandEncounters[n].char1.model1.ani = "woman";
	LandEncounters[n].char1.model2 = "towngirl1_1";
	LandEncounters[n].char1.model2.ani = "woman";
	LandEncounters[n].char1.model3 = "towngirl2";
	LandEncounters[n].char1.model3.ani = "woman";
	LandEncounters[n].char1.model4 = "towngirl2_1";
	LandEncounters[n].char1.model4.ani = "woman";
	LandEncounters[n].char1.model5 = "towngirl3";
	LandEncounters[n].char1.model5.ani = "woman";
	LandEncounters[n].char1.model6 = "towngirl3_1";
	LandEncounters[n].char1.model6.ani = "woman";
	LandEncounters[n].char1.model7 = "towngirl4";
	LandEncounters[n].char1.model7.ani = "woman";
	LandEncounters[n].char1.model8 = "towngirl5";
	LandEncounters[n].char1.model8.ani = "woman";
	LandEncounters[n].char1.model9 = "towngirl6";
	LandEncounters[n].char1.model9.ani = "woman";
	LandEncounters[n].char1.model9 = "towngirl7";
	LandEncounters[n].char1.model9.ani = "woman";	
	LandEncounters[n].char1.cont = true;
	LandEncounters[n].char1.pistol = false;
	LandEncounters[n].char1.sword = false;
	LandEncounters[n].char1.type = "actor";
	LandEncounters[n].char1.Dialog = "Enc_RapersGirl_dialog.c";


	LandEncounters[n].char2.modelQuantity = 10;
	LandEncounters[n].char2.model1 = "sailor1";
	LandEncounters[n].char2.model1.ani = "man";
	LandEncounters[n].char2.model2 = "sailor2";
	LandEncounters[n].char2.model2.ani = "man";
	LandEncounters[n].char2.model3 = "sailor3";
	LandEncounters[n].char2.model3.ani = "man";
	LandEncounters[n].char2.model4 = "sailor4";
	LandEncounters[n].char2.model4.ani = "man";
	LandEncounters[n].char2.model5 = "chelovek";
	LandEncounters[n].char2.model5.ani = "man";
	LandEncounters[n].char2.model6 = "chelovek1";
	LandEncounters[n].char2.model6.ani = "man";
	LandEncounters[n].char2.model7 = "fisherman";
	LandEncounters[n].char2.model7.ani = "man";
	LandEncounters[n].char2.model8 = "fisherman1";
	LandEncounters[n].char2.model8.ani = "man";
	LandEncounters[n].char2.model9 = "man5_1";
	LandEncounters[n].char2.model9.ani = "man";
	LandEncounters[n].char2.model9 = "bocman4";
	LandEncounters[n].char2.model9.ani = "man";
	LandEncounters[n].char2.model10 = "sailor5";
	LandEncounters[n].char2.model10.ani = "man";
	LandEncounters[n].char2.cont = true;
	LandEncounters[n].char2.pistol = true;
	LandEncounters[n].char2.sword = true;
	LandEncounters[n].char2.type = "actor";
	LandEncounters[n].char2.Dialog = "Enc_Rapers_dialog.c";


	LandEncounters[n].char3.modelQuantity = 10;
	LandEncounters[n].char3.model1 = "sailor1";
	LandEncounters[n].char3.model1.ani = "man";
	LandEncounters[n].char3.model2 = "sailor2";
	LandEncounters[n].char3.model2.ani = "man";
	LandEncounters[n].char3.model3 = "sailor3";
	LandEncounters[n].char3.model3.ani = "man";
	LandEncounters[n].char3.model4 = "sailor4";
	LandEncounters[n].char3.model4.ani = "man";
	LandEncounters[n].char3.model5 = "chelovek";
	LandEncounters[n].char3.model5.ani = "man";
	LandEncounters[n].char3.model6 = "chelovek1";
	LandEncounters[n].char3.model6.ani = "man";
	LandEncounters[n].char3.model7 = "fisherman";
	LandEncounters[n].char3.model7.ani = "man";
	LandEncounters[n].char3.model8 = "fisherman1";
	LandEncounters[n].char3.model8.ani = "man";
	LandEncounters[n].char3.model9 = "man5_1";
	LandEncounters[n].char3.model9.ani = "man";
	LandEncounters[n].char3.model9 = "bocman4";
	LandEncounters[n].char3.model9.ani = "man";
	LandEncounters[n].char3.model10 = "sailor5";
	LandEncounters[n].char3.model10.ani = "man";
	LandEncounters[n].char3.cont = true;
	LandEncounters[n].char3.pistol = true;
	LandEncounters[n].char3.sword = true;
	LandEncounters[n].char3.type = "actor";
	LandEncounters[n].char3.Dialog = "Enc_Rapers_dialog.c";


	LandEncounters[n].char4.modelQuantity = 10;
	LandEncounters[n].char4.model1 = "sailor1";
	LandEncounters[n].char4.model1.ani = "man";
	LandEncounters[n].char4.model2 = "sailor2";
	LandEncounters[n].char4.model2.ani = "man";
	LandEncounters[n].char4.model3 = "sailor3";
	LandEncounters[n].char4.model3.ani = "man";
	LandEncounters[n].char4.model4 = "sailor4";
	LandEncounters[n].char4.model4.ani = "man";
	LandEncounters[n].char4.model5 = "chelovek";
	LandEncounters[n].char4.model5.ani = "man";
	LandEncounters[n].char4.model6 = "chelovek1";
	LandEncounters[n].char4.model6.ani = "man";
	LandEncounters[n].char4.model7 = "fisherman";
	LandEncounters[n].char4.model7.ani = "man";
	LandEncounters[n].char4.model8 = "fisherman1";
	LandEncounters[n].char4.model8.ani = "man";
	LandEncounters[n].char4.model9 = "man5_1";
	LandEncounters[n].char4.model9.ani = "man";
	LandEncounters[n].char4.model9 = "bocman4";
	LandEncounters[n].char4.model9.ani = "man";
	LandEncounters[n].char4.model10 = "sailor5";
	LandEncounters[n].char4.model10.ani = "man";
	LandEncounters[n].char4.cont = false;
	LandEncounters[n].char4.pistol = true;
	LandEncounters[n].char4.sword = true;
	LandEncounters[n].char4.type = "actor";
	LandEncounters[n].char4.Dialog = "Enc_Rapers_dialog.c";


	n = n + 1;

//----------------------------------------------------------------------------------------------

	LandEncounters[n].id = "ENC_OFFICER";

	LandEncounters[n].char1.modelQuantity = 10;
	LandEncounters[n].char1.model1 = "blackman";
	LandEncounters[n].char1.model1.ani = "man";
	LandEncounters[n].char1.model2 = "corsair1_1";
	LandEncounters[n].char1.model2.ani = "man";
	LandEncounters[n].char1.model3 = "korsar";
	LandEncounters[n].char1.model3.ani = "man";
	LandEncounters[n].char1.model4 = "chameleon";
	LandEncounters[n].char1.model4.ani = "man";
	LandEncounters[n].char1.model5 = "man1_1";
	LandEncounters[n].char1.model5.ani = "man";
	LandEncounters[n].char1.model6 = "man3_1";
	LandEncounters[n].char1.model6.ani = "man";
	LandEncounters[n].char1.model7 = "sailor3";
	LandEncounters[n].char1.model7.ani = "man";
	LandEncounters[n].char1.model8 = "sailor6";
	LandEncounters[n].char1.model8.ani = "man";
	LandEncounters[n].char1.model9 = "bocman1";
	LandEncounters[n].char1.model9.ani = "man";
	LandEncounters[n].char1.model9 = "bocman4";
	LandEncounters[n].char1.model9.ani = "man";
	LandEncounters[n].char1.model10 = "Black_Corsair";
	LandEncounters[n].char1.model10.ani = "man";
	LandEncounters[n].char1.cont = false;
	LandEncounters[n].char1.pistol = false;
	LandEncounters[n].char1.sword = true;
	LandEncounters[n].char1.type = "sit";
	LandEncounters[n].char1.Dialog = "Enc_Companion_dialog.c";


	//n = n + 1;

	return n;

}