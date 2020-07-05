
void Sea_Speak(aref chr, int whospeak, int isTaxAsk)
{
	//whospeak = 0 - pchar
	//whospeak = 1 - target
	
	
	string starget = chr.id;
	
	chr.dialogtype = "";
	
	string sMode = "";

	if (whospeak != sti(pchar.index))
	{
		if (!bIslandLoaded)
		{
			if(isTaxAsk == -1)
			{
				if (!CheckAttribute(&characters[GetCharacterIndex(sTarget)], "speak.ransom"))
				{
					if (CalculateForCapture(chr))
					{
						sMode = "capture";
					}
					else
					{
						if (CalculateForRansom(chr))
						{
							sMode = "ransom";
						}
					}
				}
			}
			else
			{
				if(!CheckAttribute(&characters[GetCharacterIndex(sTarget)], "speak.tax"))
				{
					sMode = "tax";
				}
			}
		}
	}
	//if(sMode != "")
	//{
		LaunchSpeakInterface(chr, whospeak, sMode);
	//}
} 

void DeleteRansomCharacters()
{
	for(int i=800; i<1000; i++)
	{
		if(checkAttribute(&characters[i], "ransom"))
		{
			DeleteAttribute(&characters[i], "ransom");
		}
	}
}

void AnalizeSituations(aref rSpeakCharacter)
{
	string sGroup = "Analize_Group_" + rSpeakCharacter.id;
	if (checkAttribute(rSpeakCharacter, "speak.ransom.taked"))
	{		
		Group_CreateGroup(sGroup);
		
		Group_SetGroupCommander(sGroup, rSpeakCharacter.id);
		
		Group_ChangeCharacter(sGroup, rSpeakCharacter.id);
		ChangeCharacterShipGroup(rSpeakCharacter, sGroup);
		
		Group_SetTaskRunaway(sGroup, PLAYER_GROUP);
		
		DeleteAttribute(rSpeakCharacter, "relation.UseOtherCharacter");

		SetCharacterRelationBoth(sti(rSpeakCharacter.index),nMainCharacterIndex,RELATION_NEUTRAL);
		UpdateRelations();
		RefreshBattleInterface();
	}
}

void AnalizeSituationsTAX(ref rSpeakCharacter)
{
	if(checkAttribute(rSpeakCharacter, "speak.tax"))
	{
		//Ship_SetTaskRunaway(PRIMARY_TASK, sti(rSpeakCharacter.index), nMainCharacterIndex);
		
		if (checkAttribute(rSpeakCharacter, "SeaAI.Group.Name"))
		{
			string sGroup = rSpeakCharacter.SeaAI.Group.Name;
			Group_SetNeutralToCharacter(sGroup, nMainCharacterIndex);
		
			//SetCharacterRelationBoth(sti(rCommander.index),nMainCharacterIndex,RELATION_NEUTRAL);
			UpdateRelations();
			RefreshBattleInterface();
		}
	}
}

void AnalizeSituationsEnemy()
{
	string sTarget = pchar.speakchr;
	Ship_SetTaskAttack(PRIMARY_TASK, getCharacterIndex(sTarget), nMainCharacterIndex);
	SetCharacterRelationBoth(GetCharacterIndex(sTarget),nMainCharacterIndex,RELATION_ENEMY);
	UpdateRelations();
	RefreshBattleInterface();
}

bool CheckForSpeak(int iCharacterIndex)
{
	ref chr = &Characters[iCharacterIndex];

	if (!CheckAttribute(chr, "speak.ransom"))
	{		
		if (Getrelation(iCharacterIndex, nMainCharacterIndex) == RELATION_ENEMY)
		{
			float fDistance = Ship_GetDistance2D(chr, pchar); 
			if (fDistance <= MIN_ENEMY_DISTANCE_TO_DISABLE_MAP_ENTER)
			{
				return true;
			}
		}
	}
	return false;
}

bool CalculateForRansom(ref rEnemyChar)
{
	if(CheckAttribute(rEnemyChar, "cannotsurrender"))
	{
		return false;
	}

	string sResult = "";
	
	int iSumm = 0;
	
	int iFlag = 0;
	
	int iMark = 0;

	int iEnemyLeadership = sti(rEnemyChar.skill.leadership);
	int iOurLeadership = GetSummonSkillFromName(pchar, SKILL_LEADERSHIP);

	ref refTargetShip = GetRealShip(sti(rEnemyChar.ship.type));	

	
	if (sti(rEnemyChar.ship.hp) * 2 <= sti(pchar.ship.hp))
	{
		iMark++;
	}
	if (sti(rEnemyChar.ship.hp) * 3 <= sti(refTargetShip.hp))
	{
		iMark++;
	}
	if (iEnemyLeadership <= iOurLeadership)
	{
		iMark++;
	}
	if (sti(rEnemyChar.ship.crew.quantity) * 2 <= sti(pchar.ship.crew.quantity))
	{
		iMark++;
	}
	if (sti(rEnemyChar.ship.crew.morale) * 2 <= sti(pchar.ship.crew.morale))
	{
		iMark++;
	}
	if (sti(rEnemyChar.ship.crew.experience) <= sti(pchar.ship.crew.experience))
	{
		iMark++;
	}
	if(sti(pchar.fame) > rand(65))
	{
		iMark++;
	}
	if(CheckCharacterPerk(pchar, "HorrorOfSeas"))
	{
		iMark++;
	}

	//return true;
	
	if (iMark >= 4)
	{
		return true;
	}
	
	//выкупаемся если
	//1. Игрок гораздо сильнее (hp больше в 2 раза) - 1 балл
	//2. Корпус поврежден (в 3 раза) - 1 балл
	//3. Вражеский Лидершип меньше нашего - 1 балл
	//4. Количество команды врага меньше более чем в 2 раза чем у нас - 1 балл	
	//5. Мораль команды врага меньше более чем в 2 раза чем у нас - 1 балл	
	//6. Опыт команды врага меньше  чем у нас - 1 балл	
	//5. Известность игрока больше чем рэндомное значение от 0 до 65 - 1 балл
	//6. Если корабль имеет флаг не сдаваться (для квестовых кораблей), то выкуп не работает
	
	return false;
}

bool CalculateForCapture(aref rSpeakCharacter)
{
	if(CheckAttribute(rSpeakCharacter, "cannotsurrender"))
	{
		return false;
	}
	string sResult = "";
	
	int iSumm = 0;
	
	int iFlag = 0;
	
	ref refTargetShip = GetRealShip(sti(rSpeakCharacter.ship.type));	
	
	int iEnemyLeadership = sti(rSpeakCharacter.skill.leadership) 
	int iOurLeadership = GetSummonSkillFromName(pchar, SKILL_LEADERSHIP);
	
	int iMark = 0;
	
	if (sti(rSpeakCharacter.ship.hp) * 4 <= sti(pchar.ship.hp))
	{
		iMark++;
	}
	if (sti(rSpeakCharacter.ship.hp) * 6 <= sti(refTargetShip.hp))
	{
		iMark++;
	}
	if (iEnemyLeadership <= iOurLeadership)
	{
		iMark++;
	}
	if (sti(rSpeakCharacter.ship.crew.morale) * 2 <= sti(pchar.ship.crew.morale))
	{
		iMark++;
	}
	if (sti(rSpeakCharacter.ship.crew.quantity) * 3 <= sti(pchar.ship.crew.quantity))
	{
		iMark++;
	}
	if (sti(rSpeakCharacter.ship.crew.experience) <= sti(pchar.ship.crew.experience))
	{
		iMark++;
	}
	if(CheckCharacterPerk(pchar, "HorrorOfSeas"))
	{
		iMark++;
	}
	if(sti(pchar.fame) > rand(85))
	{
		iMark++;
	}
	
	if (iMark > 4)
	{
		return true;
	}
	
	//выкупаемся если
	//1. Игрок гораздо сильнее (hp больше в 4 раза) - 1 балл
	//2. Корпус поврежден (в 6 раз) - 1 балл
	//3. Вражеский Лидершип меньше нашего - 1 балл
	//4. Количество команды врага меньше более чем в 3 раза чем у нас - 1 балл	
	//5. Мораль команды врага меньше более чем в 2 раза чем у нас - 1 балл	
	//6. Опыт команды врага меньше  чем у нас - 1 балл	
	//5. Известность игрока больше чем рэндомное значение от 0 до 85 - 1 балл
	//6. Если корабль имеет флаг не сдаваться (для квестовых кораблей), то выкуп не работает
	
	return false;
}

int CalculateSumm(aref chr)
{
	int iPrice = chr.money * 0.2 * GetSummonSkillFromName(pchar, SKILL_COMMERCE) / 2);
	/*
	int iTradeType;
	
	int iPrice = 0;
	
	for (int i = GOOD_WHEAT; i<GOODS_QUANTITY; i++)
	{
		iTradeType = GetCargoGoods(chr, i);
		iPrice = iPrice + sti(Goods[i].cost)*(0.5 + GetSummonSkillFromName(Pchar, SKILL_COMMERCE)/5);
	}*/
	
	return iPrice;
}
