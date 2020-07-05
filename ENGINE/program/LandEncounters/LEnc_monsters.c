

object LEnc_MonstersLoginObject;

//Начать загрузку монстров в локацию
bool LEnc_MonstersLoginStart(ref location)
{
	float monkeys, skeletons;
	LEnc_MonstersLoginObject.type = "dungeon";
	if(CheckAttribute(location, "type"))
	{
		if(location.type == "jungle")
		{
			LEnc_MonstersLoginObject.type = "jungle";
		}
	}
	LEnc_MonstersLoginObject.current = "";
	if(LEnc_MonstersLoginObject.type == "jungle")
	{
		if(IsDay())
		{
			//Днём либо бандиты, либо олухи, либо никто
			if(rand(100) > 20)
			{
				if(rand(100) < 30)
				{
					//Выбрали олухов
					LEnc_MonstersLoginObject.bandit = "0";
					LEnc_MonstersLoginObject.skeleton = "0";
					LEnc_MonstersLoginObject.monkey = "0";
					LEnc_MonstersLoginObject.mummy = "0";
					LEnc_MonstersLoginObject.vagabond = "1";
				}else{
					return false;
				}
			}else{
				LEnc_MonstersLoginObject.bandit = "1";
				LEnc_MonstersLoginObject.skeleton = "0";
				LEnc_MonstersLoginObject.monkey = "0";
				LEnc_MonstersLoginObject.mummy = "0";
				LEnc_MonstersLoginObject.vagabond = "0";
			}
		}else{
			//Ночью либо бандиты, либо скелеты, либо олухи, либо никто
			if(rand(100) < 40)
			{
				if(rand(100) < 20)
				{
					//Выбрали олухов
					LEnc_MonstersLoginObject.bandit = "0";
					LEnc_MonstersLoginObject.skeleton = "0";
					LEnc_MonstersLoginObject.monkey = "0";
					LEnc_MonstersLoginObject.mummy = "0";
					LEnc_MonstersLoginObject.vagabond = "1";
				}else{
					return false;
				}
			}else{
				if(rand(100) < 40)
				{
					//Выбрали бандитов
					LEnc_MonstersLoginObject.bandit = "1";
					LEnc_MonstersLoginObject.skeleton = "0";
					LEnc_MonstersLoginObject.monkey = "0";
					LEnc_MonstersLoginObject.mummy = "0";
					LEnc_MonstersLoginObject.vagabond = "0";
				}else{
					//Выбрали скелетов
					LEnc_MonstersLoginObject.bandit = "0";
					monkeys = 0.4 + (rand(100)*0.01)*0.6;
					skeletons = 1.0 - monkeys;
					LEnc_MonstersLoginObject.skeleton = skeletons;
					LEnc_MonstersLoginObject.monkey = monkeys;
					LEnc_MonstersLoginObject.mummy = "0";
					LEnc_MonstersLoginObject.vagabond = "0";
				}
			}
		}
	}else{
		if(LEnc_MonstersLoginObject.type == "inca_temple")
		{
				LEnc_MonstersLoginObject.bandit = "0";
				LEnc_MonstersLoginObject.skeleton = "0.2";
				LEnc_MonstersLoginObject.monkey = "0.3";
				LEnc_MonstersLoginObject.mummy = "0.5";
				LEnc_MonstersLoginObject.vagabond = "0";
		}else{
			if(rand(100) < 90)
			{
				LEnc_MonstersLoginObject.bandit = "0";
				LEnc_MonstersLoginObject.skeleton = "1";
				LEnc_MonstersLoginObject.monkey = "0";
				LEnc_MonstersLoginObject.mummy = "0";
				LEnc_MonstersLoginObject.vagabond = "0";
			}else{
				skeletons = 0.8 + (rand(100)*0.01)*0.2;
				monkeys = 1.0 - skeletons;
				LEnc_MonstersLoginObject.bandit = "0";
				LEnc_MonstersLoginObject.skeleton = skeletons;
				LEnc_MonstersLoginObject.monkey = monkeys;
				LEnc_MonstersLoginObject.mummy = "0";
				LEnc_MonstersLoginObject.vagabond = "0";
			}
		}
	}
	return true;
}

//Установить модельку для монстра
bool LEnc_MonstersLoginSelectModel(ref location, aref modelInfo, string group, string locator)
{
	float type = rand(100)*0.01;
	if(LEnc_MonstersLoginObject.type == "inca_temple") type = 1.0;
	float skeleton = stf(LEnc_MonstersLoginObject.skeleton);
	float monkey = stf(LEnc_MonstersLoginObject.monkey);
	float mummy = stf(LEnc_MonstersLoginObject.mummy);
	float vagabond = stf(LEnc_MonstersLoginObject.vagabond);
	//Определимся, что выбрали
	float p = stf(LEnc_MonstersLoginObject.bandit);
	if(p > 0.0)
	{
		type = type - p;
		if(type <= 0)
		{
			//Бандита
			LEnc_MonstersLoginObject.current = "bandit";
			modelInfo.ani = "man";
			modelInfo.model = "rabwhite1";
			switch(rand(10))
			{
			case 0:
				modelInfo.model = "rabwhite1";
				break;
			case 1:
				modelInfo.model = "rabwhite";
				break;
			case 2:
				modelInfo.model = "rabblack";
				break;
			case 3:
				modelInfo.model = "fatman";
				break;
			case 4:
				modelInfo.model = "rabblack_1";
				break;
			case 5:
				modelInfo.model = "mask";
				break;
			case 6:
				modelInfo.model = "mask_2";
				break;
			case 7:
				modelInfo.model = "mask_3";
				break;
			case 8:
				modelInfo.model = "mask_4";
				break;
			case 9:
				modelInfo.model = "mask_3";
				break;
			case 10:
				modelInfo.model = "mask_4";
				break;
			}
			return true;
		}
	}
	p = stf(LEnc_MonstersLoginObject.skeleton);
	if(p > 0.0)
	{
		type = type - p;
		if(type <= 0)
		{
			//Скелета
			LEnc_MonstersLoginObject.current = "skeleton";
			modelInfo.model = "skel" + (1 + rand(3));
			modelInfo.ani = "man";
			return true;
		}
	}
	p = stf(LEnc_MonstersLoginObject.monkey);
	if(p > 0.0)
	{		
		type = type - p;
		if(type <= 0)
		{
			//Скелета-обезьяну
			LEnc_MonstersLoginObject.current = "monkey";
			modelInfo.model = "Monkey";
			modelInfo.ani = "Monkey";
			return true;
		}
	}
	p = stf(LEnc_MonstersLoginObject.mummy);
	if(p > 0.0)
	{		
		type = type - p;
		if(type <= 0)
		{
			//Скелета-обезьяну
			LEnc_MonstersLoginObject.current = "mummy";
			modelInfo.model = "mummy";
			modelInfo.ani = "man";
			return true;
		}
	}
	p = stf(LEnc_MonstersLoginObject.vagabond);
	if(p > 0.0)
	{
		type = type - p;
		if(type <= 0)
		{
			//Олуха
			if(rand(100) > 20) return false;
			LEnc_MonstersLoginObject.current = "vagabond";
			if(rand(100) < 70)
			{
				//Man
				modelInfo.ani = "man";
				LEnc_MonstersLoginObject.current.sex = "man";
				switch(rand(16))
				{
				case 0:
					modelInfo.model = "chameleon";
					break;
				case 1:
					modelInfo.model = "chelovek";
					break;
				case 2:
					modelInfo.model = "chelovek1";
					break;
				case 3:
					modelInfo.model = "brodyaga";
					break;
				case 4:
					modelInfo.model = "fisherman";
					break;
				case 5:
					modelInfo.model = "fisherman1";
					break;
				case 6:
					modelInfo.model = "man4";
					break;
				case 7:
					modelInfo.model = "man5";
					break;
				case 8:
					modelInfo.model = "man5_1";
					break;
				case 9:
					modelInfo.model = "pirat2";
					break;
				case 10:
					modelInfo.model = "pirat7";
					break;
				case 11:
					modelInfo.model = "pirat8";
					break;
				case 12:
					modelInfo.model = "old_man2";
					break;
				case 13:
					modelInfo.model = "sailor1";
					break;
				case 14:
					modelInfo.model = "sailor2";
					break;
				case 15:
					modelInfo.model = "sailor5";
					break;
				case 16:
					modelInfo.model = "sailor6";
					break;
				}
			}else{
				//Woman
				modelInfo.ani = "woman";
				LEnc_MonstersLoginObject.current.sex = "woman";
				switch(rand(8))
				{
				case 0:
					modelInfo.model = "towngirl1";
					break;
				case 1:
					modelInfo.model = "towngirl1_1";
					break;
				case 2:
					modelInfo.model = "towngirl2";
					break;
				case 3:
					modelInfo.model = "towngirl2_1";
					break;
				case 4:
					modelInfo.model = "towngirl3";
					break;
				case 5:
					modelInfo.model = "towngirl3_1";
					break;
				case 6:
					modelInfo.model = "towngirl5";
					break;
				case 7:
					modelInfo.model = "towngirl6";
					break;
				case 8:
					modelInfo.model = "towngirl7";
					break;
				}
			}
			return true;
		}
	}
	//Никого
	return false;
}

//Скорректировать параметры созданного монстра
void LEnc_MonstersLoginCorrectParams(ref location, aref chr, string group, string locator)
{
	bool isSet = false;
	ref mchr = GetMainCharacter();
	int mchr_rank = sti(mchr.rank);
	switch(LEnc_MonstersLoginObject.current)
	{
	case "bandit":
		isSet = true;
		LAi_NPC_Equip(chr, mchr_rank - 3, true, true);
		break;
	case "skeleton":
		isSet = true;
		chr.sex = "skeleton";
		LAi_NPC_Equip(chr, mchr_rank - 2, true, false);
		break;
	case "monkey":
		isSet = true;
		chr.sex = "monkey";
		LAi_type_monkey_Init(chr);
		RemoveCharacterEquip(chr, BLADE_ITEM_TYPE);
		LAi_NPC_Equip(chr, mchr_rank + 3, true, false);
		break;
	case "mummy":
		isSet = true;
		chr.sex = "skeleton";
		LAi_NPC_Equip(chr, mchr_rank, true, false);
		break;
	case "vagabond":
		isSet = true;
		LAi_CharacterReincarnation(chr, false, true);
		LAi_group_MoveCharacter(chr, LAI_DEFAULT_GROUP);
		LAi_type_citizen_Init(chr);
		chr.sex = LEnc_MonstersLoginObject.current.sex;
		chr.dialog.CurrentNode = "First time";
		chr.dialog.TempNode = chr.Dialog.CurrentNode;
		chr.dialog.filename = "Enc_Walker.c";
		RemoveCharacterEquip(chr, BLADE_ITEM_TYPE);
		SetRandomNameToCharacter(chr);
		break;
	}
	if(LEnc_MonstersLoginObject.type == "inca_temple")
	{
		float maxhp = LAi_GetCharacterMaxHP(chr);
		maxhp = maxhp*0.5;
		LAi_SetHP(chr, maxhp, maxhp);
	}
}

//Завершить загрузку монстров в локацию
void LEnc_MonstersLoginEnd(ref location)
{

}


