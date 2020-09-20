/*
Тип: актёр, исполняет указания скрипторов

	Используемые шаблоны:
		stay
		goto
		runto
		follow
		dialog
*/



#define LAI_TYPE_ACTOR	"actor"


//Инициализация
void LAi_type_actor_Init(aref chr)
{
	bool isNew = false;
	if(CheckAttribute(chr, "chr_ai.type") == false)
	{
		isNew = true;
	}else{
		if(chr.chr_ai.type != LAI_TYPE_ACTOR) isNew = true;
	}
	if(isNew == true)
	{
		//Новый тип
		DeleteAttribute(chr, "chr_ai.type");
		chr.chr_ai.type = LAI_TYPE_ACTOR;
		chr.chr_ai.type.quest = "";
		chr.chr_ai.type.state = "wait";
		chr.chr_ai.type.checkhp = "";
		chr.chr_ai.type.lock = "0";
		chr.chr_ai.type.mode = "stay";
		//Установим шаблон стояния
		LAi_tmpl_stay_InitTemplate(chr);
	}else{
		//Востанавливаем состояние
		if(!CheckAttribute(chr, "chr_ai.type.state")) chr.chr_ai.type.state = "wait";
		if(!CheckAttribute(chr, "chr_ai.type.quest")) chr.chr_ai.type.quest = "";		
		if(!CheckAttribute(chr, "chr_ai.type.checkhp")) chr.chr_ai.type.checkhp = "";
		if(!CheckAttribute(chr, "chr_ai.type.lock")) chr.chr_ai.type.lock = "0";
		if(!CheckAttribute(chr, "chr_ai.type.mode")) chr.chr_ai.type.mode = "stay";
	}
	//Установим анимацию персонажу
	if (chr.model.animation == "mushketer")
	{
        while (FindCharacterItemByGroup(chr, BLADE_ITEM_TYPE) != "")
        {
            TakeItemFromCharacter(chr, FindCharacterItemByGroup(chr, BLADE_ITEM_TYPE));
        }
        while (FindCharacterItemByGroup(chr, GUN_ITEM_TYPE) != "")
        {             
            TakeItemFromCharacter(chr, FindCharacterItemByGroup(chr, GUN_ITEM_TYPE));
        }		
		GiveItem2Character(chr, "unarmed");
		EquipCharacterbyItem(chr, "unarmed");
		GiveItem2Character(chr, "mushket");
		EquipCharacterbyItem(chr, "mushket");
	}
	else
	{
		if(chr.chr_ai.type.mode == "sit")
		{
			LAi_ActorSetSitMode(chr);
		}else{
			if(chr.chr_ai.type.mode == "lay")
			{
				LAi_ActorSetLayMode(chr);
			}else{
				LAi_ActorSetStayMode(chr);
			}
		}	
	}
	SendMessage(&chr, "lsl", MSG_CHARACTER_EX_MSG, "SetFightWOWeapon", false);
}

//Процессирование типа персонажа
void LAi_type_actor_CharacterUpdate(aref chr, float dltTime)
{
	aref type;
	makearef(type, chr.chr_ai.type);
	if(type.state == "dialogcan")
	{
		LAi_type_actor_CheckStartDialog(chr);
	}else{
		if(type.state == "attack")
		{
			if(LAi_tmpl_fight_LostTarget(chr))
			{
				LAi_Character_TemplateComplite(chr, LAI_TMPL_FIGHT);
			}
		}
	}
}

//Загрузка персонажа в локацию
bool LAi_type_actor_CharacterLogin(aref chr)
{
	return true;
}

//Выгрузка персонажа из локацию
bool LAi_type_actor_CharacterLogoff(aref chr)
{
	return true;
}

//Завершение работы темплейта
void LAi_type_actor_TemplateComplite(aref chr, string tmpl)
{
	if(chr.chr_ai.type != LAI_TYPE_ACTOR)
	{
		return;
	}
	//Если уже сриботал квест, то просто скипаем
	if(chr.chr_ai.type.state == "wait") return;
	//Если уже стоит новый шаблон, то тоже скипаем
	if(chr.chr_ai.tmpl != tmpl) return;
	//Стоит ли квест на выполнение
	string quest = chr.chr_ai.type.quest;
	chr.chr_ai.type.quest = "";
	if(chr.chr_ai.type.state == "dialoggo")
	{
		//Пришли к персонажу, начинаем диалог
		chr.chr_ai.type.lock = "0";
		LAi_ActorDialogNow(chr, &Characters[sti(chr.chr_ai.type.dlgchr)], quest, stf(chr.chr_ai.type.dlgtime));
		return;
	}
	if(chr.chr_ai.type.state == "exitfromloc")
	{
		//Дошли до локатора, перегружаемся в другую локацию
		ChangeCharacterAddressGroup(chr, chr.chr_ai.type.location, chr.chr_ai.type.group, chr.chr_ai.type.locator);
	}
	//Разлочим персонажа
	chr.chr_ai.type.lock = "0";
	//Если не следование, сбросим состояние
	bool isResetActor = true;
	if(tmpl == LAI_TMPL_FOLLOW)
	{
		isResetActor = false;
	}
	if(tmpl == LAI_TMPL_ANI)
	{
		if(quest == "")
		{
			isResetActor = false;
		}
	}
	if(isResetActor) 
	{
		LAi_type_actor_Reset(chr);
	}
	if(quest != "") CompleteQuestName(quest, "");
}

//Сообщить о желании завести диалог
void LAi_type_actor_NeedDialog(aref chr, aref by)
{
}

//Запрос на диалог, если возвратить true то в этот момент можно начать диалог
bool LAi_type_actor_CanDialog(aref chr, aref by)
{
	bool canDialog = false;
	if(chr.chr_ai.type.state == "dialogwait")
	{
		if(sti(chr.chr_ai.type.dlgchr) == sti(by.index)) canDialog = true;
	}else{
		if(chr.chr_ai.type.state == "dialoggo")
		{
			if(sti(chr.chr_ai.type.dlgchr) == sti(by.index)) canDialog = true;
		}
	}
	return canDialog;
}

//Начать диалог
void LAi_type_actor_StartDialog(aref chr, aref by)
{
	if(chr.chr_ai.type.state == "dialogself")
	{
		LAi_tmpl_stay_InitTemplate(chr);
		return;
	}else{
		if(chr.chr_ai.tmpl != LAI_TMPL_DIALOG)
		{
			//Если мы пасивны, запускаем шаблон без времени завершения
			LAi_tmpl_SetActivatedDialog(chr, by);
			chr.chr_ai.type.state = "dialog";
		}
		if(chr.chr_ai.type.mode == "stay")
		{
			CharacterTurnByChr(chr, by);
		}else{
			LAi_tmpl_dialog_NoAni(chr);
		}
	}
}

//Закончить диалог
void LAi_type_actor_EndDialog(aref chr, aref by)
{
	if(chr.chr_ai.type.state == "dialogself")
	{
		LAi_tmpl_stay_InitTemplate(chr);
		LAi_type_actor_TemplateComplite(chr, LAI_TMPL_STAY);
	}else{
		if(chr.chr_ai.tmpl != LAI_TMPL_DIALOG) Trace("LAi_type_actor_EndDialog -> LAi_type_actor_Reset");
		LAi_type_actor_TemplateComplite(chr, LAI_TMPL_DIALOG);
	}
}

//Персонаж выстрелил
void LAi_type_actor_Fire(aref attack, aref enemy, float kDist, bool isFindedEnemy)
{

}

//Персонаж атакован
void LAi_type_actor_Attacked(aref chr, aref by)
{
	if (CheckAttribute(chr, "BreakTmplAndFight"))
	{
		LAi_SetWarriorTypeNoGroup(chr);
	}
	if (CheckAttribute(chr, "BreakTmplAndFightGroup"))
	{
		LAi_group_FightGroups(chr.chr_ai.group, by.chr_ai.group, true);
	}
}

bool LAi_type_actor_Error(aref chr, bool lockTest)
{
	if (!CheckAttribute(chr, "chr_ai.type") || chr.chr_ai.type != LAI_TYPE_ACTOR)
	{
		if (MOD_BETTATESTMODE == "On")
		{
		    Log_TestInfo("Actor error: character <" + chr.id + "> now is not actor, his have type <" + chr.chr_ai.type + ">");
		}
		return true;
	}
	if(lockTest)
	{
		if(sti(chr.chr_ai.type.lock) != 0)
		{
			Trace("Actor error: character <" + chr.id + "> now is do template <" + chr.chr_ai.tmpl + ">, his not is free for new task");
			return true;
		}
	}
	return false;
}


//------------------------------------------------------------------------------------------
//Scripters ai program interface
//------------------------------------------------------------------------------------------
//
// параметр quest:
//           если "" то никакой реакции по окончанию действия не будет
//           если "questName" то по окончанию действия сработает указанный квест
// параметр timeout:
//           если -1 то задание будет выполняться, пока не выполниться
//           если >= 0 то по истечении заданного времени просимулируется выполнение задания
//
//------------------------------------------------------------------------------------------

//Указать актёру стоять
void LAi_ActorStay(aref chr)
{
	if(LAi_type_actor_Error(chr, false)) return;
	chr.chr_ai.type.state = "stay";
	chr.chr_ai.type.quest = "";
	LAi_tmpl_stay_InitTemplate(chr);
}

//Указать актёру идти в заданный локатор
void LAi_ActorGoToLocator(aref chr, string group, string locator, string quest, float timeout)
{
	if(LAi_type_actor_Error(chr, true)) return;
	chr.chr_ai.type.state = "goto";
	chr.chr_ai.type.quest = quest;
	LAi_tmpl_goto_InitTemplate(chr);
	LAi_tmpl_goto_SetLocator(chr, group, locator, timeout);
	chr.chr_ai.type.lock = "1";
}

//Указать актёру бежать в заданный локатор
void LAi_ActorRunToLocator(aref chr, string group, string locator, string quest, float timeout)
{
	if(LAi_type_actor_Error(chr, true)) return;
	chr.chr_ai.type.state = "runto";
	chr.chr_ai.type.quest = quest;
	LAi_tmpl_runto_InitTemplate(chr);
	LAi_tmpl_runto_SetLocator(chr, group, locator, timeout);
	chr.chr_ai.type.lock = "1";
}

//Указать актёру идти в заданную локацию
void LAi_ActorGoToLocation(aref chr, string groupExit, string locatorExit, string locID, string groupEnter, string locatorEnter, string quest, float timeout)
{
	if(LAi_type_actor_Error(chr, true)) return;
	chr.chr_ai.type.state = "exitfromloc";
	chr.chr_ai.type.quest = quest;
	chr.chr_ai.type.location = locID;
	chr.chr_ai.type.group = groupEnter;
	chr.chr_ai.type.locator = locatorEnter;
	LAi_tmpl_goto_InitTemplate(chr);
	LAi_tmpl_goto_SetLocator(chr, groupExit, locatorExit, timeout);
	chr.chr_ai.type.lock = "1";
}

//Указать актёру бежать в заданную локацию
void LAi_ActorRunToLocation(aref chr, string groupExit, string locatorExit, string locID, string groupEnter, string locatorEnter, string quest, float timeout)
{
	if(LAi_type_actor_Error(chr, true)) return;
	chr.chr_ai.type.state = "exitfromloc";
	chr.chr_ai.type.quest = quest;
	chr.chr_ai.type.location = locID;
	chr.chr_ai.type.group = groupEnter;
	chr.chr_ai.type.locator = locatorEnter;
	LAi_tmpl_runto_InitTemplate(chr);
	LAi_tmpl_runto_SetLocator(chr, groupExit, locatorExit, timeout);
	chr.chr_ai.type.lock = "1";
}

//Указать актёру cледовать за персонажем в пределах локации
//При первом подходе или истечении времени сработает квест
void LAi_ActorFollow(aref chr, aref follow, string quest, float timeout)
{
	if(LAi_type_actor_Error(chr, true)) return;
	chr.chr_ai.type.state = "follow";
	chr.chr_ai.type.quest = quest;
	LAi_tmpl_SetFollow(chr, follow, timeout);
	chr.chr_ai.type.lock = "1";
}

//Указать актёру cледовать за игроком по всем локациям
//При первом подходе или истечении времени сработает квест
void LAi_ActorFollowEverywhere(aref chr, string quest, float timeout)
{
	if(LAi_type_actor_Error(chr, true)) return;
	chr.location.follower = "1";
	LAi_ActorFollow(chr, pchar, quest, timeout);
	chr.chr_ai.type.lock = "0";
}

//Указать актёру на кого напасть
void LAi_ActorAttack(aref chr, aref enemy, string quest)
{
	if(LAi_type_actor_Error(chr, true)) return;
	chr.chr_ai.type.state = "attack";
	chr.chr_ai.type.quest = quest;
	LAi_tmpl_SetFight(chr, enemy);
	chr.chr_ai.type.lock = "1";
}

//Указать актёру бояться другого персонажа
void LAi_ActorAfraid(aref chr, aref by, bool canMove)
{
	if(LAi_type_actor_Error(chr, true)) return;
	chr.chr_ai.type.state = "afraid";
	chr.chr_ai.type.quest = "";
	LAi_tmpl_afraid_SetAfraidCharacter(chr, by, canMove);
	chr.chr_ai.type.lock = "0";
}


//Активировать диалог между актёром и другим персонажем с подходом друг к другу
void LAi_ActorDialog(aref chr, aref to, string quest, float timeout, float dlgTime)
{
	if(LAi_type_actor_Error(chr, true)) return;
	chr.chr_ai.type.state = "dialoggo";
	chr.chr_ai.type.quest = quest;
	chr.chr_ai.type.dlgtime = dlgTime;
	chr.chr_ai.type.dlgchr = to.index;
	LAi_tmpl_SetFollow(chr, to, timeout);
	chr.chr_ai.type.lock = "1";
}

//Активировать диалог между актёром и другим персонажем немедленно
void LAi_ActorDialogNow(aref chr, aref to, string quest, float dlgTime)
{
	if(LAi_type_actor_Error(chr, true)) return;
	chr.chr_ai.type.state = "dialogcan";
	chr.chr_ai.type.quest = quest;
	chr.chr_ai.type.dlgtime = dlgTime;
	chr.chr_ai.type.dlgchr = to.index;
	//LAi_tmpl_SetFollow(chr, to, -1.0);
	LAi_type_actor_CheckStartDialog(chr);
	chr.chr_ai.type.lock = "1";
}

//Активировать диалог между актёром и другим персонажем с задержкой. eddy.
void LAi_ActorDialogDelay(aref chr, aref to, string quest, float delayTime)
{
    pchar.GenQuest.CallFunctionParam = "ActorDialogDelay";
    pchar.GenQuest.CallFunctionParam.ActorDialogDelay.chrId = chr.id;
    pchar.GenQuest.CallFunctionParam.ActorDialogDelay.chrToInd = to.index;
	pchar.GenQuest.CallFunctionParam.ActorDialogDelay.quest = quest;
    DoQuestCheckDelay("CallFunctionParam", delayTime);
}
void ActorDialogDelay()    // относится к методу выше.
{
	ref chr = characterFromID(pchar.GenQuest.CallFunctionParam.ActorDialogDelay.chrId);
	if(LAi_type_actor_Error(chr, true)) return;
	chr.chr_ai.type.state = "dialogcan";
	chr.chr_ai.type.quest = pchar.GenQuest.CallFunctionParam.ActorDialogDelay.quest;
	chr.chr_ai.type.dlgtime = "-1";
	chr.chr_ai.type.dlgchr = sti(pchar.GenQuest.CallFunctionParam.ActorDialogDelay.chrToInd);
	LAi_type_actor_CheckStartDialog(chr);
	chr.chr_ai.type.lock = "1";
}


//Активировать диалог с самим собой
void LAi_ActorSelfDialog(aref chr, string quest)
{
	if(LAi_type_actor_Error(chr, true)) return;
	chr.chr_ai.type.state = "dialogself";
	chr.chr_ai.type.quest = quest;
	LAi_tmpl_stay_InitTemplate(chr);
	SelfDialog(chr);
	chr.chr_ai.type.lock = "1";
}

//Указать актёру стоять и ждать диалога от другого персонажа
void LAi_ActorWaitDialog(aref chr, aref by)
{
	if(LAi_type_actor_Error(chr, false)) return;
	chr.chr_ai.type.state = "dialogwait";
	chr.chr_ai.type.quest = "";
	chr.chr_ai.type.dlgchr = by.index;
	LAi_tmpl_stay_InitTemplate(chr);
	chr.chr_ai.type.lock = "1";
}

//eddy. Указать актеру возможность диалога с другим персонажем без остановки актера.
// не перебивает вызов квеста, забитого предыдущей командой!!!
void LAi_Actor2WaitDialog(aref chr, aref by)
{
	if(LAi_type_actor_Error(chr, false)) return;
	chr.chr_ai.type.state = "dialogwait";
	chr.chr_ai.type.dlgchr = by.index;
	chr.chr_ai.type.lock = "1";
}

//Проиграть анимацию для актёра, по окончанию вызвать квест
//Если анимация зацикленна, то квест вызовется по истечению времени
void LAi_ActorAnimation(aref chr, string animation, string quest, float timeout)
{
	if(LAi_type_actor_Error(chr, false)) return;
	chr.chr_ai.type.state = "animation";
	chr.chr_ai.type.quest = quest;
	LAi_tmpl_ani_PlayAnimation(chr, animation, timeout);
	chr.chr_ai.type.lock = "1";
}

//Ориентировать актёра на персонажа (одноразово)
void LAi_ActorTurnToCharacter(aref chr, aref to)
{
	if(LAi_type_actor_Error(chr, false)) return;
	chr.chr_ai.type.state = "";
	LAi_tmpl_stay_InitTemplate(chr);
	CharacterTurnByChr(chr, to);
	chr.chr_ai.type.lock = "0";
}

//Ориентировать актёра по локатору
void LAi_ActorTurnByLocator(aref chr, string group, string locator)
{
	if(LAi_type_actor_Error(chr, false)) return;
	chr.chr_ai.type.state = "";
	LAi_tmpl_stay_InitTemplate(chr);
	CharacterTurnByLoc(chr, group, locator);
	chr.chr_ai.type.lock = "0";
}

//Ориентировать актёра на локатор
void LAi_ActorTurnToLocator(aref chr, string group, string locator)
{
	if(LAi_type_actor_Error(chr, false)) return;
	chr.chr_ai.type.state = "";
	LAi_tmpl_stay_InitTemplate(chr);
	CharacterTurnToLoc(chr, group, locator);
	chr.chr_ai.type.lock = "0";
}

//Установить анимацию стоячего персонажа
void LAi_ActorSetStayMode(aref chr)
{
	if(LAi_type_actor_Error(chr, false)) return;
	chr.chr_ai.type.mode = "stay";
	LAi_SetDefaultStayAnimation(chr);
}

//Установить анимацию сидячего персонажа
void LAi_ActorSetSitMode(aref chr)
{
	if(LAi_type_actor_Error(chr, false)) return;
	chr.chr_ai.type.mode = "sit";
	LAi_SetDefaultSitAnimation(chr);
}

//Установить анимацию лежачего персонажа
void LAi_ActorSetLayMode(aref chr)
{
	if(LAi_type_actor_Error(chr, false)) return;
	chr.chr_ai.type.mode = "lay";
	BeginChangeCharacterActions(chr);
	chr.actions.idle.i1 = "Lay_1";
	EndChangeCharacterActions(chr);
}

//------------------------------------------------------------------------------------------
//Внутреннии функции
//------------------------------------------------------------------------------------------

//Перейти в состояние ожидания новой команды
void LAi_type_actor_Reset(aref chr)
{
	DeleteAttribute(chr, "location.follower");
	if(LAi_type_actor_Error(chr, false)) return;
	LAi_tmpl_stay_InitTemplate(chr);
	chr.chr_ai.type.state = "wait";
	chr.chr_ai.type.lock = "0";
	CharacterPlayAction(chr, "");
}

//Начать по возможности диалог
void LAi_type_actor_CheckStartDialog(aref chr)
{
	if(LAi_type_actor_Error(chr, false)) return;
	if(chr.chr_ai.type.state != "dialogcan") return;
	int idx = sti(chr.chr_ai.type.dlgchr);
	if(LAi_Character_CanDialog(chr, &Characters[idx]))
	{
		chr.chr_ai.type.state = "dialogwait";
		LAi_tmpl_SetDialog(chr, &Characters[idx], stf(chr.chr_ai.type.dlgtime));
	}
}

