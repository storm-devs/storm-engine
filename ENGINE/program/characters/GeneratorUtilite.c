// BOAL методы для генератора НПС

// здесь можно объявить глобальные переменные

int m_rank_bonus, e_rank_bonus;



// генерим НПС приблизительного ранга
void SetFantomParamFromRank(ref NPchar, int  rank, bool setEquip)
{
    SetRandSPECIAL(Npchar);
    CalculateSkillsFromRank(Npchar, rank);
    SetFantomHP(NPchar);
    if (setEquip)
    {
	    LAi_NPC_Equip(NPchar, sti(NPchar.rank), true, true);
    }
}


// расчитать скилы заданного ранга, типа как от ГГ в будущем (ранг у НПС будет приблизительно, зависит от сложности)
void CalculateSkillsFromRank(ref NPchar, int  rank)
{
    float  iMiddleK;
    
	// вычислим средний скил к этому рангу
	iMiddleK = 6 + rank*GetCharacterRankRate(GetMainCharacter()) / 14.0;
	
	CalculateAppropriateSkillsParam(Npchar, iMiddleK, 5);
}
// расчет скилов от среднего скила
void CalculateAppropriateSkillsParam(ref NPchar, float  MiddleK, int _complex)
{
    //trace("CalculateAppropriateSkillsParam " + NPchar.id);
    float  fValue;
    
    fValue = (20 * _complex / 4.0);
    Npchar.skill.FencingLight = makeint(MiddleK + (frandSmall(fValue) + frandSmall((20* _complex / 3.2)) - 24));
    Npchar.skill.Fencing      = makeint(MiddleK + (frandSmall(fValue) + frandSmall((20* _complex / 3.0)) - 20));
    Npchar.skill.FencingHeavy = makeint(MiddleK + (frandSmall(fValue) + frandSmall((20* _complex / 3.5)) - 28));
    Npchar.skill.Pistol       = makeint(MiddleK + (frandSmall(fValue) + frandSmall(fValue) - 30));
    Npchar.skill.Fortune      = makeint(MiddleK + (frandSmall(fValue) + frandSmall(fValue) - 30));

    Npchar.skill.Leadership   = makeint(MiddleK + (frandSmall(fValue) + frandSmall(fValue) - 30));
    Npchar.skill.Commerce     = makeint(MiddleK + (frandSmall(fValue) + frandSmall(fValue) - 30));
    Npchar.skill.Sailing      = makeint(MiddleK + (frandSmall(fValue) + frandSmall(fValue) - 30));
    Npchar.skill.Accuracy     = makeint(MiddleK + (frandSmall(fValue) + frandSmall(fValue) - 30));
    Npchar.skill.Cannons      = makeint(MiddleK + (frandSmall(fValue) + frandSmall(fValue) - 30));
    Npchar.skill.Grappling    = makeint(MiddleK + (frandSmall(fValue) + frandSmall(fValue) - 30));
    Npchar.skill.Repair       = makeint(MiddleK + (frandSmall(fValue) + frandSmall(fValue) - 30));
    Npchar.skill.Defence      = makeint(MiddleK + (frandSmall(fValue) + frandSmall(fValue) - 30));
    Npchar.skill.Sneak        = makeint(MiddleK + (frandSmall(fValue) + frandSmall(fValue) - 30));

    CorrectSkillParam(Npchar);

    SetRankFromSkill(Npchar);
}
void CalculateAppropriateSkills(ref NPchar)
{
    float   MiddleK;
    bool    ok;
    int     rank, i, j, k, sum, rank_bonus, base_rank;
    string  sTemp;
    
    SetRandSPECIAL(Npchar);
    if (false)
    {
    	MiddleK = GetMiddleMainSkill();

        CalculateAppropriateSkillsParam(Npchar, MiddleK, MOD_SKILL_ENEMY_RATE);
    }
    else
    // setting NPC skills based on main char rank and MOD_SKILL_ENEMY_RATE (Gray 14.12.2004)
    {
        rank_bonus = 0;
        sTemp = "";
		if (CheckAttribute(NPchar, "RankBonus"))
		{
			rank_bonus = sti(NPchar.RankBonus);
		    sTemp = "RankBonus: " + rank_bonus;
		}

		base_rank = 0;
		if (CheckAttribute(NPchar, "BaseRank"))
		{       
			base_rank = sti(NPchar.BaseRank);
			sTemp += " BaseRank: " + base_rank;
		}

		if (sti(Pchar.rank) > base_rank) base_rank = sti(Pchar.rank);
		
		MiddleK = base_rank;
		
        ok =  (MOD_SKILL_ENEMY_RATE > 5) && (bNewCodeOn);
        if (!ok)
        {
        	ok =  (MOD_SKILL_ENEMY_RATE <= 5) && (!bNewCodeOn); // наоборот - есть - сложнее станет
        }
        ok =  (!CheckAttribute(NPchar, "OurMan")) || (ok);

  		if (!CheckAttribute(NPchar, "BaseRank") && ok)// токо Компы и не двойной учет для моря
  		{
            MiddleK = MiddleK + (MOD_SKILL_ENEMY_RATE-5) * 1.5;
            sTemp += " ComplexBonus = " + FloatToString((MOD_SKILL_ENEMY_RATE-5) * 1.5, 1);
  		}
  		rank = MakeInt(MiddleK + frandSmall(4) + frandSmall(4) + frandSmall(4) + frandSmall(4) - 8 + rank_bonus);

		if (rank < 1) rank = 1;
		if (rank > 300) rank = 300;

    	NPchar.rank = rank;
    	if (CheckAttribute(NPchar, "OurMan"))
    	{
            sTemp += " OurNPCRank: " + rank;
    	}
    	else
    	{
    	   sTemp += " NPCRank: " + rank;
        }
        //debug
        if (CheckAttribute(NPchar, "RankBonus") || CheckAttribute(NPchar, "BaseRank") || CheckAttribute(NPchar, "OurMan"))
        {
    	   //Log_TestInfo("Fantom_gen: " + sTemp);
    	}
    	
    	if (rank > (1400 / GetCharacterRankRate(Npchar)))
		{
            SetSelfSkill(Npchar, SKILL_MAX, SKILL_MAX, SKILL_MAX, SKILL_MAX, SKILL_MAX);
            SetShipSkill(Npchar, SKILL_MAX, SKILL_MAX, SKILL_MAX, SKILL_MAX, SKILL_MAX, SKILL_MAX, SKILL_MAX, SKILL_MAX, SKILL_MAX);
			return;
		}
        CalculateSkillsFromRank(Npchar, rank);
    }
}

void SetRankFromSkill(ref Npchar)
{
    Npchar.rank = 1 + makeint( (sti(Npchar.skill.FencingLight) + sti(Npchar.skill.FencingHeavy) + sti(Npchar.skill.Fortune) +
                           sti(Npchar.skill.Pistol) + sti(Npchar.skill.Leadership) + sti(Npchar.skill.Fencing) +
                           sti(Npchar.skill.Sailing) + sti(Npchar.skill.Accuracy) + sti(Npchar.skill.Cannons) +
                           sti(Npchar.skill.Grappling) + sti(Npchar.skill.Repair) + sti(Npchar.skill.Defence) +
                           sti(Npchar.skill.Commerce) + sti(Npchar.skill.Sneak) - 84) / GetCharacterRankRate(Npchar) );
    if (sti(Npchar.rank) < 1)
    {
        Npchar.rank = 1;
    }
}

void CorrectSkillParam(ref Npchar)
{
    int i;
    string  skillName;

    for (i=1; i<15; i++)
    {
        skillName = GetSkillNameByIdx(i);
        if(sti(Npchar.skill.(skillName)) > SKILL_MAX) Npchar.skill.(skillName) = SKILL_MAX;
        if(sti(Npchar.skill.(skillName)) < 1)         Npchar.skill.(skillName) = 1;
    }
}
float GetMiddleMainSkill()
{
    float MiddleK;

    MiddleK = makefloat(GetSkillSum(Pchar) / 14.0) ;

    return  MiddleK;
}
int GetSkillSum(ref Npchar)
{
    int i, sum;
    string  skillName;
    
	sum = 0;
    for (i=1; i<15; i++)
    {
        skillName = GetSkillNameByIdx(i);
        sum += sti(Npchar.skill.(skillName));
    }
    return sum;
}

int GetSPECIALSum(ref Npchar)
{
    int i, sum;
    string  skillName;

	sum = 0;
    for (i=15; i<22; i++)
    {
        skillName = GetSkillNameByIdx(i);
        sum += sti(Npchar.SPECIAL.(skillName));
    }
    return sum;
}
// boal new formula 10.12.03 // очень важная раздача скилов, зависит от сложности <--

void SetFantomParam(ref _pchar)
{
    SetFantomParamCommon(_pchar);
}

void SetFantomParamCommon(ref _pchar)
{
    CalculateAppropriateSkills(_pchar);
    SetFantomHP(_pchar);
    // тут трем накопивщиеся сабли и корабли 290704 BOAL -->
    DeleteAttribute(_pchar, "Items");
    // тут трем накопивщиеся сабли и корабли 290704 BOAL <--
    LAi_NPC_Equip(_pchar, sti(_pchar.rank), true, true);
    AntiCheat(_pchar);
}

void SetFantomParamEncout(ref _pchar)  // выдача сабель и НР отдельно
{
    CalculateAppropriateSkills(_pchar);
    SetFantomHP(_pchar);
    // тут трем накопивщиеся сабли и корабли 290704 BOAL -->
    DeleteAttribute(_pchar, "Items");
    DeleteAttribute(_pchar, "equip");
    DeleteAttribute(_pchar, "perks.list");
    //LAi_NPC_EquipPerk(_pchar, "fantom");
	//GiveItem2Character(_pchar, "unarmed");
	//EquipCharacterByItem(_pchar, "unarmed");
    // тут трем накопивщиеся сабли и корабли 290704 BOAL <--
    AntiCheat(_pchar);
}

void SetSeaFantomParam(ref _pchar, string type)
{
	ref MChar;

	MChar = GetMainCharacter();
	if (CheckAttribute(MChar, "EnemyRank"))
	{
		Log_TestInfo("If you see this, feel free to kick Gray: " + MChar.EnemyRank);
		DeleteAttribute(MChar, "EnemyRank");
	}

	Fantom_SetRandomSkills(_pchar, type); // там вызов CalculateAppropriateSkills(_pchar); + бонус от класса корабля
	Fantom_SetRandomMoney(_pchar, type);
	Fantom_SetRandomCrewExp(_pchar, type);
	DeleteAttribute(_pchar, "items");

	SetFantomHP(_pchar);
	LAi_NPC_Equip(_pchar, sti(_pchar.rank), true, true);
	AntiCheat(_pchar);
}
/////////////////////////////////////////////////////
// из AIFantom.c
void Fantom_SetRandomSkills(ref rFantom, string sFantomType)
{
	aref aFSkills;

    makearef(aFSkills, rFantom.Skill);

	CalculateAppropriateSkills(rFantom);

	int iSClass = GetCharacterShipClass(rFantom);
    // boal 15.03.04 -->
    /*
			aFSkills.Commerce	= Fantom_CalcSkill(rFantom, SKILL_COMMERCE,	iSClass, 0, 1, 0, 0, 0, 0, 0, 0, 0);
			aFSkills.Grappling	= Fantom_CalcSkill(rFantom, SKILL_GRAPPLING, iSClass, 0, 1, 0, 0, 0, 0, 0, 0, 0);
			aFSkills.Fencing	= Fantom_CalcSkill(rFantom, SKILL_FENCING,	iSClass, 0, 1, 0, 0, 0, 0, 0, 0, 0);
			aFSkills.Repair		= Fantom_CalcSkill(rFantom, SKILL_REPAIR,	iSClass, 0, 1, 0, 0, 0, 0, 0, 0, 0);
			aFSkills.Sneak		= Fantom_CalcSkill(rFantom, SKILL_SNEAK,	    iSClass, 0, 1, 0, 0, 0, 0, 0, 0, 0);
			aFSkills.Pistol   	= Fantom_CalcSkill(rFantom, SKILL_PISTOL,	iSClass, 0, 1, 0, 0, 0, 0, 0, 0, 0);
    */
    // boal 15.03.04 <--
	switch (sFantomType)
	{
		case "trade":
		// комментируем лишеие - все равно ничего не меняют boal
            // умножение на 10 идет внутри метода
            aFSkills.Leadership = Fantom_CalcSkill(rFantom, SKILL_LEADERSHIP,iSClass, 0, 0, 0, 1, 1, 2, 2, 3, 4);
            aFSkills.Accuracy	= Fantom_CalcSkill(rFantom, SKILL_ACCURACY,	iSClass, 0, 0, 0, 1, 1, 2, 3, 4, 5);
			aFSkills.Cannons	= Fantom_CalcSkill(rFantom, SKILL_CANNONS,	iSClass, 0, 0, 0, 1, 1, 2, 3, 4, 5);
			aFSkills.Sailing	= Fantom_CalcSkill(rFantom, SKILL_SAILING,	iSClass, 0, 0, 0, 1, 1, 2, 3, 6, 7);
			aFSkills.Defence	= Fantom_CalcSkill(rFantom, SKILL_DEFENCE,	iSClass, 0, 0, 0, 1, 1, 2, 3, 4, 5);
		break;
		case "war":
            aFSkills.Leadership = Fantom_CalcSkill(rFantom, SKILL_LEADERSHIP,iSClass, 0, 0, 0, 1, 1, 2, 3, 4, 5);
            aFSkills.Accuracy	= Fantom_CalcSkill(rFantom, SKILL_ACCURACY,	iSClass, 0, 0, 0, 1, 2, 3, 4, 5, 6);
			aFSkills.Cannons	= Fantom_CalcSkill(rFantom, SKILL_CANNONS,	iSClass, 0, 0, 0, 1, 2, 3, 4, 7, 8);
			aFSkills.Sailing	= Fantom_CalcSkill(rFantom, SKILL_SAILING,	iSClass, 0, 0, 0, 1, 2, 3, 4, 6, 7);
			aFSkills.Defence	= Fantom_CalcSkill(rFantom, SKILL_DEFENCE,	iSClass, 0, 0, 0, 1, 2, 3, 4, 5, 6);
		break;
		case "pirate":
            aFSkills.Leadership = Fantom_CalcSkill(rFantom, SKILL_LEADERSHIP,iSClass, 0, 0, 0, 1, 1, 2, 3, 4, 4);
            aFSkills.Accuracy	= Fantom_CalcSkill(rFantom, SKILL_ACCURACY,	iSClass, 0, 0, 0, 2, 3, 4, 5, 6, 7);
			aFSkills.Cannons	= Fantom_CalcSkill(rFantom, SKILL_CANNONS,	iSClass, 0, 0, 0, 2, 3, 4, 5, 6, 7);
			aFSkills.Sailing	= Fantom_CalcSkill(rFantom, SKILL_SAILING,	iSClass, 0, 0, 0, 2, 3, 4, 5, 7, 8);
            aFSkills.Defence	= Fantom_CalcSkill(rFantom, SKILL_DEFENCE,	iSClass, 0, 0, 0, 1, 2, 3, 4, 5, 6);
		break;
	}
    //if (sti(rFantom.rank) < 50)
    //{
	SetRankFromSkill(rFantom);
	//}
}
/////////////////////////////////////////////////////
void SetFantomHP(ref _pchar)
{
	int hp;
	hp = GetCharacterBaseHPValue(_pchar) + (sti(_pchar.rank) * GetCharacterAddHPValue(_pchar));
	LAi_SetHP(_pchar, hp, hp);
	LAi_SetCurHPMax(_pchar);
}

void SetMonsterLoginHP(ref _pchar) // жизнь у монстров больше (рейдеры тоже тут), как и пассажиры - потенциально читовые офицеры!!
{
	int hp;
	if (true)
	{
		int rank = makeint(sti(_pchar.rank) * (1 + MOD_SKILL_ENEMY_RATE / 40.0));
		hp = LAI_DEFAULT_HP_MAX + rank * 5;
		_pchar.rank = rank;
		LAi_SetHP(_pchar, hp, hp);
		LAi_SetCurHPMax(_pchar);
	}
	else
	{
		hp = LAI_DEFAULT_HP_MAX + makeint(sti(_pchar.rank)*(MOD_SKILL_ENEMY_RATE / 2.0 + 5.0)) + 10;
		LAi_SetHP(_pchar, hp, hp);
		LAi_SetCurHPMax(_pchar);
	}
}

/// метод расчета от опыта солдат, дает бонус в НР (или пенальти)
bool GetBoardingHP(ref mchr, ref echr, ref float_boarding_player_hp, ref float_boarding_enemy_hp)
{
	float b_p_hp, b_e_hp;
	float moral;
	float exp;
	b_p_hp = LAi_GetCharacterMaxHP(mchr) / 3.0;  // треть от НР кэпа идет в базу бонуса
	
	exp = GetCrewExp(mchr, "Soldiers") / GetCrewExpRate() - 0.7;
	moral = 0;
	if(CheckAttribute(mchr, "ship.crew.morale"))
	{
		moral = (stf(mchr.ship.crew.morale) - MORALE_NORMAL)/(MORALE_MAX - MORALE_MIN);
		if(moral < -0.5) moral = -0.5;
		if(moral > 0.5) moral = 0.5;
	}
	exp = exp + moral;  // может быть минус
	b_p_hp = b_p_hp*exp; 
	
	b_e_hp = 0;  // не будем рандом городить рандомом, опыт и еще скилы кэпа, все это не зависит от ГГ, а вот ГГ бонус от опыта даем
	/*
	b_e_hp = LAi_GetCharacterMaxHP(echr) / 3.0;
	exp = GetCrewExp(echr, "Soldiers") / GetCrewExpRate();
	moral = 0;
	if(CheckAttribute(echr, "ship.crew.morale"))
	{
		moral = (stf(echr.ship.crew.morale) - MORALE_NORMAL)/(MORALE_MAX - MORALE_MIN);
		if(moral < -0.5) moral = -0.5;
		if(moral > 0.5) moral = 0.5;
	}
	exp = exp + moral;
	b_e_hp = b_e_hp*exp;
	 */
	float_boarding_player_hp   =  b_p_hp;
	float_boarding_enemy_hp    =  b_e_hp;
	
	return true;
}
void AddCharHP(ref _pchar, int _bonus)
{
	int hp = LAi_GetCharacterMaxHP(_pchar) + _bonus;
	if (hp < 1) hp = 1;
	LAi_SetHP(_pchar, hp, hp);
	LAi_SetCurHPMax(_pchar);
}

// в коде далее
// boarding_player_hp = boarding_player_hp + GetBoarding_player_hp_Bonus;
// boarding_enemy_hp = boarding_enemy_hp + GetBoarding_enemy_hp_Bonus;

int GetBoarding_player_hp_Bonus(int mcrew, int ecrew) // бонус от перевеса, когда if (mcrew > ecrew)
{
    m_rank_bonus = 0;
    e_rank_bonus = 0;
    if (true)
	{     // только этот код
		if (mcrew > ecrew)
		{
			m_rank_bonus = makeint(0.6 * MOD_SKILL_ENEMY_RATE * makefloat((mcrew - ecrew) / makefloat(ecrew)));
			Log_TestInfo("Бонус к уровню игрока за перевес в численности: " + m_rank_bonus);
		}
		return 0;
	}
	else
	{
		return makeint(5 * MOD_SKILL_ENEMY_RATE * makefloat((mcrew - ecrew) / makefloat(ecrew)));
	}
}

int GetBoarding_enemy_hp_Bonus(int mcrew, int ecrew) // бонус от перевеса
{
    m_rank_bonus = 0;
    e_rank_bonus = 0;
	if (mcrew < 1) mcrew = 1;
    if (true)
	{     // только этот код
		if (ecrew > mcrew)
		{
			e_rank_bonus = makeint(MOD_SKILL_ENEMY_RATE * makefloat((ecrew - mcrew) / makefloat(mcrew)));
			Log_TestInfo("Бонус к уровню врагов за перевес в численности: " + e_rank_bonus);
		}
		return 0;
	}
	else
	{
		return makeint(6 * MOD_SKILL_ENEMY_RATE * makefloat((ecrew - mcrew) / makefloat(mcrew)));
	}
}

float GetBoarding_player_hp(float b_p_hp) // итоговое здоровье нашего матроса
{
	return b_p_hp; // нафиг случайность иначе мрут сильно + 6*rand(11 - MOD_SKILL_ENEMY_RATE) - 20;
}

float GetBoarding_enemy_hp(float b_e_hp) // итоговое здоровье матроса врага
{
	return b_e_hp + 4*(MOD_SKILL_ENEMY_RATE - 5); // нафиг случайность + 6*rand(MOD_SKILL_ENEMY_RATE+1) - 20;
}
// наши на абордаде (далее применяется НР капитана)
void SetFantomParamAbordOur(ref _pchar)
{
	_pchar.RankBonus = m_rank_bonus;
	_pchar.OurMan = true;
	SetFantomParamCommon(_pchar);
	DeleteAttribute(_pchar, "RankBonus");
	DeleteAttribute(_pchar, "OurMan");
}
// наши в форте НР не меняем
void SetFantomParamFortOur(ref _pchar)
{
	_pchar.RankBonus = m_rank_bonus;
	_pchar.OurMan = true;
	SetFantomParamCommon(_pchar);
	DeleteAttribute(_pchar, "RankBonus");
	DeleteAttribute(_pchar, "OurMan");
}
// враги на абордаде (далее применяется НР капитана)
void SetFantomParamAbordEnemy(ref _pchar)
{
	ref MChar;

	MChar = GetMainCharacter();
	if (CheckAttribute(MChar, "EnemyRank"))
	{
		_pchar.BaseRank = MChar.EnemyRank;
	}
	_pchar.RankBonus = e_rank_bonus;

	SetFantomParamCommon(_pchar);
	DeleteAttribute(_pchar, "BaseRank");
	DeleteAttribute(_pchar, "RankBonus");
}
// враги в форте НР меняем потом на GetBoarding_enemy_hp(LAi_GetCharacterMaxHP(НПС))
void SetFantomParamFortEnemy(ref _pchar)
{
	ref MChar;

	MChar = GetMainCharacter();
	if (CheckAttribute(MChar, "EnemyRank"))
	{
		_pchar.BaseRank = MChar.EnemyRank;
	}
	_pchar.RankBonus = e_rank_bonus;

	SetFantomParamCommon(_pchar);
	DeleteAttribute(_pchar, "BaseRank");
	DeleteAttribute(_pchar, "RankBonus");
}

//крутые парни -->
void SetFantomParamHunter(ref sld)
{
    SetSeaFantomParam(sld, "war");
    LAi_SetHP(sld, LAI_DEFAULT_HP_MAX + sti(sld.chr_ai.hp), LAI_DEFAULT_HP_MAX + sti(sld.chr_ai.hp));
    TakeNItems(sld,"potion2", 2);
}
//крутые парни <--

// boal 20.03.2004 <--
void AntiCheat(ref _pchar)
{
    /*ref mc = GetMainCharacter();
    if (sti(mc.chr_ai.hp) > 310)
    {
        LAi_SetHP(_pchar, 10000, 10000);
        mc.chr_ai.hp = 310;
    }
    SetRankFromSkill(mc);*/
}

void CheckAntiCheat()
{
 	if (CheckAttribute(PChar, "SystemInfo.CRC"))
 	{
		if (stf(PChar.SystemInfo.CRC) != GetCRCCheatSum(PChar))
		{
		    bPlayerIsCheater = true;
		}
	}
}

void SetAntiCheat()
{
	PChar.SystemInfo.CRC = GetCRCCheatSum(PChar);
}
float GetCRCCheatSum(ref _PChar)
{
	return makefloat(GetSPECIALSum(_PChar) + GetSkillSum(_PChar) + sti(_PChar.Ship.Type) + stf(_PChar.Health.HP) + sti(_PChar.rank) + sti(_PChar.Money) + stf(_PChar.chr_ai.hp));
}

////////////////////////////////////////
//   SEA
////////////////////////////////////////
float Sea_TurnRateMagicNumber();
{
    return 244.444; //162.962; //244.444; *2/3
}

//#define WIND_NORMAL_POWER		18.0 // делитель для силы ветра на циферблате - влияет на мах скорость

float Sea_ApplyMaxSpeedZ(aref arCharShip, float fWindDotShip) //float fTRFromSailDamage,
// arCharShip - корабль на НПС,  fTRFromSailDamage - паруса 0..1, fWindDotShip - направление ветра -1..1
{
    ref		rShip = GetRealShip(sti(arCharShip.Type)); // база
    float   fMaxSpeedZ;
    float   fWindAgainstSpeed;
    //fMaxSpeedZ = (0.16 + fTRFromSailDamage / 1.2) * stf(arCharShip.MaxSpeedZ);
    fMaxSpeedZ = stf(arCharShip.MaxSpeedZ);
    fWindAgainstSpeed = stf(rShip.WindAgainstSpeed);// / 1.7; // мин fWindAgainstSpeed = 0.8 - мах 10.5
	if (fWindDotShip >= -0.1)
    { //по ветру
        fMaxSpeedZ = fMaxSpeedZ * (0.81 + fWindDotShip / (1.9 + pow(fWindAgainstSpeed, 0.33)));
    }
    else
    { //против ветра
        fMaxSpeedZ = fMaxSpeedZ * (0.75 - fWindDotShip/3.2 - pow(abs(fWindDotShip), fWindAgainstSpeed)); // тут есть влияние кода в ЕХЕ
    }

    return fMaxSpeedZ;
}