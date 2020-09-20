// диалог по городам
void ProcessCommonDialogEvent(ref NPChar, aref Link, aref NextDiag)
{
    ref sld;
   	float locx, locy, locz;
    switch (Dialog.CurrentNode)
	{
		case "quests":
			dialog.text = NPCStringReactionRepeat(RandPhraseSimple("Какие вопросы?", "Что вам угодно?"), "Совсем недавно вы пытались задать мне вопрос...", "М-да, позвольте угадаю... Опять ничего существенного?",
                          "Послушайте, я финансами оперирую, а не на вопросы отвечаю...", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat(RandPhraseSimple("Я передумал...", "Сейчас мне не о чем говорить"), "Хм, что-то с памятью моей стало...",
                      "Вы угадали, простите...", "Я понимаю...", npchar, Dialog.CurrentNode);
			link.l1.go = "exit";
            // ==> англ.линейка квест №4 Разведмиссию в Кюрасао
			if (pchar.questTemp.State == "Intelligence_Curacao_toYozef")
            {
                link.l1 = "Я хотел бы попросить у вас помощи в одном деле. Эта помощь будет щедро оплачена.";
                link.l1.go = "Step_E4_1";
            }
			if (pchar.questTemp.State == "Intelligence_Curacao_NoMoney_Yozef")
            {
                link.l1 = "Я хотел бы поговорить о том деле на 100000 монет.";
                link.l1.go = "no_money";
            }
            if (pchar.questTemp.State == "Inquisition_toCuracao" || pchar.questTemp.State == "Inquisition_afterFrancisco" )// квест №2 голл. линейки.
            {
                if (!CheckAttribute(pchar, "questTemp.State.Usurer"))
                {
        			dialog.text = "Что вам нужно?";
        			link.l1 = "Святая инквизиция предоставляет вам прощение Папы всего за 50 тысяч. Что такое? Почему я не вижу слез радости на глазах?";
        			link.l1.go = "Step_S2_1";
                }
                else
                {
                    if (CheckPassengerInCharacter(pchar, "JansenDeFonceicao"))
                    {
                        dialog.text = "Это опять ты! Вон отсюда!";
            			link.l1 = "Ты мне надоел! Закрой свою пасть. Условия теперь диктую я, а ты внимательно слушаешь, осмысливаешь, и уже начинаешь сожалеть о том, что сразу не внял моим словам.";
            			link.l1.go = "Step_S2_5";
                    }
                    else
                    {
                        if (pchar.questTemp.State.Usurer != "Ok")
                        {
                            dialog.text = "Что, ты еще не все мне сказки рассказал про прощение Папы?";
                			link.l1 = "Ладно, Йозеф, поговорим еще...";
                			link.l1.go = "exit";
                        }
                        else
                        {
                            dialog.text = "Что еще тебе нужно, проклятый кровопийца, иезуитский прихвостень...";
                			link.l1 = "Какие эмоции, какой кураж! Великолепно...";
                			link.l1.go = "exit";
                        }
                    }
                }
            }
			//пиратка, квест №5
			if (pchar.questTemp.piratesLine == "PL5Hunter_toVillemstad")
            {
				link.l1 = "Папаша, долги вытрясти с кого-нибудь не надо?!";
				link.l1.go = "Step_P5_1";
			}
		break;
//********************** Английская линейка, квест №4 "Разведка в Кюрасао" ************************
		case "Step_E4_1":
			dialog.text = "Говори, что тебе нужно?";
			link.l1 = "Я готов дорого заплатить за информацию. Мне нужно знать о планах генерал-губернатора Кюрасао в отношении англичан.";
			link.l1.go = "Step_E4_2";
		break;
		case "Step_E4_2":
			dialog.text = "Надеюсь, вы понимаете, о чем просите.";
			link.l1 = "Надеюсь, и вы понимаете, что я буду очень щедр.";
			link.l1.go = "Step_E4_3";
		break;
		case "Step_E4_3":
			dialog.text = "100000 монет и я расскажу вам все, что знаю.";
			link.l1 = "Я готов внести требуемую сумму.";
			link.l1.go = "agree_to_pay";
			link.l2 = "У меня нет с собой таких денег, но я обязательно вернусь позже.";
			link.l2.go = "no_money_now";
			link.l3 = "Не слишком ли жирно, грязная свинья?";
			link.l3.go = "fack_yozef";
		break;
		case "agree_to_pay":
            if (sti(pchar.money) >= 100000)
            {
        		dialog.text = "Что ж, могу вам доложить следующее: около часа тому назад в резиденции Питера Стэвезанта, генерал-губернатора этого острова, закончился военный совет по поводу как раз вашего вопроса - участие голландских колоний в торговой войне с Англией. На самом совещании я не присутствовал, но знаю, что по результатам оного вестовой генерал-губернатора отправляется куда-то по срочному поручению. На рейде вестового ожидает бриг.\n"+
                         "Вы по виду пират и вам не составит труда взять голландский бриг. Так что поторопитесь, пока тот не скрылся за горизонтом. И еще... я вас не знаю. Прощайте.";
        		link.l1 = "Вот ваши деньги. И я не знаю вас также, да и знать не хочу. Прощайте.";
        		link.l1.go = "exit";
        		AddMoneyToCharacter(pchar, -100000);
        		AddQuestRecord("Eng_Line_4_Intelligence_Curacao", "3");
        		pchar.questTemp.State = "Intelligence_Curacao_GaveMoneyYozef"; // заглушка на повторный вывод данного пункта в меню.
                Pchar.quest.Intelligence_Curacao_GoToShore.win_condition.l1 = "location";
                Pchar.quest.Intelligence_Curacao_GoToShore.win_condition.l1.location = "Curacao";
                Pchar.quest.Intelligence_Curacao_GoToShore.win_condition = "Intelligence_Curacao_GoToShore";
                Log_QuestInfo("Бриг установлен в бухте.");
            }
            else
            {
        		dialog.text = "Хмм, похоже таких денег у вас все-таки нет. Впредь, прошу вас, не морочьте мне голову. Жду вас с требуемой суммой.";
        		link.l1 = "Хорошо, я вас понял.";
        		link.l1.go = "exit";
        		pchar.questTemp.State = "Intelligence_Curacao_NoMoney_Yozef";
        		//ChangeCharacterReputation(pchar, -1); TO_DO eddy
            }
		break;
		case "no_money_now":
			dialog.text = "Ну что, обзаведитесь требуемой суммой. Я буду ждать вас.";
			link.l1 = "Думаю, я скоро управлюсь.";
			link.l1.go = "exit";
            if (pchar.questTemp.State != "Intelligence_Curacao_NoMoney_Yozef")
            {
                AddQuestRecord("Eng_Line_4_Intelligence_Curacao", "4");
            }
            pchar.questTemp.State = "Intelligence_Curacao_NoMoney_Yozef";
		break;
		case "no_money":
			dialog.text = "Вы раздобыли деньги? Где мои 100000?";
			link.l1 = "Да, деньги у меня и я готов их вам вручить. Так что же вам удалось узнать?";
			link.l1.go = "agree_to_pay";
			link.l2 = "Нет еще, но я обязательно достану их и вернусь позже.";
			link.l2.go = "no_money_now";
			link.l3 = "Не слишком ли жирно, грязная свинья?";
			link.l3.go = "fack_yozef";
		break;
		case "fack_yozef":    // посыл Йозефа с попыткой замочить
			dialog.text = "Да ты нахал! Попрошу убраться из моего дома немедленно!";
			link.l1 = "Черт тебя побери, проклятый кровопийца! Сейчас я тебе выпущу кишки!";
			link.l1.go = "Step_E4_4";
		break;
		case "Step_E4_4":
			dialog.text = "Не тут то было, ублюдок. Охрана, ко мне!!!";
			link.l1 = "Проклятье!";
			link.l1.go = "Step_E4_5"; 
		break;
		case "Step_E4_5":
            pchar.questTemp.State = "Intelligence_Curacao_FightGard";
            LAi_LocationFightDisable(&Locations[FindLocation("Villemstad_bank")], false);
            AddQuestRecord("Eng_Line_4_Intelligence_Curacao", "5");
            LAi_NoRebirthEnable(npchar);
            LAi_group_Register("EnemyFight");
            GetCharacterPos(pchar, &locx, &locy, &locz);
            for (int i=1; i<=3; i++)
            {
                sld = GetCharacter(NPC_GenerateCharacter("HolSolder"+i, "sold_hol_"+i, "man", "man", 20, HOLLAND, 0, true));
                FantomMakeCoolFighter(sld, 20, 70, 60, BLADE_LONG, "pistol2", 40);
            	LAi_SetWarriorType(sld);
                LAi_group_MoveCharacter(sld, "EnemyFight");
               	ChangeCharacterAddressGroup(sld, "Villemstad_bank", "goto", LAi_FindNearestFreeLocator("goto", locx, locy, locz));
            }
            LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
            LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
            LAi_SetCheckMinHP(pchar, 10, true, "Intelligence_Curacao_KickYozef");
            LAi_SetPlayerType(pchar);
            LAi_SetFightMode(pchar, true);
            Pchar.quest.Intelligence_Curacao_GoToShore.win_condition.l1 = "location";
            Pchar.quest.Intelligence_Curacao_GoToShore.win_condition.l1.location = "Curacao";
            Pchar.quest.Intelligence_Curacao_GoToShore.win_condition = "Intelligence_Curacao_GoToShore";
			DialogExit();
		break;
//********************** Испанская линейка, квест №2 "Инквизиция" ************************
		case "Step_S2_1":
			dialog.text = "Вы меня с кем-то перепутали. За мной нет никакой вины!";
			link.l1 = "Это вы так считаете, а орден другого мнения. Так что деньги вам придется заплатить.";
			link.l1.go = "Step_S2_2";
		break;
		case "Step_S2_2":
			dialog.text = "Я вам ничего платить не буду! И если вы сейчас же не уберетесь из моего дома, то я позову слуг!";
			link.l1 = "Ты слишком самонадеян. Но это лечится. И у меня как раз есть средство - моя шпага.";
			link.l1.go = "Step_S2_3";
			link.l2 = "Ну как знаешь, тебе же хуже будет...";
			link.l2.go = "Step_S2_4";
		break;
		case "Step_S2_3":
			dialog.text = "Убивают!!! Эндрю, ко мне!";
			link.l1 = "Эрндрю?..";
			link.l1.go = "Step_S2_fihgt";
		break;
		case "Step_S2_fihgt":
			bDisableFastReload = false; 
            AddQuestRecord("Spa_Line_2_Inquisition", "4");
            LAi_LocationFightDisable(&Locations[FindLocation("Villemstad_bank")], false);
            LAi_NoRebirthEnable(npchar);
            LAi_SetImmortal(npchar, false);
            LAi_SetActorType(npchar);
            LAi_ActorAfraid(npchar, pchar, false);
            sld = GetCharacter(NPC_GenerateCharacter("Andrew", "shipowner_7", "man", "man", 35, HOLLAND, 10, true));
            FantomMakeCoolFighter(sld, 35, 80, 60, BLADE_LONG, "pistol3", 70);
        	LAi_SetWarriorType(sld);
			LAi_NoRebirthEnable(sld);
            LAi_group_MoveCharacter(sld, "EnemyFight");
            GetCharacterPos(pchar, &locx, &locy, &locz);
           	ChangeCharacterAddressGroup(sld, "Villemstad_bank", "goto", LAi_FindNearestFreeLocator("goto", locx, locy, locz));
            LAi_group_SetRelation("EnemyFight", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);
            LAi_group_FightGroups("EnemyFight", LAI_GROUP_PLAYER, true);
			Pchar.quest.Inquisition_afterFightAndrew.win_condition.l1 = "MapEnter";
            Pchar.quest.Inquisition_afterFightAndrew.win_condition = "Inquisition_afterFightAndrew";
            NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
		case "Step_S2_4":
			dialog.text = "Ты что же, угрожать мне вздумал?";
			link.l1 = "Да нет, почему же. Все нормально, Иозеф...";
			link.l1.go = "exit";
			AddQuestRecord("Spa_Line_2_Inquisition", "5");
			pchar.questTemp.State.Usurer = 1;
		break;
		case "Step_S2_5":
			dialog.text = "И с чего это я должен выслушивать бред, который ты несешь?";
			link.l1 = "Все просто, Йозеф - твой сын в моих руках. Я убедительно говорю? Или тебе предъявить доказательства? Ну к примеру я могу принести отрезанный палец, или ухо твоего сына. Хочешь?";
			link.l1.go = "Step_S2_6";
		break;
		case "Step_S2_6":
			dialog.text = "Ты... Не надо, я верю... Если я сейчас отдам деньги, то где гарантия, что вы не расправитесь с ним?";
			link.l1 = "А такой гарантии нет. Побудь хоть немного в шкуре тех, кого ты доводил до отчаяния. Узнай, почем фунт лиха. И хватит болтать, давай деньги.";
			link.l1.go = "Step_S2_7";
		break;
		case "Step_S2_7":
			dialog.text = "Забирай. Но я этого не забуду.";
			link.l1 = "Нет проблем. Помни. О том, что Орден следит за тобой, тоже помни...";
			link.l1.go = "Step_S2_8";
		break;
		case "Step_S2_8":
            AddQuestRecord("Spa_Line_2_Inquisition", "10");
			ReleasePrisoner(characterFromId("JansenDeFonceicao"));
            AddMoneyToCharacter(pchar, 50000);
            pchar.questTemp.State.Usurer = "Ok"; //бабки получены
            NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
//********************** Пиратская линейка, квест №5 ОГЗ ************************
		case "Step_P5_1":
			dialog.text = "Нет. Не надо.\nХе, вы, так называемые вытрясыватели долгов, одинаковые, как однояйцевые близнецы. Вот и Эйвори точно так же разговаривает...";
			link.l1 = "Джон Эйвори?! Так это мой приятель. Он еще в городе? Где его можно отыскать?";
			link.l1.go = "Step_P5_2";
		break;	
		case "Step_P5_2":
			dialog.text = "Понятия не имею. Он вольный человек, ходит, где заблагорассудится. А насчет того, что он еще в Виллемстаде, это вряд ли, здесь для него работы не осталось. Вы же приятель Джона, должны понимать, что искать его надо там, где ром льется рекой - в тавернах!";
			link.l1 = "Ха! Ну конечно, кто бы сомневался... Но ты точно ничего такого не припомнишь, куда он мог податься? Может чего-то сказал?..";
			link.l1.go = "Step_P5_3";
		break;
		case "Step_P5_3":
			pchar.questTemp.piratesLine.Q5.city_1 = GetQuestNationsCity(ENGLAND); 
			dialog.text = "Сказал, что теперь к англичанам собиратеся. Вроде о " + XI_ConvertString("Colony" + pchar.questTemp.piratesLine.Q5.city_1 + "Dat") + " шла речь, но точно не скажу.";
			link.l1 = "А говорил, что понятия не имеешь. Ну спасибо тебе!";
			link.l1.go = "exit";
			AddQuestRecord("Pir_Line_5_Hunter", "2");
			AddQuestUserData("Pir_Line_5_Hunter", "sCity", XI_ConvertString("Colony" + pchar.questTemp.piratesLine.Q5.city_1));
			AddQuestUserData("Pir_Line_5_Hunter", "sIsland", XI_ConvertString(GetIslandByCityName(pchar.questTemp.piratesLine.Q5.city_1) + "Dat"));
			pchar.questTemp.piratesLine = "PL5Hunter_toFirst";
		break;		
	}
	UnloadSegment(NPChar.FileDialog2);  // если где-то выход внутри switch  по return не забыть сделать анлод
}

