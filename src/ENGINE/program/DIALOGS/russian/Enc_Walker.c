// диалог из ПКМ
#include "DIALOGS\russian\Enc_Walker.h"
#include "DIALOGS\russian\Rumours\Common_rumours.c"  //homo 25/06/06
void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, Diag;
	string NPC_Meeting, TempInfoCharacterID_1, TempInfoCharacterID_2;
	int Sum;
	
	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(Diag, NPChar.Dialog);

    ProcessCommonDialogRumors(NPChar, Link, Diag);//homo 16/06/06

	string iDay, iMonth;
	iDay = environment.date.day;
	iMonth = environment.date.month;
	string lastspeak_date = iday + " " + iMonth;

	
	switch(Dialog.CurrentNode)
	{
		// ----------------------------------- Диалог первый - первая встреча
		
		case "First time":
			if (NPChar.sex != "man")
			{
                dialog.text = RandPhraseSimple("Мой муж запрещает говорить с незнакомцами!",
                         "Мне не о чем с вами говорить.");
				Link.l1 = "Ну и не нужно";
				Link.l1.go = "exit";
                break;
			}
			link.l10 = LinkRandPhrase ("Расскажи мне лучше, что за байки в ходу в здешней таверне?",
									"Лучше скажим мне, новенького в этих краях?",
									"Расскажи-ка мне лучше, приятель, как течет жизнь на суше?");
			link.l10.go = "rumours_citizen"; //повесил на горожанина, если нужны отедельные слухи, то пишем соответствующую реализацию
			if (npchar.quest.Meeting != LastSpeakDate() || bBettaTestMode)
		    {
				npchar.quest.last_theme = Rand(17);
				npchar.quest.Meeting = LastSpeakDate();
			}
			switch (sti(npchar.quest.last_theme))
			{	
				case 0:
					dialog.text = DLG_TEXT[0]; 
					Link.l1 = DLG_TEXT[1];
					Link.l1.go = "skeletons";
					Link.l2 = DLG_TEXT[2];
					Link.l2.go = "exit";
				break;

				case 1:
					dialog.text = DLG_TEXT[3];
					Link.l1 = DLG_TEXT[4];
					Link.l1.go = "police";
					Link.l2 = DLG_TEXT[5];
					Link.l2.go = "exit";
				break;

				case 2:
					dialog.text = DLG_TEXT[6];
					Link.l1 = DLG_TEXT[7];
					Link.l1.go = "health";
					Link.l2 = DLG_TEXT[8];
					Link.l2.go = "exit";
				break;

				case 3:
					dialog.text = DLG_TEXT[9];
					Link.l1 = DLG_TEXT[10];
					Link.l1.go = "tavern";
					Link.l2 = DLG_TEXT[11];
					Link.l2.go = "exit";
				break;

				case 4:
					dialog.text = DLG_TEXT[12];
					Link.l1 = DLG_TEXT[13];
					Link.l1.go = "officer";
					Link.l2 = DLG_TEXT[14];
					Link.l2.go = "exit";
				break;

				case 5:
					dialog.text = DLG_TEXT[15];
					Link.l1 = DLG_TEXT[16];
					Link.l1.go = "shipyard";
					Link.l2 = DLG_TEXT[17];
					Link.l2.go = "exit";
				break;

				case 6:
					dialog.text = DLG_TEXT[18];
					Link.l1 = DLG_TEXT[19];
					Link.l1.go = "torg";
					Link.l2 = DLG_TEXT[20];
					Link.l2.go = "exit_once_more";
				break;

				case 7:
					dialog.text = DLG_TEXT[21];
					Link.l1 = DLG_TEXT[22];
					Link.l1.go = "healing";
					Link.l2 = DLG_TEXT[23];
					Link.l2.go = "exit_once_more_portion";
				break;

				case 8:
					dialog.text = DLG_TEXT[24];
					Link.l1 = DLG_TEXT[25];
					Link.l1.go = "reput";
					Link.l2 = DLG_TEXT[26];
					Link.l2.go = "exit_out";
				break;

				case 9:
					if (1 < Rand(20))
					{
						switch(Rand(1))
						{
							case 0:
								dialog.text = DLG_TEXT[27];
								Link.l1 = DLG_TEXT[28];
								Link.l1.go = "beg";
								Link.l2 = DLG_TEXT[29];
								Link.l2.go = "exit_out";
							break;

							case 1:
								dialog.text = DLG_TEXT[30];
								Link.l1 = DLG_TEXT[31];
								Link.l1.go = "beg";
								Link.l2 = DLG_TEXT[32];
								Link.l2.go = "exit_out";
							break;
						}
					}
					else
					{
						dialog.text = DLG_TEXT[33];
						Link.l1 = DLG_TEXT[34];
						Link.l1.go = "skill";
						Link.l2 = DLG_TEXT[35];
						Link.l2.go = "exit_out";
					}
				break;
				
				case 10:
					dialog.text = DLG_TEXT[36];
					Link.l1 = DLG_TEXT[37];
					Link.l1.go = "enlist_me";
					Link.l2 = DLG_TEXT[38];
					Link.l2.go = "exit_out";
				break;

				case 11:
					dialog.text = DLG_TEXT[39];
					Link.l1 = DLG_TEXT[40];
					Link.l1.go = "masters";
					Link.l2 = DLG_TEXT[41];
					Link.l2.go = "exit";
				break;
				// дубли
				case 12:
					dialog.text = DLG_TEXT[39];
					Link.l1 = DLG_TEXT[40];
					Link.l1.go = "masters";
					Link.l2 = DLG_TEXT[41];
					Link.l2.go = "exit";
				break;
				
				case 13:
					dialog.text = DLG_TEXT[39];
					Link.l1 = DLG_TEXT[40];
					Link.l1.go = "masters";
					Link.l2 = DLG_TEXT[41];
					Link.l2.go = "exit";
				break;
				
				case 14:
					dialog.text = DLG_TEXT[18];
					Link.l1 = DLG_TEXT[19];
					Link.l1.go = "torg";
					Link.l2 = DLG_TEXT[20];
					Link.l2.go = "exit_once_more";
				break;
				
				case 15:
					dialog.text = DLG_TEXT[15];
					Link.l1 = DLG_TEXT[16];
					Link.l1.go = "shipyard";
					Link.l2 = DLG_TEXT[17];
					Link.l2.go = "exit";
				break;
				
				case 16:
					dialog.text = DLG_TEXT[0];
					Link.l1 = DLG_TEXT[1];
					Link.l1.go = "skeletons";
					Link.l2 = DLG_TEXT[2];
					Link.l2.go = "exit";
				break;

				case 17:
					dialog.text = DLG_TEXT[3];
					Link.l1 = DLG_TEXT[4];
					Link.l1.go = "police";
					Link.l2 = DLG_TEXT[5];
					Link.l2.go = "exit";
				break;
			}
		break;

		case "skeletons":
			dialog.text = DLG_TEXT[42];
			Link.l1 = DLG_TEXT[43];
			Link.l1.go = "exit";
		break;

		case "police":
			dialog.text = DLG_TEXT[44];
			Link.l1 = DLG_TEXT[45];
			Link.l1.go = "police1";
			Link.l2 = DLG_TEXT[46];
			Link.l2.go = "exit";
		break;

		case "police1":
			dialog.text = DLG_TEXT[47];
			Link.l1 = DLG_TEXT[48];
			Link.l1.go = "exit";
		break;

		case "health":
			dialog.text = DLG_TEXT[49];
			Link.l1 = DLG_TEXT[50];
			Link.l1.go = "health1";
			Link.l2 = DLG_TEXT[51];
			Link.l2.go = "exit";
		break;

		case "health1":
			dialog.text = DLG_TEXT[52];
			Link.l1 = DLG_TEXT[53];
			Link.l1.go = "exit";
		break;

		case "tavern":
			dialog.text = DLG_TEXT[54];
			Link.l1 = DLG_TEXT[55];
			Link.l1.go = "exit";
		break;

		case "officer":
			dialog.text = DLG_TEXT[56];
			Link.l1 = DLG_TEXT[57];
			Link.l1.go = "exit";
		break;

		case "shipyard":
			dialog.text = DLG_TEXT[58];
			Link.l1 = DLG_TEXT[59];
			Link.l1.go = "exit";
		break;

		case "masters":
			dialog.text = DLG_TEXT[60];
			Link.l1 = DLG_TEXT[61];
			Link.l1.go = "exit";
		break;

		case "torg":
			if (CheckNPCQuestDate(npchar, "Torg_date"))
			{
                SetNPCQuestDate(npchar, "Torg_date");
				GiveItemToTrader(npchar);
			}

			Diag.CurrentNode = "once_more";
			DialogExit();

			LaunchItemsTrade(NPChar);
		break;

		case "healing":
			AddMoneytoCharacter(Pchar, -200);
			LAi_SetCurHP(pchar, LAi_GetCharacterMaxHP(pchar));
			Diag.CurrentNode = "once_more_portion";
			DialogExit();
		break;

		case "healing1":
			LAi_SetCurHP(pchar, LAi_GetCharacterMaxHP(pchar));
			Diag.CurrentNode = "thanx";
			DialogExit();
		break;

		case "reput":
			dialog.text = DLG_TEXT[62];
			Link.l1 = DLG_TEXT[63];
			Link.l1.go = "exit_out";
			Link.l2 = DLG_TEXT[64];
			Link.l2.go = "exit1";
		break;

		case "beg":
			AddMoneyToCharacter(pchar, -200);
			switch (Rand(2))
			{
					case 0:
						dialog.text = DLG_TEXT[65];
						Link.l1 = DLG_TEXT[66];
						Link.l1.go = "healing1";
					break;

					case 1:
						dialog.text = DLG_TEXT[67];
						Link.l1 = DLG_TEXT[68];
						Link.l1.go = "item";
					break;

					case 2:
						dialog.text = DLG_TEXT[69];
						Link.l1 = DLG_TEXT[70];
						Link.l1.go = "exit_than";
					break;
			}
		break;

		case "item":
			switch (Rand(10))
			{
				case 0:
					GiveItem2Character(pchar, "indian1");
				break;

				case 2:
					GiveItem2Character(pchar, "indian2");
				break;

				case 3:
					GiveItem2Character(pchar, "indian3");
				break;

				case 4:
					GiveItem2Character(pchar, "indian4");
				break;

				case 5:
					GiveItem2Character(pchar, "indian5");
				break;

				case 6:
					GiveItem2Character(pchar, "indian6");
				break;

				case 7:
					GiveItem2Character(pchar, "indian7");
				break;
				
				case 8:
					GiveItem2Character(pchar, "indian8");
				break;

				case 9:
					GiveItem2Character(pchar, "indian9");
				break;
				
				case 10:
					GiveItem2Character(pchar, "indian10");
				break;
			}
			Diag.CurrentNode = "thanx";
			DialogExit();
		break;

		case "enlist_me":
			dialog.text = DLG_TEXT[71];
			Link.l2 = DLG_TEXT[73];
			Link.l2.go = "exit_out";
		break;
		
		case "skill":

			switch (Rand(7))
			{
				case 0:
					dialog.text = DLG_TEXT[74];
					if(makeint(PChar.skill.Fencing)>= SKILL_MAX)
					{
						Link.l1 = DLG_TEXT[75];
						Link.l1.go = "exit_Fencing";
					}
					else
					{
						Link.l1 = DLG_TEXT[78];
						Link.l1.go = "Fencing";
					}
					if(makeint(PChar.skill.Defence)>= SKILL_MAX)
					{
						Link.l1 = DLG_TEXT[77];
						Link.l1.go = "exit_out";
					}
					else
					{
						Link.l2 = DLG_TEXT[76];
						Link.l2.go = "Defence";
					}
					Link.l3 = DLG_TEXT[79];
					Link.l3.go = "exit_Fencing";
				break;

				case 1:
					dialog.text = DLG_TEXT[80];
					if(makeint(PChar.skill.Leadership)>= SKILL_MAX)
					{
						Link.l1 = DLG_TEXT[81];
						Link.l1.go = "exit_out";
					}
					else
					{
						Link.l1 = DLG_TEXT[82];
						Link.l1.go = "Leadership";
					}
					Link.l2 = DLG_TEXT[83];
					Link.l2.go = "exit_Leadership";
				break;

				case 2:
					dialog.text = DLG_TEXT[84];
					if(makeint(PChar.skill.Sailing)>= SKILL_MAX)
					{
						Link.l1 = DLG_TEXT[85];
						Link.l1.go = "exit_out";
					}
					else
					{
						Link.l1 = DLG_TEXT[86];
						Link.l1.go = "Sailing";
					}
					Link.l2 = DLG_TEXT[87];
					Link.l2.go = "exit_Sailing";
				break;

				case 3:
					dialog.text = DLG_TEXT[88];
					if(makeint(PChar.skill.Accuracy)>= SKILL_MAX)
					{
						Link.l1 = DLG_TEXT[89];
						Link.l1.go = "exit_out";
					}
					else
					{
						Link.l1 = DLG_TEXT[90];
						Link.l1.go = "Accuracy";
					}
					Link.l2 = DLG_TEXT[91];
					Link.l2.go = "exit_Accuracy";
				break;

				case 4:
					dialog.text = DLG_TEXT[92];
					if(makeint(PChar.skill.Cannons)>= SKILL_MAX)
					{
						Link.l1 = DLG_TEXT[93];
						Link.l1.go = "exit_out";
					}
					else
					{
						Link.l1 = DLG_TEXT[94];
						Link.l1.go = "Cannons";

					}
					Link.l2 = DLG_TEXT[95];
					Link.l2.go = "exit_Cannons";
				break;

				case 5:
					dialog.text = DLG_TEXT[96];
					if(makeint(PChar.skill.Grappling)>= SKILL_MAX)
					{
						Link.l1 = DLG_TEXT[97];
						Link.l1.go = "exit_out";
					}
					else
					{
						Link.l1 = DLG_TEXT[98];
						Link.l1.go = "Grappling";
					}
					Link.l2 = DLG_TEXT[99];
					Link.l2.go = "exit_Grappling";
				break;

				case 6:
					dialog.text = DLG_TEXT[100];
					if(makeint(PChar.skill.Repair)>= SKILL_MAX)
					{
						Link.l1 = DLG_TEXT[101];
						Link.l1.go = "exit_out";
					}
					else
					{
						Link.l1 = DLG_TEXT[102];
						Link.l1.go = "Repair";
					}
					Link.l2 = DLG_TEXT[103];
					Link.l2.go = "exit_Repair";
				break;
	
				case 7:
					dialog.text = DLG_TEXT[104];
					if(makeint(PChar.skill.Commerce)>=SKILL_MAX)
					{
						Link.l1 = DLG_TEXT[105];
						Link.l1.go = "exit_out";
					}
					else
					{
						Link.l1 = DLG_TEXT[106];
						Link.l1.go = "Commerce";
					}
					Link.l2 = DLG_TEXT[107];
					Link.l2.go = "exit_Commerce";
				break;
			}
		break;

		case "Fencing":
			Sum = (sti(Pchar.skill.Fencing) + 1) * 10000;
			dialog.text = DLG_TEXT[108] + Sum + DLG_TEXT[109];
			if(makeint(Pchar.money) < Sum)
			{
				Link.l1 = DLG_TEXT[110];
				Link.l1.go = "exit_Fencing";		
			}
			else
			{
				Link.l1 = DLG_TEXT[111];
				Link.l1.go = "Fencing1";
				Link.l2 = DLG_TEXT[112];
				Link.l2.go = "exit_Fencing";
			}
		break;

		case "Fencing2":
			Sum = (sti(Pchar.skill.Fencing) + 1) * 10000;
			dialog.text = DLG_TEXT[113] + Sum + DLG_TEXT[114];
			if(makeint(Pchar.money) < Sum)
			{
				Link.l1 = DLG_TEXT[115];
				Link.l1.go = "exit_Fencing";		
			}
			else
			{
				Link.l1 = DLG_TEXT[116];
				Link.l1.go = "Fencing1";
				Link.l2 = DLG_TEXT[117];
				Link.l2.go = "exit_Fencing";
			}
		break;

		case "Fencing1":
			Sum = (sti(Pchar.skill.Fencing) + 1) * 10000;
			AddCharacterSkill(Pchar, SKILL_FENCING, 10);
			AddMoneytoCharacter(Pchar, -Sum);
			Diag.CurrentNode = "get_out";
			DialogExit();
		break;

		case "Defence":
			Sum = (sti(Pchar.skill.Defence) + 1) * 10000;
			dialog.text = DLG_TEXT[118] + Sum + DLG_TEXT[119];
			if(makeint(Pchar.money) < Sum)
			{
				Link.l1 = DLG_TEXT[120];
				Link.l1.go = "exit_Defence";		
			}
			else
			{
				Link.l1 = DLG_TEXT[121];
				Link.l1.go = "Defence1";
				Link.l2 = DLG_TEXT[122];
				Link.l2.go = "exit_Defence";
			}
		break;

		case "Defence2":
			Sum = (sti(Pchar.skill.Defence) + 1) * 10000;
			dialog.text = DLG_TEXT[123] + Sum + DLG_TEXT[124];
			if(makeint(Pchar.money) < Sum)
			{
				Link.l1 = DLG_TEXT[125];
				Link.l1.go = "exit_Defence";		
			}
			else
			{
				Link.l1 = DLG_TEXT[126];
				Link.l1.go = "Defence1";
				Link.l2 = DLG_TEXT[127];
				Link.l2.go = "exit_Defence";
			}
		break;

		case "Defence1":
			Sum = (sti(Pchar.skill.Defence) + 1) * 10000;
            AddCharacterSkill(Pchar, SKILL_DEFENCE, 10);
			AddMoneytoCharacter(Pchar, -Sum);
			Diag.CurrentNode = "get_out";
			DialogExit();
		break;

		case "Leadership":
			Sum = (sti(Pchar.skill.Leadership) + 1) * 10000;
			dialog.text = DLG_TEXT[128] + Sum + DLG_TEXT[129];
			if(makeint(Pchar.money) < Sum)
			{
				Link.l1 = DLG_TEXT[130];
				Link.l1.go = "exit_Leadership";		
			}
			else
			{
				Link.l1 = DLG_TEXT[131];
				Link.l1.go = "Leadership1";
				Link.l2 = DLG_TEXT[132];
				Link.l2.go = "exit_Leadership";
			}
		break;

		case "Leadership2":
			Sum = (sti(Pchar.skill.Leadership) + 1) * 10000;
			dialog.text = DLG_TEXT[133] + Sum + DLG_TEXT[134];
			if(makeint(Pchar.money) < Sum)
			{
				Link.l1 = DLG_TEXT[135];
				Link.l1.go = "exit_Leadership";		
			}
			else
			{
				Link.l1 = DLG_TEXT[136];
				Link.l1.go = "Leadership1";
				Link.l2 = DLG_TEXT[137];
				Link.l2.go = "exit_Leadership";
			}
		break;

		case "Leadership1":
			Sum = (sti(Pchar.skill.Leadership) + 1) * 10000;
            AddCharacterSkill(Pchar, SKILL_LEADERSHIP, 10);
			AddMoneytoCharacter(Pchar, -Sum);
			Diag.CurrentNode = "get_out";
			DialogExit();
		break;

		case "Sailing":
			Sum = (sti(Pchar.skill.Sailing) + 1) * 10000;
			dialog.text = DLG_TEXT[138] + Sum + DLG_TEXT[139];
			if(makeint(Pchar.money) < Sum)
			{
				Link.l1 = DLG_TEXT[140];
				Link.l1.go = "exit_Sailing";		
			}
			else
			{
				Link.l1 = DLG_TEXT[141];
				Link.l1.go = "Sailing1";
				Link.l2 = DLG_TEXT[142];
				Link.l2.go = "exit_Sailing";
			}
		break;

		case "Sailing2":
			Sum = (sti(Pchar.skill.Sailing) + 1) * 10000;
			dialog.text = DLG_TEXT[143] + Sum + DLG_TEXT[144];
			if(makeint(Pchar.money) < Sum)
			{
				Link.l1 = DLG_TEXT[145];
				Link.l1.go = "exit_Sailing";		
			}
			else
			{
				Link.l1 = DLG_TEXT[146];
				Link.l1.go = "Sailing1";
				Link.l2 = DLG_TEXT[147];
				Link.l2.go = "exit_Sailing";
			}
		break;

		case "Sailing1":
			Sum = (sti(Pchar.skill.Sailing) + 1) * 10000;
            AddCharacterSkill(Pchar, SKILL_SAILING, 10);
			AddMoneytoCharacter(Pchar, - Sum);
			Diag.CurrentNode = "get_out";
			DialogExit();
		break;

		case "Accuracy":
			Sum = (sti(Pchar.skill.Accuracy) + 1) * 10000;
			dialog.text = DLG_TEXT[148] + Sum + DLG_TEXT[149];
			if(makeint(Pchar.money) < Sum)
			{
				Link.l1 = DLG_TEXT[150];
				Link.l1.go = "exit_Accuracy";		
			}
			else
			{
				Link.l1 = DLG_TEXT[151];
				Link.l1.go = "Accuracy1";
				Link.l2 = DLG_TEXT[152];
				Link.l2.go = "exit_Accuracy";
			}
		break;

		case "Accuracy2":
			Sum = (sti(Pchar.skill.Accuracy) + 1) * 10000;
			dialog.text = DLG_TEXT[153] + Sum + DLG_TEXT[154];
			if(makeint(Pchar.money) < Sum)
			{
				Link.l1 = DLG_TEXT[155];
				Link.l1.go = "exit_Accuracy";		
			}
			else
			{
				Link.l1 = DLG_TEXT[156];
				Link.l1.go = "Accuracy1";
				Link.l2 = DLG_TEXT[157];
				Link.l2.go = "exit_Accuracy";
			}
		break;

		case "Accuracy1":
			Sum = (sti(Pchar.skill.Accuracy) + 1) * 10000;
            AddCharacterSkill(Pchar, SKILL_ACCURACY, 10);
			AddMoneytoCharacter(Pchar, -Sum);
			Diag.CurrentNode = "get_out";
			DialogExit();
		break;

		case "Cannons":
			Sum = (sti(Pchar.skill.Cannons) + 1) * 10000;
			dialog.text = DLG_TEXT[158] + Sum + DLG_TEXT[159];
			if(makeint(Pchar.money) < Sum)
			{
				Link.l1 = DLG_TEXT[160];
				Link.l1.go = "exit_Cannons";		
			}
			else
			{
				Link.l1 = DLG_TEXT[161];
				Link.l1.go = "Cannons1";
				Link.l2 = DLG_TEXT[162];
				Link.l2.go = "exit_Cannons";
			}
		break;

		case "Cannons2":
			Sum = (sti(Pchar.skill.Cannons) + 1) * 10000;
			dialog.text = DLG_TEXT[163] + Sum + DLG_TEXT[164];
			if(makeint(Pchar.money) < Sum)
			{
				Link.l1 = DLG_TEXT[165];
				Link.l1.go = "exit_Cannons";		
			}
			else
			{
				Link.l1 = DLG_TEXT[166];
				Link.l1.go = "Cannons1";
				Link.l2 = DLG_TEXT[167];
				Link.l2.go = "exit_Cannons";
			}
		break;

		case "Cannons1":
			Sum = (sti(Pchar.skill.Cannons) + 1) * 10000;
            AddCharacterSkill(Pchar, SKILL_CANNONS, 10);
			AddMoneytoCharacter(Pchar, -Sum);
			Diag.CurrentNode = "get_out";
			DialogExit();
		break;

		case "Grappling":
			Sum = (sti(Pchar.skill.Grappling) + 1) * 10000;
			dialog.text = DLG_TEXT[168] + Sum + DLG_TEXT[169];
			if(makeint(Pchar.money) < Sum)
			{
				Link.l1 = DLG_TEXT[170];
				Link.l1.go = "exit_Grappling";		
			}
			else
			{
				Link.l1 = DLG_TEXT[171];
				Link.l1.go = "Grappling1";
				Link.l2 = DLG_TEXT[172];
				Link.l2.go = "exit_Grappling";
			}
		break;

		case "Grappling2":
			Sum = (sti(Pchar.skill.Grappling) + 1) * 10000;
			dialog.text = DLG_TEXT[173] + Sum + DLG_TEXT[174];
			if(makeint(Pchar.money) < Sum)
			{
				Link.l1 = DLG_TEXT[175];
				Link.l1.go = "exit_Grappling";		
			}
			else
			{
				Link.l1 = DLG_TEXT[176];
				Link.l1.go = "Grappling1";
				Link.l2 = DLG_TEXT[177];
				Link.l2.go = "exit_Grappling";
			}
		break;

		case "Grappling1":
			Sum = (sti(Pchar.skill.Grappling) + 1) * 10000;
            AddCharacterSkill(Pchar, SKILL_GRAPPLING, 10);
			AddMoneytoCharacter(Pchar, -Sum);
			Diag.CurrentNode = "get_out";
			DialogExit();
		break;

		case "Repair":
			Sum = (sti(Pchar.skill.Repair) + 1) * 10000;
			dialog.text = DLG_TEXT[178] + Sum + DLG_TEXT[179];
			if(makeint(Pchar.money) < Sum)
			{
				Link.l1 = DLG_TEXT[180];
				Link.l1.go = "exit_Repair";		
			}
			else
			{
				Link.l1 = DLG_TEXT[181];
				Link.l1.go = "Repair1";
				Link.l2 = DLG_TEXT[182];
				Link.l2.go = "exit_Repair";
			}
		break;

		case "Repair2":
			Sum = (sti(Pchar.skill.Repair) + 1) * 10000;
			dialog.text = DLG_TEXT[183] + Sum + DLG_TEXT[184];
			if(makeint(Pchar.money) < Sum)
			{
				Link.l1 = DLG_TEXT[185];
				Link.l1.go = "exit_Repair";		
			}
			else
			{
				Link.l1 = DLG_TEXT[186];
				Link.l1.go = "Repair1";
				Link.l2 = DLG_TEXT[187];
				Link.l2.go = "exit_Repair";
			}
		break;

		case "Repair1":
			Sum = (sti(Pchar.skill.Repair) + 1) * 10000;
			AddCharacterSkill(Pchar, SKILL_REPAIR, 10);
			AddMoneytoCharacter(Pchar, -Sum);
			Diag.CurrentNode = "get_out";
			DialogExit();
		break;

		case "Commerce":
			Sum = (sti(Pchar.skill.Commerce) + 1) * 10000;
			dialog.text = DLG_TEXT[188] + Sum + DLG_TEXT[189];
			if(makeint(Pchar.money) < Sum)
			{
				Link.l1 = DLG_TEXT[190];
				Link.l1.go = "exit_Commerce";		
			}
			else
			{
				Link.l1 = DLG_TEXT[191];
				Link.l1.go = "Commerce1";
				Link.l2 = DLG_TEXT[192];
				Link.l2.go = "exit_Commerce";
			}
		break;

		case "Commerce2":
			Sum = (sti(Pchar.skill.Commerce) + 1) * 10000;
			dialog.text = DLG_TEXT[193] + Sum + DLG_TEXT[194];
			if(makeint(Pchar.money) < Sum)
			{
				Link.l1 = DLG_TEXT[195];
				Link.l1.go = "exit_Commerce";		
			}
			else
			{
				Link.l1 = DLG_TEXT[196];
				Link.l1.go = "Commerce1";
				Link.l2 = DLG_TEXT[197];
				Link.l2.go = "exit_Commerce";
			}
		break;

		case "Commerce1":
			Sum = (sti(Pchar.skill.Commerce) + 1) * 10000;
			AddCharacterSkill(Pchar, SKILL_COMMERCE, 10);
			AddMoneytoCharacter(Pchar, -Sum);
			Diag.CurrentNode = "get_out";
			DialogExit();
		break;

		case "thanx":
			dialog.text = DLG_TEXT[198];
			Link.l1 = DLG_TEXT[199];
			Link.l1.go = "exit_than";
		break;

		case "once_more_portion":
			dialog.text = DLG_TEXT[200];
			Link.l1 = DLG_TEXT[201];
			Link.l1.go = "healing";
			Link.l2 = DLG_TEXT[202];
			Link.l2.go = "exit_once_more_portion";
		break;

		case "once_more":
			dialog.text = DLG_TEXT[203];
			Link.l1 = DLG_TEXT[204];
			Link.l1.go = "torg";
			Link.l2 = DLG_TEXT[205];
			Link.l2.go = "exit_once_more";
		break;

		case "get_out":
			dialog.text = DLG_TEXT[206];
			Link.l2 = DLG_TEXT[207];
			Link.l2.go = "exit_out";
		break;

		case "got_officer":
			dialog.text = DLG_TEXT[208];
			Link.l2 = DLG_TEXT[209];
			Link.l2.go = "exit_off";
		break;

		case "sovet":
			if (NPChar.sex != "man") //fix eddy. второй раз баба говорит как мужик
			{
                dialog.text = RandPhraseSimple("Мой муж запрещает говорить с незнакомцами!",
                         "Мне не о чем с вами говорить.");
				Link.l1 = "Ну и не нужно";
				Link.l1.go = "exit";
                break;
			}
			link.l10 = LinkRandPhrase ("Расскажи мне лучше, что за байки в ходу в здешней таверне?",
									"Лучше скажим мне, новенького в этих краях?",
									"Расскажи ка мне лучше, приятель, как течет жизнь на суше?");
			link.l10.go = "rumours_citizen"; //повесил на горожанина, если нужны отедельные слухи, то пишем соответствующую реализацию
			dialog.text = DLG_TEXT[210];
			Link.l1 = DLG_TEXT[211];
			Link.l1.go = "advises";
			Link.l2 = DLG_TEXT[212];
			Link.l2.go = "exit";
		break;

		case "advises":

			switch(Rand(6))
			{	
				case 0:
					dialog.text = DLG_TEXT[213];
					Link.l1 = DLG_TEXT[214];
					Link.l1.go = "exit";
				break;

				case 1:
					dialog.text = DLG_TEXT[215];
					Link.l1 = DLG_TEXT[216];
					Link.l1.go = "police1";
					Link.l2 = DLG_TEXT[217];
					Link.l2.go = "exit";
				break;

				case 2:
					dialog.text = DLG_TEXT[218];
					Link.l1 = DLG_TEXT[219];
					Link.l1.go = "health1";
					Link.l2 = DLG_TEXT[220];
					Link.l2.go = "exit";
				break;

				case 3:
					dialog.text = DLG_TEXT[221];
					Link.l1 = DLG_TEXT[222];
					Link.l1.go = "exit";
				break;

				case 4:
					dialog.text = DLG_TEXT[223];
					Link.l1 = DLG_TEXT[224];
					Link.l1.go = "exit";
				break;

				case 5:
					dialog.text = DLG_TEXT[225];
					Link.l1 = DLG_TEXT[226];
					Link.l1.go = "exit";
				break;

				case 6:
					dialog.text = DLG_TEXT[227];
					Link.l1 = DLG_TEXT[228];
					Link.l1.go = "exit";
				break;
			}
		break;

		case "exit_Fencing":
			Diag.CurrentNode = "Fencing2";
			DialogExit();
		break;

		case "exit_Defence":
			Diag.CurrentNode = "Defence2";
			DialogExit();
		break;

		case "exit_Leadership":
			Diag.CurrentNode = "Leadership2";
			DialogExit();
		break;

		case "exit_Sailing":
			Diag.CurrentNode = "Sailing2";
			DialogExit();
		break;

		case "exit_Accuracy":
			Diag.CurrentNode = "Accuracy2";
			DialogExit();
		break;

		case "exit_Cannons":
			Diag.CurrentNode = "Cannons2";
			DialogExit();
		break;

		case "exit_Grappling":
			Diag.CurrentNode = "Grappling2";
			DialogExit();
		break;

		case "exit_Repair":
			Diag.CurrentNode = "Repair2";
			DialogExit();
		break;

		case "exit_Commerce":
			Diag.CurrentNode = "Commerce2";
			DialogExit();
		break;

		case "exit":
			Diag.CurrentNode = "sovet";
			DialogExit();
		break;

		case "exit1":
			ChangeCharacterReputation(pchar, 1);
			AddMoneytoCharacter(Pchar, -200);
			Diag.CurrentNode = "thanx";
			DialogExit();
		break;

		case "exit_out":
			Diag.CurrentNode = "get_out";
			DialogExit();
		break;

		case "exit_once_more":
			Diag.CurrentNode = "once_more";
			DialogExit();
		break;

		case "exit_once_more_portion":
			Diag.CurrentNode = "once_more_portion";
			DialogExit();
		break;
		
		case "exit_than":
			Diag.CurrentNode = "thanx";
			DialogExit();
		break;

		case "exit_off":
			Diag.CurrentNode = "got_officer";
			DialogExit();
		break;

		//у горожанина должна быть эта ветка.
		case "new question":
            dialog.text = NPCharRepPhrase(npchar,
		                PCharRepPhrase(LinkRandPhrase("О, да я рад"+NPCharSexPhrase(NPChar, " ", "а ")+" тебе как выпивке, спрашивай, чего хочешь.", "Любишь поболтать, капитан? Что ж, я тоже... Особенно за кружечкой рома.", "Да, " + PChar.name + "?"),
                                        LinkRandPhrase("Ну, чего тебе еще?", "Вижу, капитан "+PChar.name+", ты охотник поболтать? ", "Замучил вопросами, капитан, лучше бы угостил ромом.")),
		                PCharRepPhrase(LinkRandPhrase("Что вы хотели узнать "+ GetAddress_Form(NPChar) + "?", "Я вас слушаю капитан.", "Мне нужно идти, поэтому спрашивайте скорее капитан."),
                                        LinkRandPhrase("Всегда рад"+NPCharSexPhrase(NPChar, " ", "а ")+" приятному собеседнику, " + GetAddress_Form(NPChar) + " " + PChar.lastname + ". Говорите.",
                                                        "Да, " + GetAddress_Form(NPChar) + "?",
                                                        "Любите поболтать, капитан? Что ж, я тоже..."))
                            );

            // homo 25/06/06
			link.l1 = LinkRandPhrase ("Какие байки в ходу в здешней таверне?",
                                    "Что новенького в этих краях?",
                                    "Как течет жизнь на суше?");
			link.l1.go = "rumours_citizen";
			link.l5 = PCharRepPhrase(RandPhraseSimple("Тебе послышалось, я пойду.", "Нет, ничего - просто вышел погулять."),
                                        RandPhraseSimple("Ладно, ничего. Удачи!", "Просто прогуливаюсь. До свидания."));
			link.l5.go = "exit";
			break;
	}
}
