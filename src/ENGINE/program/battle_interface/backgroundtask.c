/////  boal этот файлик рудиент от свободного плавания кэпов. Нужно реанимить
int g_nCompanionTaskQuantity = 0;
object g_objCompanionTask[4];
int g_nTaskCheckRetResult;

#event_handler( "evCompanionTaskSuccess", "procCompanionTaskSuccess" );
#event_handler( "evCompanionTaskFault", "procCompanionTaskFault" );
#event_handler( "evCheckBattleResult", "procCheckCompanionBattleResult" );

//========================================
// nTaskType : 1 - блокирование колонии
// nTaskType : 2 - охота на корабли
//========================================
void BackgroundTask_SetTask( int char_index, int task_period, int difficult, int nTaskType, string sColony )
{
	if( g_nCompanionTaskQuantity >= 4 )
	{
		trace("warning! can`t start companion task. so mach task quantity.");
		return;
	}

	CreateEntity( &g_objCompanionTask[g_nCompanionTaskQuantity], "BackgroundShipTask" );
	g_objCompanionTask[g_nCompanionTaskQuantity].character = char_index;
	g_objCompanionTask[g_nCompanionTaskQuantity].period = task_period;
	g_objCompanionTask[g_nCompanionTaskQuantity].difficult = difficult;
	g_objCompanionTask[g_nCompanionTaskQuantity].nTaskType = nTaskType;
	g_objCompanionTask[g_nCompanionTaskQuantity].colony = sColony;

	aref aData;
	makearef( aData, Environment.date );
	SendMessage( &g_objCompanionTask[g_nCompanionTaskQuantity], "llla", 0, char_index, task_period, aData );

	g_nCompanionTaskQuantity++;
}

void LoadCompanionTasks()
{
	int n;
	aref aData;
	makearef( aData, Environment.date );

	for( n=0; n<g_nTaskCheckRetResult; n++ )
	{
		CreateEntity( &g_objCompanionTask[n], "BackgroundShipTask" );
		SendMessage( &g_objCompanionTask[n], "la", 1, aData );
	}
}

void procCompanionTaskSuccess()
{
	int nChrIndex = GetEventData();
	RemoveCompanionBackTask( nChrIndex );
}

void procCompanionTaskFault()
{
	int nChrIndex = GetEventData();
	RemoveCompanionBackTask( nChrIndex );
}

ref procCheckCompanionBattleResult()
{
	g_nTaskCheckRetResult = false;

	int nChrIndex = GetEventData();
	ref chref = &characters[nChrIndex];
	int nTask = FindCompanionBackTask( nChrIndex );
	if( nTask < 0 ) // нет такой задачи
		return &g_nTaskCheckRetResult;
	int nDifficult = sti( g_objCompanionTask[nTask] );

	int nDice = rand(100);
	int nCheck = 50; // стандартная проверка на исход битвы: 50/50
	//============================================================
	// модификаторы успеха/поражения
	//============================================================
		// сложность
	if( nDifficult == 1 )
	{
		nCheck = nCheck * 60 / 100; // 60% от нормальной
	}
	if( nDifficult == 3 )
	{
		nCheck = nCheck * 120 / 100; // 120% от нормальной
	}
		// удача капитана
	nDice = nDice + sti( chref.skill.Sneak ) * 2;
		// умение капитана
	nDice = nDice + BackgroundTask_SkillModificator( sti( chref.skill.Leadership ), 5 );
	nDice = nDice + BackgroundTask_SkillModificator( sti( chref.skill.Sailing ), 3 );
	nDice = nDice + BackgroundTask_SkillModificator( sti( chref.skill.Defence ), 4 );
	nDice = nDice + BackgroundTask_SkillModificator( sti( chref.skill.Grappling ), 1 );
	nDice = nDice + BackgroundTask_SkillModificator( sti( chref.skill.Accuracy ), 3 );
	nDice = nDice + BackgroundTask_SkillModificator( sti( chref.skill.Cannons ), 2 );

	//============================================================
	// собственно проверка
	//============================================================
	if( nDice >= nCheck ) // проверка прошла успешно
	{
		g_nTaskCheckRetResult = true;
		if( sti(g_objCompanionTask[nTask].nTaskType) == 1 ) // блокирование колоний
			BackgroundTaskMakeColonyWeaken( nChrIndex, nDifficult );
		else // охота
			BackgroundTaskAddLoot( nChrIndex, nDifficult );
		BackgroundTaskAddDamage( nChrIndex, nDifficult );
		BackgroundTask_CheckReturn( nChrIndex, nTask );
	}
	else
	{
		g_nTaskCheckRetResult = false;
		// попытка убежать (для сложности 1)
		if( nDifficult == 1 )
		{
			if( rand(1) == 0 ) // убежали
			{
				g_nTaskCheckRetResult = true;
				BackgroundTaskAddDamage( nChrIndex, nDifficult );
				BackgroundTask_CheckReturn( nChrIndex, nTask );
			}
		}
	}

	return &g_nTaskCheckRetResult;
}

void BackgroundTask_CheckReturn( int nChrIndex, int nTaskIndex )
{
	bool bReturnStatus = false;

	// check HP
	int nCurShipHP = GetCurrentShipHP( &Characters[nChrIndex] );
	int nMaxShipHP = GetCharacterShipHP( &Characters[nChrIndex] );
	int nDamageProcent = nCurShipHP * 100 / nMaxShipHP;
	if( nDamageProcent < 30 )
		bReturnStatus = true;

	// check shiphold
	int nCargoFreeSpace = GetCargoFreeSpace( &Characters[nChrIndex] );
	if( nCargoFreeSpace < 3 )
		bReturnStatus = true;

	if( bReturnStatus )
	{
		SendMessage( &g_objCompanionTask[nTaskIndex], "l", 2 );
	}
}

int BackgroundTask_SkillModificator( int nSkillValue, int nSkillInfluence )
{
	int nRetVal = 0;

	if( nSkillValue < 5 )
	{ // отрицательное влияние (уменьшает бросок)
		nRetVal = nSkillInfluence * (nSkillValue - 5) / 4;
	}

	if( nSkillValue > 5 )
	{ // положительное влияние (увеличивает бросок)
		nRetVal = nSkillInfluence * (nSkillValue - 5) / 5;
	}

	return nRetVal;
}

void BackgroundTaskAddLoot( int nCharIndex, int nDifficult )
{
	int n, nMax, nCur, nQnt;
	ref chref = &Characters[nCharIndex];

	int nFreeSpace = GetCargoFreeSpace( chref );
	for( n=0; n<GOODS_QUANTITY; n++ )
	{
		nMax = GetGoodQuantityByWeight( n, nFreeSpace );
		nCur = rand( nMax );
		if( nDifficult == 2 )
		{ // половина товара
			nCur = nCur / 2;
		}
		if( nDifficult == 1 )
		{ // четверть товара
			nCur = nCur / 4;
		}
		nQnt = GetCargoGoods( chref, n ) + nCur;
		SetCharacterGoods( chref, n, nQnt );
		nFreeSpace = GetCargoFreeSpace( chref );
	}
}

void BackgroundTaskMakeColonyWeaken( int nChrIndex, int nDifficult )
{
}

void BackgroundTaskAddDamage( int nCharIndex, int nDifficult )
{
	int nMaxDamageProcent = 0;
	switch( nDifficult )
	{
	case 1: nMaxDamageProcent=50; break;
	case 2: nMaxDamageProcent=75; break;
	case 3: nMaxDamageProcent=90; break;
	}
	// максимальное повреждение не больше имеющегося HP
	int nHP = GetCurrentShipHP( &Characters[nCharIndex] );
	nHP = nHP - nHP * nMaxDamageProcent / 100;
	Characters[nCharIndex].ship.hp = nHP;
}

int FindCompanionBackTask( int nChrIndex )
{
	int n;

	for( n=0; n<g_nCompanionTaskQuantity; n++ )
	{
		if( nChrIndex == sti(g_objCompanionTask[n].character) )
		{
			break;
		}
	}

	if( n < g_nCompanionTaskQuantity )
	{
		return n;
	}

	return -1;
}

void RemoveCompanionBackTask( int nChrIndex )
{
	if( nChrIndex < 0 ) return;
	int i;
	for( i=0; i<g_nCompanionTaskQuantity; i++ )
	{
		if( nChrIndex == sti(g_objCompanionTask[i].character) )
		{
			break;
		}
	}

	if( i < g_nCompanionTaskQuantity )
	{
		DeleteClass( &g_objCompanionTask[i] );
		g_nCompanionTaskQuantity--;
		while( i < g_nCompanionTaskQuantity )
		{
			g_objCompanionTask[i] = g_objCompanionTask[i+1];
			CopyAttributes( &g_objCompanionTask[i], &g_objCompanionTask[i+1] );
		}
	}
}
