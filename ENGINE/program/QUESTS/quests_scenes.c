/*===================================================================**
		Типы исполняемых команд и их параметры:
	- "sass","gotopoint", <референс на персонажа>, <имя группы локаторов>, <имя локатора>
**===================================================================*/

#define SCENES_TIMEOUT		90.0
#define SCENES_DIALOGTIME	15.0

#event_handler("QuestSceneCommand","procQuestSceneCommand");
#event_handler("startAfterWaitScene","startAfterWaitScene");
#event_handler("qprocTaskEnd","qprocTaskEnd");

object objQuestScene;

void procQuestSceneCommand()
{
	string	command_name	= GetEventData();
	string	QuestCheckName	= GetEventData();
	aref	arCharacter		= GetEventData();
	string	s1,s2,s3;
	bool	b1;
	float	f1;
	int		i1;
	aref	arTask;
	switch(command_name)
	{
	// идти к локатору (задаем имя группы и имя локатора)
	case "gotopoint": // "ssass"
		s1 = GetEventData();
		s2 = GetEventData();
		arTask = PushSceneTask(arCharacter,"Goto point");
		arTask.group = s1;
		arTask.locator = s2;
		if(QuestCheckName!="") arTask.QuestCheck = QuestCheckName;
		return;
	break;
	// принять нормальную стойку в небоевом режиме (нет параметров)
	case "stay": // "ssa"
		arTask = PushSceneTask(arCharacter,"Stay");
		if(QuestCheckName!="") arTask.QuestCheck = QuestCheckName;
		return;
	break;
	// бежать к локатору (задаем имя группы и имя локатора)
	case "runtopoint": // "ssass"
		s1 = GetEventData();
		s2 = GetEventData();
		arTask = PushSceneTask(arCharacter,"Runto point");
		arTask.group = s1;
		arTask.locator = s2;
		if(QuestCheckName!="") arTask.QuestCheck = QuestCheckName;
		return;
	break;
	// подойти к персонажу (задаем идентификатор персонажа)
	case "followcharacter": // "ssas"
		s1 = GetEventData();
		arTask = PushSceneTask(arCharacter,"Follow character");
		arTask.charID = s1;
		if( GetCharacterIndex(s1)<0 ) trace("SCENE WARNING! Character wish id = <"+s1+"> NOT FOUND!");
		if(QuestCheckName!="") arTask.QuestCheck = QuestCheckName;
		return;
	break;
	// бить персонажа (задаем идентификатор персонажа)
	case "fight": // "ssas"
		s1 = GetEventData();
		arTask = PushSceneTask(arCharacter,"Fight");
		arTask.charID = s1;
		if( GetCharacterIndex(s1)<0 ) trace("SCENE WARNING! Character wish id = <"+s1+"> NOT FOUND!");
		if(QuestCheckName!="") arTask.QuestCheck = QuestCheckName;
		return;
	break;
	// спасаться от персонажа (задаем идентификатор персонажа)
	case "escape": // "ssas"
		s1 = GetEventData();
		arTask = PushSceneTask(arCharacter,"Escape");
		arTask.charID = s1;
		if( GetCharacterIndex(s1)<0 ) trace("SCENE WARNING! Character wish id = <"+s1+"> NOT FOUND!");
		if(QuestCheckName!="") arTask.QuestCheck = QuestCheckName;
		return;
	break;
	// умереть (без параметров), но персонаж не умирает в действительности и остается в данной локации
	case "dead": // "ssa"
		arTask = PushSceneTask(arCharacter,"Dead");
		if(QuestCheckName!="") arTask.QuestCheck = QuestCheckName;
		return;
	break;
	// повернуться к другому персонажу (задаем идентификатор персонажа)
	case "TurnByCharacter": // "ssas"
		s1 = GetEventData();
		arTask = PushSceneTask(arCharacter,"TurnByCharacter");
		arTask.charID = s1;
		if( GetCharacterIndex(s1)<0 ) trace("SCENE WARNING! Character wish id = <"+s1+"> NOT FOUND!");
		if(QuestCheckName!="") arTask.QuestCheck = QuestCheckName;
		return;
	break;
	// повернуться по локатору (задаем имена группы и локатора)
	case "TurnByLocator": // "ssass"
		s1 = GetEventData();
		s2 = GetEventData();
		arTask = PushSceneTask(arCharacter,"TurnByLocator");
		arTask.group = s1;
		arTask.locator = s2;
		if(QuestCheckName!="") arTask.QuestCheck = QuestCheckName;
		return;
	break;
	// поговорить с другим персонажем (задаем идентификатор персонажа)
	case "Dialog": // "ssas"
		s1 = GetEventData();
		arTask = PushSceneTask(arCharacter,"Dialog");
		arTask.charID = s1;
		if( GetCharacterIndex(s1)<0 ) trace("SCENE WARNING! Character wish id = <"+s1+"> NOT FOUND!");
		if(QuestCheckName!="") arTask.QuestCheck = QuestCheckName;
		return;
	break;
	// закончить квестовый ролик (без параметров), в смысле разблокировать управление и перегрузку
	case "EndQuestMovie": // "ssa"
		arTask = PushSceneTask(arCharacter,"EndQuestMovie");
		if(QuestCheckName!="") arTask.QuestCheck = QuestCheckName;
		return;
	break;
	// проиграть анимацию для персонажа (задаем имя анимации)
	case "action": // "ssas"
		s1 = GetEventData();
		arTask = PushSceneTask(arCharacter,"Action");
		arTask.action = s1;
		if(QuestCheckName!="") arTask.QuestCheck = QuestCheckName;
		return;
	break;
	// проигрывать анимацию для персонажа в цикле (задаем имя типа анимации: "stay" - стоять,
	// "fightStand" - стоять в режиме боя, "sit" - сидеть , либо имя анимации в остальных случаях )
	case "loopAction": // "ssas"
		s1 = GetEventData();
		arTask = PushSceneTask(arCharacter,"loopAction");
		arTask.action = s1;
		return;
	break;
	// Поместить в руку саблю (задаем флаг перереключения сабли: true - есть сабля, false - нет ее)
	case "BladeToHand": // "ssal"
		b1 = GetEventData();
		arTask = PushSceneTask(arCharacter,"BladeToHand");
		arTask.boolHand = b1;
		if(QuestCheckName!="") arTask.QuestCheck = QuestCheckName;
		return;
	break;
	// Исчезнуть и переместиться в другую локацию (задаем имя новой локации и локатора)
	case "Teleport": // "ssasss"
		s1 = GetEventData();
		s2 = GetEventData();
		s3 = GetEventData();
		arTask = PushSceneTask(arCharacter,"Teleport");
		arTask.location = s1;
		arTask.group = s2;
		arTask.locator = s3;
		if(QuestCheckName!="") arTask.QuestCheck = QuestCheckName;
		return;
	break;
	// перегрузиться в локацию (задаем имя локации, группу и имя локатора)
	case "ReloadToLocation": // "ssasss"
		s1 = GetEventData();
		s2 = GetEventData();
		s3 = GetEventData();
		arTask = PushSceneTask(arCharacter,"ReloadToLocation");
		arTask.location = s1;
		arTask.group = s2;
		arTask.locator = s3;
		if(QuestCheckName!="") arTask.QuestCheck = QuestCheckName;
		return;
	break;
	// подождать пока пройдет заданное число секунд (задаем число секунд, использовать число типа float)
	case "TimeWait": // "ssaf"
		f1 = GetEventData();
		arTask = PushSceneTask(arCharacter,"TimeWait");
		arTask.time = MakeInt(f1*1000.0);
		return;
	break;
	// сделать фэйд экрана и добавить игровое время на заданное число минут (задаем число игровых минут, число типа int)
	case "TimeFade": // "ssal"
		i1 = GetEventData();
		arTask = PushSceneTask(arCharacter,"TimeFade");
		arTask.time = i1;
		return;
	break;
	// Установили групповую атаку (задаем имя группы за которую борется персонаж)
	case "AttackGroup": // "ssas"
		s1 = GetEventData();
		arTask = PushSceneTask(arCharacter,"AttackGroup");
		arTask.group = s1;
		return;
	break;
	// проиграть музыку (задаем имя музыкального файла)
	case "PlayMusic": // "ssas"
		s1 = GetEventData();
		arTask = PushSceneTask(arCharacter,"PlayMusic");
		arTask.name = s1;
		return;
	break;
	// проиграть звук (задаем имя файла озвучки)
	case "PlayMusic": // "ssas"
		s1 = GetEventData();
		arTask = PushSceneTask(arCharacter,"PlaySound");
		arTask.name = s1;
		return;
	break;
	// включить исполнение последовательности задач для другого персонажа (задаем идентификатор персонажа)
	case "StartTaskForOtherChar": // "ssas"
		s1 = GetEventData();
		arTask = PushSceneTask(arCharacter,"StartTaskForOtherChar");
		arTask.charID = s1;
		if( GetCharacterIndex(s1)<0 ) trace("SCENE WARNING! Character wish id = <"+s1+"> NOT FOUND!");
		if(QuestCheckName!="") arTask.QuestCheck = QuestCheckName;
		return;
	break;
	// остановить исполнение потока задач для текущего персонажа
	case "StopTask": // "ssa"
		if( GetLastTask(arCharacter,&arTask) )	arTask.StopTask = true;
		return;
	break;
	}
	trace("SCENE WARNING! unknown task setting : <"+command_name+">");
}

void qprocTaskEnd()
{
	aref arch = GetEventData();

	aref taskRef;
	if( !GetFirstTask(arch,&taskRef) ) return;
	if( !CheckAttribute(taskRef,"Executing") || taskRef.Executing!="1" ) return;

	string locTask = GetAttributeValue(taskRef);

	if( locTask == "loopAction" )
	{
		PerformLoopAnimation(GetCharacter(sti(arch.index)),taskRef.action);
		return;
	}

	if( CheckAttribute(taskRef,"QuestCheck") ) ExecuteQuestCheck(taskRef.QuestCheck);

	if( CheckAttribute(taskRef,"StopTask") && sti(taskRef.StopTask)==true )
	{
		PopBegSceneTask(arch);
	}
	else
	{
		PopBegSceneTask(arch);
		StartSceneExecute(arch);
	}
}

void qprocEndReload()
{
	DelEventHandler(EVENT_LOCATION_LOAD,"qprocEndReload");
	Event("qprocTaskEnd","a",pchar);
}

string PopSceneTask(aref character)
{
	aref taskRef;
	if( !GetLastTask(character,&taskRef) ) return "";
	string taskName = GetAttributeValue(taskRef);
	DeleteAttribute(character,"SceneTask."+GetAttributeName(taskRef));
	return taskName;
}

string PopBegSceneTask(aref character)
{
	aref taskRef;
	if( !GetFirstTask(character,&taskRef) ) return "";
	string taskName = GetAttributeValue(taskRef);
	DeleteAttribute(character,"SceneTask."+GetAttributeName(taskRef));
	return taskName;
}

aref PushSceneTask(aref character,string TaskName)
{
	aref staref; makearef(staref,character.SceneTask);
	string attrName = "s"+GetAttributesNum(staref);
	character.SceneTask.(attrName) = TaskName;
	makearef(staref,character.SceneTask.(attrName));
	return staref;
}

bool GetLastTask(aref character, ref taskRef)
{
	aref staref; makearef(staref,character.SceneTask);
	int qn = GetAttributesNum(staref);
	if(qn==0) return false;
	taskRef = GetAttributeN(staref,qn-1);
	return true;
}

bool GetFirstTask(aref character, ref taskRef)
{
	aref staref; makearef(staref,character.SceneTask);
	int qn = GetAttributesNum(staref);
	if(qn==0) return false;
	taskRef = GetAttributeN(staref,0);
	return true;
}

void StartSceneExecute(aref character)
{
	aref scnref;
	if( !GetFirstTask(character,&scnref) )	return;

	if( CheckAttribute(scnref,"Executing") && scnref.Executing=="1" )
	{
		PopBegSceneTask(character);
		if( !GetFirstTask(character,&scnref) )	return;
	}

	int i;
	scnref.Executing = "1";

	// Персонаж становиться актером
	LAi_SetActorType(character);

	switch(GetAttributeValue(scnref))
	{
	case "Goto point":
		LAi_ActorGoToLocator(ARefChrToRef(character), scnref.group, scnref.locator, QuestNameForChr(character), SCENES_TIMEOUT );
		return;
	break;

	case "Stay":
		LAi_ActorStay(ARefChrToRef(character));
		PostEvent("qprocTaskEnd",1,"a",character); // !!!
		return;
	break;

	case "Runto point":
		LAi_ActorRunToLocator(ARefChrToRef(character), scnref.group, scnref.locator, QuestNameForChr(character), SCENES_TIMEOUT );
		return;
	break;

	case "Follow character":
		LAi_ActorFollow(ARefChrToRef(character), CharacterFromID(scnref.charID), QuestNameForChr(character), SCENES_TIMEOUT);
		return;
	break;

	case "Fight":
		LAi_ActorAttack(ARefChrToRef(character), CharacterFromID(scnref.charID), QuestNameForChr(character) );
		return;
	break;

	case "Escape":
		trace("WARNING!!!  Escape не реализована!");
		PostEvent("qprocTaskEnd",1,"a",character); // !!!
		return;
	break;

	case "Dead":
		trace("WARNING!!!  Dead не реализована!");
		PostEvent("qprocTaskEnd",1,"a",character); // !!!
		return;
	break;

	case "TurnByCharacter":
		LAi_ActorTurnToCharacter(ARefChrToRef(character), CharacterFromID(scnref.charID));
		PostEvent("qprocTaskEnd",1,"a",character); // !!!
		return;
	break;

	case "TurnByLocator":
		LAi_ActorTurnByLocator(ARefChrToRef(character), scnref.group, scnref.locator);
		PostEvent("qprocTaskEnd",1,"a",character); // !!!
		return;
	break;

	case "Dialog":
		i =  GetCharacterIndex(scnref.charID);
		if(i<0)
		{
			trace("SCENE WARNING! Error character id <"+scnref.charID+">  for task Dialog");
			PostEvent("qprocTaskEnd",1,"a",character); // !!!
			return;
		}
		else
		{
			LAi_ActorDialog(ARefChrToRef(character), GetCharacter(i), QuestNameForChr(character), SCENES_TIMEOUT+SCENES_DIALOGTIME, SCENES_DIALOGTIME);
		}
		return;
	break;

	case "EndQuestMovie":
		EndQuestMovie();
		PostEvent("qprocTaskEnd",1,"a",character); // !!!
		return;
	break;

	case "action":
		LAi_ActorAnimation(ARefChrToRef(character), scnref.action, QuestNameForChr(character), SCENES_TIMEOUT);
		return;
	break;

	case "loopAction":
		PerformLoopAnimation(ARefChrToRef(character),scnref.action);
		return;
	break;

	case "BladeToHand":
		trace("WARNING!!! BladeToHand функция не реализована!!!"); // !!!
		PostEvent("qprocTaskEnd",1,"a",character); // !!!
		return;
	break;

	case "Teleport":
		ChangeCharacterAddressGroup(ARefChrToRef(character), scnref.location,scnref.group,scnref.locator);
		PostEvent("qprocTaskEnd",1,"a",character);
		return;
	break;

	case "ReloadToLocation":
		DoReloadCharacterToLocation ( scnref.location, scnref.group, scnref.locator );
		SetEventHandler(EVENT_LOCATION_LOAD,"qprocEndReload",0);
		return;
	break;

	case "TimeWait":
		LAi_QuestDelay(QuestNameForChr(character), stf(scnref.time)*0.001);
		return;
	break;

	case "TimeFade":
		//WaitNightPause(false);
		AddTimeToCurrent(0,sti(scnref.time));
		PostEvent("qprocTaskEnd",1,"a",character); // !!!
		return;
	break;

	case "AttackGroup":
		trace( "WARNING!!! AttackGroup не реализована" ); // !!!
		PostEvent("qprocTaskEnd",1,"a",character); // !!!
		return;
	break;

	case "PlayMusic":
		PlayMusic(scnref.name);
		PostEvent("qprocTaskEnd",1,"a",character); // !!!
		return;
	break;

	case "PlaySound":
		PlayMusic(scnref.name);
		PostEvent("qprocTaskEnd",1,"a",character); // !!!
		return;
	break;

	case "StartTaskForOtherChar":
		StartSceneExecute ( CharacterFromID(scnref.charID) );
		Event( "qprocTaskEnd", "a", character );
		return;
	break;
	}
	trace("SCENE WARNING! unknown task starting : <"+GetAttributeValue(scnref)+">   TASK WILL DELETED!");
	Event( "qprocTaskEnd", "a", character );
}

void startAfterWaitScene()
{
	aref ar1 = GetEventData();
	StartSceneExecute(ar1);
}

void ExecuteQuestCheck(string questCheckName)
{
	CompleteQuestName(questCheckName, "");
	QuestsCheck();
}

bool PerformLoopAnimation(ref chr,string action)
{
	switch(action)
	{
	case "stay":	LAi_ActorAnimation(chr, GetStayAniName(), QuestNameForChr(chr), SCENES_TIMEOUT);	return;	break;
	case "fightStand":	LAi_ActorAnimation(chr, "fight stand_"+(rand(3)+1), QuestNameForChr(chr), SCENES_TIMEOUT);	return; break;
	case "sit":	LAi_ActorAnimation(chr, GetSitAniName(), QuestNameForChr(chr), SCENES_TIMEOUT);	return; break;
	}
	return LAi_ActorAnimation(chr, action, QuestNameForChr(chr), SCENES_TIMEOUT);
}

string GetStayAniName()
{
	float fl = frnd();
	if(fl<=0.02) return "idle_4";
	if(fl<=0.04) return "idle_5";
	if(fl<=0.09) return "idle_6";
	if(fl<=0.11) return "idle_7";
	if(fl<=0.16) return "idle_8";
	if(fl<=0.17) return "idle_9";
	if(fl<=0.18) return "idle_10";
	if(fl<=0.2) return "idle_3";
	if(fl<=0.3) return "idle_1";
	return "idle_2";
}

string GetSitAniName()
{
	switch(rand(4))
	{
		case 0:		return "Sit_Look_Around";	break;
		case 1:		return "Sit_Lower_head";	break;
		case 2:		return "Sit_HeadonHand";	break;
		case 3:		return "Sit_WaveHand";		break;
		case 4:		return "Sit_WaveHand";		break;
	}
}

void ClearOldScenes(ref chref)
{
	DeleteAttribute(chref,"SceneTask");
}

string QuestNameForChr(aref chref)
{
	string questName;
	aref arQuestList;
	int q;

	makearef(arQuestList,objQuestScene.list);
	q = GetAttributesNum(arQuestList);

	if(q<=0)	{questName = "_ScenesQuest_0";}
	else	{questName = "_ScenesQuest_" + q;}

	objQuestScene.list.(questName).chrIdx = chref.index;
	return questName;
}

ref ARefChrToRef(aref charef)
{
	return &Characters[sti(charef.index)];
}
