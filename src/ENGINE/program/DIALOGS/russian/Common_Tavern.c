#include "DIALOGS\russian\Rumours\Common_rumours.c"  //homo 25/06/06
void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);
	
    // вызов диалога по городам -->
    NPChar.FileDialog2 = "DIALOGS\" + LanguageGetLanguage() + "\Tavern\" + NPChar.City + "_Tavern.c";
    if (LoadSegment(NPChar.FileDialog2))
	{
        ProcessCommonDialog(NPChar, Link, NextDiag);
		UnloadSegment(NPChar.FileDialog2);
	}
    // вызов диалога по городам <--
	ProcessCommonDialogRumors(NPChar, Link, NextDiag);//homo 25/06/06
	
	int iTest;
    iTest = FindColony(NPChar.City); // город 
    ref rColony;
	if (iTest != -1)
	{
		rColony = GetColonyByIndex(iTest);
	}
	bool ok;
	
	switch(Dialog.CurrentNode)
	{
		case "First time":
			NextDiag.TempNode = "First time";
			if (LAi_grp_playeralarm > 0)
			{
       			dialog.text = NPCharRepPhrase(pchar,
					LinkRandPhrase("В городе поднята тревога, тебя всюду ищут! На твоем месте я бы не стал здесь задерживаться.", "Вся городская стража рыщет по городу в поисках тебя. Я не идиот и разговаривать с тобой не буду!", "Беги, приятель, пока солдаты не сделали из тебя решето..."),
					LinkRandPhrase("Что тебе нужно, негодяй?! Городская стража уже взяла твой след, далеко тебе не уйти, грязный пират!", "Грязный убийца, вон из моего дома! Стража!!", "Я не боюсь тебя, мерзавец! Скоро тебя повесят в нашем форте, далеко тебе не уйти..."));
				link.l1 = NPCharRepPhrase(pchar,
					RandPhraseSimple("Хех, тревога для меня не проблема...", "Им меня ни за что не поймать."),
					RandPhraseSimple("Заткни свою пасть, " + GetWorkTypeOfMan(npchar, "") + ", а не то вырву твой поганый язык!", "Хех, " + GetWorkTypeOfMan(npchar, "") + ", а все туда же - пиратов ловить! Вот что я тебе скажу, приятель: сиди тихо и будешь жить..."));
				link.l1.go = "exit";
				break;
			}
			
            // homo линейка капитана Блада
            if (Pchar.questTemp.CapBloodLine == true )
            {


                dialog.Text = LinkRandPhrase("Эй, доктор Блад! " + TimeGreeting() + ".",
                                    "Рад видеть Вас, Питер Блад.",
                                    "Хорошо, что Вы заглянули ко мне, " + GetFullName(pchar) + ". Как поживает полковник Бишоп?");
                Link.l1 = "Увы, я уже ухожу, " + NPChar.name + ". До встречи.";
				Link.l1.go = "exit";

                if(Pchar.questTemp.CapBloodLine.stat == "WakerOffer")
                {
            		link.l2 = "Не подскажешь ли, дружище " + NPChar.name + ", как найти доктора Вакера?";
            		link.l2.go = "TStep_0";
                }
                
                if(Pchar.questTemp.CapBloodLine.stat == "PrepareToEscape")
                {
            		link.l2 = "У меня к вам одно деликатное дело.";
            		link.l2.go = "TStep_1";
                }
                break;
    		}

			if(NPChar.quest.meeting == "0")
			{
				dialog.Text = "Добро пожаловать, " + GetAddress_Form(NPChar) + ". Еда, выпивка и развлечения - все по умеренным ценам!";
				Link.l1 = "Посмотрим... Я " + GetFullName(pchar) + ".";
				Link.l1.go = "meeting";
				NPChar.quest.meeting = "1";
			}
			else
			{
				dialog.Text = LinkRandPhrase("Эй, " + GetAddress_Form(NPChar) + " " + PChar.name + "! " + TimeGreeting() + ".",
                                    "О, какие у нас гости! Рад видеть вас, " + GetAddress_Form(NPChar) + " " + PChar.name + ".",
                                    "Хорошо, что вы заглянули ко мне, " + GetFullName(pchar) + ". Каков будет сегодня ваш заказ?");
				Link.l1 = "Есть на острове парни, готовые уйти в плавание?";
				Link.l1.go = "crew hire";
				Link.l2 = "Я надеюсь, ты сможешь ответить на пару вопросов.";
				Link.l2.go = "int_quests";
				if (CheckAttribute(pchar, "GenQuest.LoanChest.TakeChest") && sti(pchar.GenQuest.LoanChest.TargetIdx) == sti(NPChar.index))
				{
					link.l21 = "Слушай, нам нужно поговорить о финансовых делах.";
					link.l21.go = "LoanForAll";//(перессылка в кредитный генератор)
				}
				link.l3 = pcharrepphrase("Есть ли в твоем клоповнике свободная конура?", "Нет ли у тебя свободной комнаты, я хотел бы остановиться здесь на некоторое время.");
				link.l3.go = "room";
				Link.l4 = "Увы, я уже ухожу, " + NPChar.name + ". До встречи.";
				Link.l4.go = "exit";
			}

		break;
		
		case "Meeting":
			dialog.Text = "О, вы сами в этом убедитесь, " + GetAddress_Form(NPChar) + ". Меня зовут " + GetFullName(npchar) + ". Я владелец этой таверны.";
			Link.l1 = "Мне нужна команда, " + NPChar.name + ".";
			Link.l1.go = "crew hire";
			Link.l2 = NPChar.name + ", давай поговорим...";
			Link.l2.go = "int_quests";
			if (CheckAttribute(pchar, "GenQuest.LoanChest.TakeChest") && sti(pchar.GenQuest.LoanChest.TargetIdx) == sti(NPChar.index))
			{
				link.l21 = "Слушай, нам нужно поговорить о финансовых делах.";
				link.l21.go = "LoanForAll";//(перессылка в кредитный генератор)
			}
			link.l3 = pcharrepphrase("Есть ли в твоем клоповнике свободная конура?", "Нет ли у тебя свободной комнаты, я хотел бы остановиться здесь на некоторое время.");
			link.l3.go = "room";
			Link.l4 = "Пожалуй, загляну к тебе попозже.";
			Link.l4.go = "exit";
		break;
		
		case "crew hire":
			ok = (rColony.from_sea == "") || (Pchar.location.from_sea == rColony.from_sea);
			ok = sti(Pchar.Ship.Type) != SHIP_NOTUSED && ok;
			
			if (!ok)
			{
				Dialog.text = "А на что тебе матросы? Что-то не вижу твоего корабля в порту.";
				link.l1 = RandPhraseSimple("Точно... я его пришвартовал не в том месте.", "Забыл войти в порт...");
				link.l1.go = "exit";
				break;
			}
			
            if (makeint(environment.time) > 22.0 || makeint(environment.time) < 7.0)
			{
				Dialog.text = "Обычно у меня в таверне полно людей, желающих стать матросами, но сейчас слишком поздно, и они начнут появляться только утром. Может быть, вы хотите снять комнату, и подождать их?";
				link.l1 = "Хорошо. У тебя есть свободные комнаты?";
				link.l1.go = "room";
				link.l2 = "Меня не интересует комната на данный момент. До встречи.";
				link.l2.go = "exit";
			}
			else
			{
                if (GetNationRelation2MainCharacter(sti(NPChar.nation)) == RELATION_ENEMY && sti(NPChar.nation) != PIRATE)
                {
                    Dialog.text = "А ты думаешь, к тебе кто-то пойдет, когда ты с нами во вражде? Скажи спасибо, что стражу не зову.";
					link.l1 = "Спасибо.";
					link.l1.go = "exit";
                }
                else
                {
					if (CheckAttribute(NPChar, "CrewHired.PGGId") && GetNpcQuestPastDayParam(NPChar, "CrewHired") < 3 && GetCrewQuantity(rColony) == 0)
					{
						Dialog.text = "Извините, но сейчас нет никого. Все ушли с отважным кэпом " + GetFullName(CharacterFromID(NPChar.CrewHired.PGGId)) + ".";
						link.l1 = "Эх, жаль!";
						link.l1.go = "exit";
					}
					else
					{
						DeleteAttribute(NPChar, "CrewHired"); //на всяк случай.
						NextDiag.CurrentNode =  NextDiag.TempNode;
						DialogExit();
						LaunchHireCrew();
					}
				}
			}
		break;
		
        case "int_quests":
			dialog.text = "Я весь внимание.";
			link.l1 = "Я ищу работу. Не поможешь мне?";
			link.l1.go = "work";
			//homo 15/06/06 слухи
			link.l2 = LinkRandPhrase("Расскажи мне последние новости?","Что новенького в этих краях?",
                                    "Как течет жизнь на суше?");
			link.l2.go = "rumours_tavern";
			//homo
			link.l3 = "Я по другому делу.";
			link.l3.go = "quests";//(перессылка в файл города)
			if (CheckAttribute(pchar, "GenQuest.SeekSpy.City")) //квест мэра поп поиску шпиона
			{
				link.l4 = "Я действую по поручению губернатора. Скажи мне, ты не замечал никого подозрительного в последнее время в городе?";
				if (pchar.GenQuest.SeekSpy.City == "0")
				{
					link.l4.go = "SeekSpy_Seen";
				}
				else
				{
					link.l4.go = "SeekSpy_NotSeen";
				}
			}
			link.l9 = "Спасибо, ничего. До свидания.";
			link.l9.go = "exit";
		break;
		
		case "work":
			if (sti(NPChar.nation) != PIRATE && GetNationRelation2MainCharacter(sti(NPChar.nation)) == RELATION_ENEMY)
			{
				dialog.text = RandPhraseSimple("Не хочу тебе помогать, ты ходишь под флагом " + NationNameGenitive(sti(pchar.nation)) + "!", "Хоть я и простой хозяин таверны, но сотрудничать с " + NationNameAblative(sti(pchar.nation)) + " я не буду.");
				link.l1 = RandPhraseSimple("Хм, твое право... Ладно, прощай.", "Ну, как знаешь...");
				link.l1.go = "exit";
				break;
			}
			dialog.text = "Работу, говоришь? Могу тебе посоветовать обратиться к местному торговцу - ему частенько нужны капитаны для перевозки груза\nЕще можешь посмотреть нет ли в таверне какого-нибудь купца - они часто ищут охрану для своих лоханок\nЕсли же не гнушаешься перевозкой пассжиров, то поспрашивай на улицах - возможно, что кому-нибудь надо добраться до другой колонии\nНу и напоследок могу посоветовать тебе поступить на службу к нашему губернатору. Уж он то будет снабжать тебя заданиями с завидной регулярностью.";
            if (sti(Pchar.Ship.Type) != SHIP_NOTUSED)
            {
				if (!CheckAttribute(npchar, "work_date") || GetNpcQuestPastDayParam(npchar, "work_date") >= 1 || bBettaTestMode)
    			{					
					SaveCurrentNpcQuestDateParam(npchar, "work_date");
					if (isBadReputation(pchar, 40))
					{
						dialog.text = RandPhraseSimple("Ты знаешь, людей, которым нужно попасть на другой остров, у меня в таверне много бывает. Только вот репутация у тебя, приятель, неважная... Не найти тебе идиота, что пойдет на твое на судно пассажиром.", "Как правило, у меня полно торговцев, которым нужно сопровождение. Но у тебя руки по локоть в крови! Ты всерьез думаешь, что с тобой хоть кто-нибудь согласится отправиться в плавание?");
						link.l1 = RandPhraseSimple("Эх, молва обо мне идет впереди меня! Хех!", "Ты прав, меня все боятся! Ха-ха!");
						link.l1.go = "exit";
						break;
					}
					switch (rand(1))
        			{
                        case 0: //конвой
                            if (!CheckQuestAttribute("generate_convoy_quest_progress", "begin"))
                			{
								if (GetCompanionQuantity(PChar) == COMPANION_MAX)
								{
									dialog.text = "Часто торговцы просят сопроводить их корабль до места назначения. Но если и есть такие, то они предпочитают плавать в составе меньшей эскадры. Ты же знаешь.";
									link.l1 = "Верно, я и забыл. Ладно, подойду позже, когда количество моих кораблей уменьшится, ха-ха.";
									link.l1.go = "exit";
								}
								else
								{
									if (rand(5) > 1)
									{
										dialog.text = "Ты вовремя ко мне обратился парень, тут один торговец как раз искал подходящую компанию... А-а-а, вот как раз и он, видишь, вошел в таверну? Поговори с ним.";
										link.l1 = RandPhraseSimple("Как ты думаешь, с ним можно иметь дело?", "А кто он такой? Думаешь, мне стоит с ним иметь дела?");
										link.l1.go = "ConvoyAreYouSure";
									}
									else
									{
										dialog.text = "Обычно у меня ошиваются торговцы, которым нужно сопровождение, но сейчас никого нет. Может быть, тебе повезет в другой день.";
										link.l1 = RandPhraseSimple("Ладно, на нет и суда нет.", "Хех, не свезло... Ну ладно, прощай.");
										link.l1.go = "exit";
									}
								}
                			}							
            			break;

            			case 1: // пассажир
							if (rand(6) > 1)
							{
								dialog.Text = "Хотел тут один тип найти корабль, чтобы добраться кое-куда. Да вот и он!";
								link.l1 = RandPhraseSimple("Как ты думаешь, этого человека можно брать на борт?", "А кто он такой? Думаешь, мне стоит браться за его доставку?");
								Link.l1.go = "PassangerAreYouSure";
							}
							else
							{
								dialog.text = "Пассажиров много у меня бывает, но сейчас, как назло, никого нет. Так что не обессудь...";
								link.l1 = RandPhraseSimple("Если нет никого, то чего уж...", "Хех, не свезло... Ну ладно, прощай.");
								link.l1.go = "exit";
							}
            			break;
        			}
    			}
    			else
    			{
					dialog.text = RandPhraseSimple("Ты уже спрашивал меня сегодня о работе.", "Мы же говорили уже о работе сегодня...");
					link.l1 = "Черт возьми, мне очень нужна работа, поэтому и спрашивал!";
                    link.l1.go = "SayAgain";
    			}
			}
			link.l2 = "Благодарю за помощь. Давай поговорим о чем-нибудь другом.";
			link.l2.go = "First time";
		break;

        case "SayAgain":
		    dialog.text = "Я же тебе уже все сказал! Мне нечего больше добавить!";
			link.l1 = "Ладно, ладно, все понял.";
			link.l1.go = "exit";
        break;

        case "ConvoyAreYouSure":
		    dialog.text = RandPhraseSimple("Конечно, это вполне нормальная работа.", "Почему нет, если сговоритесь в цене?");
			Link.l1 = "Спасибо, сейчас с ним и переговорю.";
			Link.l1.go = "exit";
			pchar.quest.destination = findTraderCity(npchar);
			AddDialogExitQuest("prepare_for_convoy_quest");
        break;

        case "PassangerAreYouSure":
		    dialog.text = RandPhraseSimple("Конечно, это вполне нормальный пассажир.", "Почему нет, если сговоритесь в цене?");
			Link.l1 = "Спасибо, сейчас с ним и переговорю.";
			Link.l1.go = "exit";
			sGlobalTemp = findPassangerCity(npchar);
			AddDialogExitQuest("prepare_for_passenger_quest");
        break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;

		case "room":
   			if (chrDisableReloadToLocation || CheckAttribute(pchar, "questTemp.different.Church_NightGuard")) //кто-то должен подойти к ГГ.
			{
				dialog.text = "Сейчас комната занята, " + GetAddress_Form(NPChar) + ", ничем не могу помочь.";
				link.l1 = "Жаль, очень жаль...";
				link.l1.go = "exit";
				break;
			}
			if (CheckAttribute(pchar, "questTemp.State.Open") && npchar.id == "Tortuga_tavernkeeper")//исп.линейка, квест №6, блокировка спать в комнате
			{
				dialog.text = "Вы заплатили мне за неделю вперед, я не могу брать у вас еще деньги. Комната ваша, идите и отдыхайте...";
				link.l1 = "Хорошо, приятель, спасибо.";
				link.l1.go = "exit";
				break;
			}
			//-->> квест официантки
			if (pchar.questTemp.different == "FackWaitress_toRoom")
			{
				dialog.text = "Вы хотите снять комнату? Не проблема. С вас 100 монет - и можете туда пройти.";
				if (sti(pchar.money) >= 100)
				{
					link.l1 = "Забирай свои деньги, открывай комнату, приятель...";
					link.l1.go = "exit";
					AddMoneyToCharacter(pchar, -100);
					LocatorReloadEnterDisable(npchar.city + "_tavern", "reload2_back", false);
					pchar.questTemp.different = "FackWaitress_toRoomUp";
				}
				else
				{
					link.l1 = "Хм, я, пожалуй, подойду попозже...";
					link.l1.go = "exit";
				}
				break;
			}
			if (pchar.questTemp.different == "FackWaitress_toRoomUp")//квест официантки
			{
				dialog.text = "Комната ваша, идите и отдыхайте...";
				link.l1 = "Хорошо, приятель, спасибо.";
				link.l1.go = "exit";
				break;
			}
			//<<-- квест официантки
			dialog.text = "На какой срок вы планируете остановиться?";
			if(!isDay())
			{
				link.l1 = "До утра.";
				link.l1.go = "room_day";
			}
			else
			{
				link.l1 = "До ночи.";
				link.l1.go = "room_night";
				link.l2 = "До следующего утра.";
				link.l2.go = "room_day_next";
			}
		break;

		case "room_day":
			dialog.text = "Это обойдется вам в 5 золотых.";
			if (makeint(pchar.money) >= 5)
			{
				link.l1 = "Идет. Вот твои деньги.";
				link.l1.go = "room_day_wait";
			}
			link.l2 = pcharrepphrase("Ты сдаешь королевские покои? Нет уж, за такую цену я лучше посижу в общей зале.",
            "Увы, видимо, такая роскошь, как сон на кровати не для моего кошелька. Я переночую в общей зале.");
			link.l2.go = "hall_day_wait";
			link.l3 = "Не сейчас. Давай поговорим о чем-нибудь другом.";
			link.l3.go = "int_quests";
		break;

		case "room_day_next":
			dialog.text = "Это обойдется вам в 10 золотых.";
			if (makeint(pchar.money) >= 10)
			{
				link.l1 = "Идет. Вот твои деньги.";
				link.l1.go = "room_day_wait_next";
			}
			link.l2 = pcharrepphrase("Ты сдаешь королевские покои? Нет уж, за такую цену я лучше посижу в общей зале.",
            "Увы, видимо, такая роскошь, как сон на кровати не для моего кошелька. Я переночую в общей зале.");
			link.l2.go = "hall_day_wait";
			link.l3 = "Не сейчас. Давай поговорим о чем-нибудь другом.";
			link.l3.go = "int_quests";
		break;

		case "room_night":
			dialog.text = "Это обойдется вам в 5 золотых.";
			if (makeint(pchar.money) >= 5)
			{
				link.l1 = "Идет. Вот твои деньги.";
				link.l1.go = "room_night_wait";
			}
			link.l2 = pcharrepphrase("Ты сдаешь королевские покои? Нет уж, за такую цену я лучше посижу в общей зале.",
            "Увы, видимо, такая роскошь, как сон на кровати не для моего кошелька. Я переночую в общей зале.");
			link.l2.go = "hall_night_wait";
			link.l3 = "Не сейчас. Давай поговорим о чем-нибудь другом.";
			link.l3.go = "int_quests";
		break;

		case "room_night_wait":
			DialogExit();
			NextDiag.CurrentNode = NextDiag.TempNode;
			AddMoneyToCharacter(pchar, -5);
			AddDialogExitQuest("sleep_in_tavern");
			TavernWaitDate("wait_night");
		break;

		case "hall_night_wait":
			if (rand(2) == 1)
            {
                Dialog.text = "Убирайся, оборванец! Нечего тут в общей зале просиживать за бесплатно.";
				link.l1 = "Ладно, ладно... уже ухожу.";
				link.l1.go = "exit";
            }
            else
            {
				DialogExit();
				NextDiag.CurrentNode = NextDiag.TempNode;
				TavernWaitDate("wait_night");
			}
		break;

		case "room_day_wait":
			DialogExit();
			NextDiag.CurrentNode = NextDiag.TempNode;
			AddMoneyToCharacter(pchar, -5);
			AddDialogExitQuest("sleep_in_tavern");
			TavernWaitDate("wait_day");
		break;

		case "room_day_wait_next":
			DialogExit();
			NextDiag.CurrentNode = NextDiag.TempNode;
			AddMoneyToCharacter(pchar, -10);
			AddDialogExitQuest("sleep_in_tavern");
			TavernWaitDate("wait_day");
		break;

		case "hall_day_wait":
			if (rand(1) == 1)
            {
                Dialog.text = "Убирайся, оборванец! Нечего тут в общей зале просиживать за бесплатно.";
				link.l1 = "Ладно, ладно... уже ухожу.";
				link.l1.go = "exit";
            }
            else
            {
				DialogExit();
				NextDiag.CurrentNode = NextDiag.TempNode;
				TavernWaitDate("wait_day");
			}
		break;
		
		case "tavern_keeper":
			NextDiag.TempNode = "First time";

			dialog.text = "Уважаемые, прошу вас прекратить это безобразие в моей таверне, или я позову стражников!";
			link.l1 = "Ладно, ладно, я уже ухожу.";
			link.l1.go = "exit_sit";
		break;

		case "exit_sit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
			AddDialogExitQuest("exit_sit");
		break;
		//==> eddy. квест мэра, вопросы не замечал ли шпиона
		case "SeekSpy_Seen":
			dialog.text = NPCStringReactionRepeat("Хм, дай подумать... Ты знаешь, есть один тип, который лично мне доверия не внушает. По виду обычный горожанин, но крутится то тут, то в городе шастает, вопросы у него странные. Сюда он вряд ли заявится - я его вчера отшил, а вот в городе поищи.", 
				"Мы уже говорили на эту тему, более мне сказать тебе нечего.", "Опять то же самое... Я уже все тебе сказал.", "Сколько можно спрашивать одно и то же?!", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat("Спасибо, тебе, приятель. Что же, пойду искать этого типа.", "Хм, ну извини...",
                      "Хм, да уж...", "Извини, приятель...", npchar, Dialog.CurrentNode);
			link.l1.go = "exit";
		break;
		case "SeekSpy_NotSeen":
			dialog.text = NPCStringReactionRepeat("Нет, никого не видел, чтобы у меня подозрение вызывал.", 
				"Мы уже говорили на эту тему, более мне сказать тебе нечего.", "Опять то же самое... Я уже все тебе сказал.", "Сколько можно спрашивать одно и то же?!", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat("Ну что же, и на этом спасибо, приятель.", "Хм, ну извини...",
                      "Хм, да уж...", "Извини, приятель...", npchar, Dialog.CurrentNode);
			link.l1.go = "exit";
		break;
		
		//==> homo линейка Блада
		case "TStep_0":
			dialog.text = "Вверх по лестнице. Я с тобой не разговаривал.";
			link.l1 = "Благодарю.";
			link.l1.go = "Exit";
			int n= FindLocation("Bridgetown_tavern");
            locations[n].reload.l2.disable = false;
		break;
		
		case "TStep_1":
			dialog.text = "Что за дело ко мне у Вас, уважаемый доктор?";
			link.l1 = "Дело житейское... мне бы не помешали, на всякий случай, несколько абордажных сабель и пара пистолетов, дорогой друг. Вы ведь можете это устроить или указать на человека, могущего нечто подобное провернуть? Вы же знаете, в магазин мне идти нет смысла...";
			link.l1.go = "TStep_2";
		break;
		
		case "TStep_2":
			dialog.text = "Ну, разумеется, нет.";
			link.l1 = "Не можете? Или нет смысла?";
			link.l1.go = "TStep_3";
            if (makeint(pchar.money) >= 2500)
            {
    			link.l1 = "Я разве не упомянул, что у меня для знающего человека найдётся премия в размере... ну, скажем, тысячи пиастров? Да и вспомните, как я лечил вашу дочь, не жалея своих сил и времени.";
    			link.l1.go = "TStep_4";
            }
		break;
		
		case "TStep_3":
			dialog.text = "Ничем не могу помочь, доктор.";
			link.l1 = "Всего наилучшего.";
			link.l1.go = "exit";
		break;
		
		case "TStep_4":
			dialog.text = "Конечно, у меня есть то, что Вам нужно, доктор Блад. Мистер Гриффин, которому Вы не так давно вправляли перелом, по жизни старый оружейник. Его дом находится неподалёку от поместья губернатора Стида - сходите к нему. Уверен, он не откажет Вам.";
			link.l1 = "Всего наилучшего.";
			link.l1.go = "exit";
			AddMoneyToCharacter(pchar, -1000);
			Pchar.questTemp.CapBloodLine.stat = "PrepareToEscape1";
			AddQuestRecord("WeaponsForEscape", "2");

		break;
	}

}

string findTraderCity(ref NPChar)
{
	int n, nation;
    int storeArray[MAX_COLONIES];
    int howStore = 0;

	for(n=0; n<MAX_COLONIES; n++)
	{
		nation = GetNationRelation(sti(npchar.nation), sti(colonies[n].nation));
		if (nation != RELATION_ENEMY && colonies[n].id != "Panama" && colonies[n].nation != "none" && GetIslandByCityName(npchar.city) != colonies[n].islandLable) //не на свой остров
		{
			storeArray[howStore] = n;
			howStore++;
		}
	}
	if (howStore == 0) return "none";
	nation = storeArray[rand(howStore-1)];
	return colonies[nation].id;
}

string findPassangerCity(ref NPChar)
{
	int n, nation;
    int storeArray[MAX_COLONIES];
    int howStore = 0;

	for(n=0; n<MAX_COLONIES; n++)
	{
		nation = GetNationRelation(sti(npchar.nation), sti(colonies[n].nation));
		if (colonies[n].nation != "none" && colonies[n].id != "Panama" && GetIslandByCityName(npchar.city) != colonies[n].islandLable) //не на свой остров
		{
			storeArray[howStore] = n;
			howStore++;
		}
	}
	if (howStore == 0) return "none";
	nation = storeArray[rand(howStore-1)];
	return colonies[nation].id;
}
