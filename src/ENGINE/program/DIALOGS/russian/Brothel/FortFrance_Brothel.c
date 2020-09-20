// диалог по городам
void ProcessCommonDialogEvent(ref NPChar, aref Link, aref NextDiag)
{
    ref sld;   
	switch (Dialog.CurrentNode)
	{
		case "quests":
			dialog.text = NPCStringReactionRepeat(RandPhraseSimple("Какие вопросы, молодой человек?", "Чего ты хочешь, красавчик? Задавай свои вопросы..."), "Опять вопросы?", "Хи-хи, третий раз на дню одно и то же - вопросы...",
                          "Хм, может ты выберешь какую-нибудь красотку себе? А то у меня уже складывается в отношении тебя вполне определенное мнение...", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat(RandPhraseSimple("Я передумал...", "Хм, да ничего..."), "Не могу... Нет вопросов...",
                      "Да уж, действительно в третий раз... Извини.", "Не сейчас... В другой раз...", npchar, Dialog.CurrentNode);
			link.l1.go = "exit";
			// ==> Проверяем поле состояния квестов.
            if (pchar.questTemp.State == "SeekBible_toFFBrothel" && npchar.id == "FortFrance_Hostess") //Голландская линейка, квест №8. узнаем о пирате, который о Библии базарил.
			{
                link.l1 = "Послушай, некоторое время назад у тебя отдыхал Джекмен в компании с корсарами...";
                link.l1.go = "Step_H8_1";
			}
			if (pchar.questTemp.Sharp == "seekSharp" && GetNpcQuestPastDayWOInit(npchar, "quest.SharpTime") > 7 && !LAi_IsDead(&characters[GetCharacterIndex("Sharp")]))
			{
				link.l1 = "Послушай, " + npchar.name + ", я ищу Шарпа. Ты не знаешь, где он?";
				link.l1.go = "SharpPearl_1";
				SaveCurrentNpcQuestDateParam(npchar, "quest.SharpTime");
			}
            // <== Проверяем поле состояния квестов.
		break;

        case "Step_H8_1":
            dialog.text = "Молодой человек, они отдыхают у нас в компании девушек, а не друг друга! Следите за тем, что произносите...";
            link.l1 = "Хорошо, они отдыхали в большой компании...";
            link.l1.go = "Step_H8_2";
        break;
        case "Step_H8_2":
            dialog.text = "Действительно в большой, народу там было много. Надолго мне запомнится этот их отдых...";
            link.l1 = "Почему, позволь тебя спросить?";
            link.l1.go = "Step_H8_3";
        break;
        case "Step_H8_3":
            dialog.text = "Да потому, что все мое заведение работало на них в эту ночь. Ужас был какой-то!";
            link.l1 = "Понятно... Слушай, ты не слышала вот о чем: кто-то пытался продать одну книгу, вроде Библию...";
            link.l1.go = "Step_H8_4";
        break;
        case "Step_H8_4":
            dialog.text = "Нет, такого я не слышала.";
            link.l1 = "А может девушки твои подскажут?";
            link.l1.go = "Step_H8_5";
        break;
        case "Step_H8_5":
            dialog.text = "Может и подскажут. Только мне до этого дела нет, спрашивай сам.";
            link.l1 = "А кого? Кто у них там был в эту ночь?";
            link.l1.go = "Step_H8_6";
        break;
        case "Step_H8_6":
            dialog.text = "Все были! Все, как одна! Всех и спрашивай. Может действительно кто и вспомнит...";
            link.l1 = "Спасибо тебе, пойду спрашивать...";
            link.l1.go = "exit";
            pchar.questTemp.State = "SeekBible_toGirl";
        break;
		// капитан Шарп, грабеж жемчужных промыслов
		case "SharpPearl_1":
			if (rand(1) || bBettaTestMode)
			{
				if (sti(pchar.questTemp.Sharp.brothelChance) < 9)
				{	//футболим геймера в город
					pchar.questTemp.Sharp.City = GetSharpCity();
					pchar.questTemp.Sharp.City.rumour = true; //флаг дачи слуха
					dialog.text = "Насколько мне известно, капитан Шарп отправился в " + XI_ConvertString("Colony" + pchar.questTemp.Sharp.City + "Acc") + ".";
					link.l1 = "Хех! Благодарю тебя, " + npchar.name + ". Ты мне здорово помогла!";
					link.l1.go = "exit";
					pchar.questTemp.Sharp = "toSharp_going"; //в бордели больше не заходить
					ReOpenQuestHeader("SharpPearl");
					AddQuestRecord("SharpPearl", "1");
					AddQuestUserData("SharpPearl", "sCity", XI_ConvertString("Colony" + npchar.city + "Dat"));
					AddQuestUserData("SharpPearl", "sTarget", XI_ConvertString("Colony" + pchar.questTemp.Sharp.City + "Acc"));
					if (GetIslandByCityName(pchar.questTemp.Sharp.City) != pchar.questTemp.Sharp.City)
					{
						AddQuestUserData("SharpPearl", "sAreal", ", что находится на " + XI_ConvertString(GetIslandByCityName(pchar.questTemp.Sharp.City) + "Dat"));
					}
					//запускаем энкаунтер Шарпа на карте
					group_DeleteGroup("Sharp_Group");
					sld = characterFromId("Sharp");
					string sGroup = "Sharp_Group";
					Group_FindOrCreateGroup(sGroup);
					Group_SetTaskAttackInMap(sGroup, PLAYER_GROUP);
					Group_LockTask(sGroup);
					Group_AddCharacter(sGroup, sld.id);
					Group_SetGroupCommander(sGroup, sld.id);
					sld.mapEnc.type = "trade";
					sld.mapEnc.worldMapShip = "quest_ship";
					sld.mapEnc.Name = "бриг 'Шарпоносец'";
					string sColony= SelectAnyColony(npchar.city); //колония, откуда плывет Шарп
					int daysQty = GetMaxDaysFromIsland2Island(GetArealByCityName(sColony), GetArealByCityName(pchar.questTemp.Sharp.City))+3; //дней доехать даем с запасом
					Map_CreateTrader(sColony, pchar.questTemp.Sharp.City, sld.id, daysQty);
					Log_TestInfo("Шарпоносец установлен из " + sColony + "  в " + pchar.questTemp.Sharp.City);
				}
				else
				{	//чудо, Шарп здесь, в борделе!
					dialog.text = "Знаю. Он у меня в заведении отдыхает. Могу позвать, если он тебе срочно нужен.";
					link.l1 = "Сделай, одолжение, " + npchar.name + "...";
					link.l1.go = "SharpPearl_2";
				}
			}
			else
			{
				dialog.text = "Не знаю, у меня его давно уже не было.";
				link.l1 = "Понятно... Ну что же, спасибо тебе, " + npchar.name + ".";
				link.l1.go = "exit";
			}
		break;
		case "SharpPearl_2":
			dialog.text = "Барталомью-ю-ша! Зайди ко мне, дорогой, тебя тут спрашивают!";
			link.l1 = "Хех, а Шарп у вас в почете, как я погляжу...";
			link.l1.go = "SharpPearl_3";
		break;
		case "SharpPearl_3":
			chrDisableReloadToLocation = true;
			pchar.questTemp.Sharp.count = 0; //счетчик нулим для след. раза
			pchar.questTemp.Sharp.price = 5000+rand(20)*1000; //цена за наводку
			pchar.questTemp.Sharp.price.evil = rand(1); //добрый или злой, для скидки
			sld = &characters[GetCharacterIndex("Sharp")];
			sld.dialog.currentnode = "BrothelSharp";
			LAi_group_MoveCharacter(sld, LAI_GROUP_PLAYER_OWN);
			ChangeCharacterAddressGroup(sld, pchar.location, "reload", "reload1");
			LAi_SetActorTypeNoGroup(sld);
			LAi_ActorDialog(sld, pchar, "", -1, 0);
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
	}
	UnloadSegment(NPChar.FileDialog2);  // если где-то выход внутри switch  по return не забыть сделать анлод
}

