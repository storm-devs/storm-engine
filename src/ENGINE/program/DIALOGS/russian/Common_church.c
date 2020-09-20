// boal 25/04/04 общий диалог церковь
#include "DIALOGS\russian\Rumours\Common_rumours.c"  //homo 25/06/06
void ProcessDialogEvent()
{
	ref NPChar, sld;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

    // вызов диалога по городам -->
    NPChar.FileDialog2 = "DIALOGS\" + LanguageGetLanguage() + "\Church\" + NPChar.City + "_Church.c";
    if (LoadSegment(NPChar.FileDialog2))
	{
        ProcessCommonDialog(NPChar, Link, NextDiag);
		UnloadSegment(NPChar.FileDialog2);
	}
    // вызов диалога по городам <--
	ProcessCommonDialogRumors(NPChar, Link, NextDiag);//homo 16/06/06
    
    string iDay, iMonth, lastspeak_date;
    string sTemp, sTitle;

	iDay           = environment.date.day;
	iMonth         = environment.date.month;
	lastspeak_date = iday + " " + iMonth;

	if (!CheckAttribute(npchar, "quest.HealthMonth"))
    {
        npchar.quest.HealthMonth = "";
    }
    if (!CheckAttribute(npchar, "quest.BadMeeting"))
    {
        npchar.quest.BadMeeting = "";
    }
    if (!CheckAttribute(npchar, "quest.GhostShipMonth"))
    {
        npchar.quest.GhostShipMonth = "";
    }
	switch(Dialog.CurrentNode)
	{
		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
		
		case "fight":
			DialogExit();
			NextDiag.CurrentNode = NextDiag.TempNode;
			LAi_group_Attack(NPChar, Pchar);
		break;
		
		case "First time":
			if (LAi_grp_playeralarm > 0)
			{
       			dialog.text = NPCharRepPhrase(pchar, 
					LinkRandPhrase("Сын мой, к сожалению, я не могу дать тебе убежища. Беги!", "Вся городская стража рыщет по городу в поисках тебя, сын мой. Церковь открыта для страждущих, но я не могу предоставить тебе убежища...", "Сын мой, тебе надо бежать! Прошу тебя, поторопись!"), 
					LinkRandPhrase("Что ты делаешь в церкви, падшая душа? Я требую, чтобы ты удалился немедленно, пока солдаты не ворвались и не устроили здесь резню!", "Немедленно покинь храм, безбожник! Я не заступлюсь за такого, как ты!", "Немедленно покинь храм Божий, еретик! Мне не нужны убийства здесь!!"));
				link.l1 = NPCharRepPhrase(pchar,
					RandPhraseSimple("Да ладно тебе, падре...", "Хех, мне не нужна мне твоя помощь..."), 
					RandPhraseSimple("Хех, святой отец, лучше заткнись...", "Уйду, не бойся, падре..."));
				link.l1.go = "fight";
				break;
			} 
            if (npchar.quest.BadMeeting != lastspeak_date)
			{
				Dialog.Text = "Добро пожаловать в лоно церкви, сын мой.";
				link.l1 = "Здравствуйте, святой отец.";
				link.l1.go = "node_3";
				Link.l2 = "Ты мне не отец, и не смей так больше обращаться.";
				Link.l2.go = "node_2";
			}
			else
			{
				Dialog.Text = "Покиньте храм Господа. Нам не о чем сегодня более говорить.";
				Link.l1 = "Ну и не нужно!";
				Link.l1.go = "exit";
			}
			NextDiag.TempNode = "First time";
		break;
		
		case "node_2":
			dialog.text = "Ах ты, богохульник! А ну прочь отсюда, дабы не осквернять своим присутствием дом Бога!";
			link.l1 = "Да я и так не собирался здесь задерживаться.";
			link.l1.go = "exit";
			ChangeCharacterReputation(pchar, -0.25);
			npchar.quest.BadMeeting = lastspeak_date;
		break;

		case "node_3":
			dialog.text = "Да благословит Господь вас и дела ваши... Вы пришли ко мне с какой-то целью?";
   			//homo Линейка Блада
            if (Pchar.questTemp.CapBloodLine != true )
            {
    			link.l1 = RandPhraseSimple("Я хочу внести пожертвования.", "Хочу пожертвовать на благое дело.");
    			link.l1.go = "donation";
    			link.l2 = RandPhraseSimple("Думаю, настало время исповеди.","Мне нужно покаяться, "+RandPhraseSimple("падре.", "святой отец."));
    			link.l2.go = "ispoved";
    			link.l3 = RandPhraseSimple("У меня к вам дело, " + RandPhraseSimple("падре.", "святой отец."),
                                           "Я прибыл по делу, " + RandPhraseSimple("падре.", "святой отец."));
    			link.l3.go = "work";//"quest lines";
            }

			link.l6 = "Извините, мне пора идти.";
			link.l6.go = "exit";
		break;

		case "donation":
			dialog.Text = "Конечно, сын мой. Сколько ты желаешь пожертвовать Святой Церкви?";
			Link.l1 = "Простите, святой отец, но пожертвования не будет.";
			Link.l1.go = "No donation";
			if(makeint(PChar.money)>=100)
			{
				Link.l2 = "К сожалению, немного. 100 пиастров.";
				Link.l2.go = "donation paid_100";
			}
			if(makeint(PChar.money)>=1000)
			{
				Link.l3 = "1000 пиастров. Я думаю, этого хватит.";
				Link.l3.go = "donation paid_1000";
			}
			if(makeint(PChar.money)>=5000)
			{
				Link.l4 = "Мне везет с деньгами, поэтому я пожертвую 5000 пиастров.";
				Link.l4.go = "donation paid_5000";
			}
			//-->> квест пожертвования хозяйки борделя
			if(pchar.questTemp.different == "HostessChurch_toChurch" && pchar.questTemp.different.HostessChurch.city == npchar.city && sti(pchar.money) >= sti(pchar.questTemp.different.HostessChurch.money))
			{
				Link.l5 = "Святой отец, я хочу сделать пожертвование не от себя. Я делаю это по поручению.";
				Link.l5.go = "HostessChurch";
			}
			//<<-- квест пожертвования хозяйки борделя
		break;

		case "No donation":
			dialog.Text = "Хм... Это ваше решение, и вы вправе его изменить. Но не забывайте о том, что все это зачтется вам после смерти.";
			Link.l1 = "Мне еще далеко до смерти! Давайте лучше сменим тему.";
			Link.l1.go = "node_3";
			Link.l2 = "Извините, святой отец, мне пора.";
			Link.l2.go = "exit";
		break;

		case "donation paid_100":
			AddMoneyToCharacter(pchar, -100);
			pchar.questTemp.donate = makeint(pchar.questTemp.donate) + 100;
			dialog.Text = "От лица Святой Церкви благодарю вас, сын мой, за ваш дар.";
			Link.l1 = "Мне надо поговорить с вами, святой отец.";
			Link.l1.go = "node_3";
			Link.l2 = "Извините, святой отец, мне пора.";
			Link.l2.go = "exit";
			AddDialogExitQuest("donation");
		break;

		case "donation paid_1000":
			AddMoneyToCharacter(pchar, -1000);
			pchar.questTemp.donate = makeint(pchar.questTemp.donate) + 1000;
			dialog.Text = "От лица Святой Церкви благодарю вас, сын мой, за ваш дар.";
            Link.l1 = "Мне надо поговорить с вами, святой отец.";
			Link.l1.go = "node_3";
			Link.l2 = "Извините, святой отец, мне пора.";
			Link.l2.go = "exit";
			AddDialogExitQuest("donation");
		break;

		case "donation paid_5000":
			AddMoneyToCharacter(pchar, -5000);
			pchar.questTemp.donate = makeint(pchar.questTemp.donate) + 5000;
			dialog.Text = "От лица Святой Церкви благодарю вас, сын мой, за ваш дар.";
            Link.l1 = "Мне надо поговорить с вами, святой отец.";
			Link.l1.go = "node_3";
			Link.l2 = "Извините, святой отец, мне пора.";
			Link.l2.go = "exit";
			AddDialogExitQuest("donation");
		break;

		case "ispoved":
			dialog.text = "Конечно же, сын мой, я тебя слушаю.";
			link.l1 = "Я передумал. Может быть в другой раз.";
			link.l1.go = "exit";
			link.l2 = "Я так грешен, святой отец...";
			link.l2.go = "ispoved_1";
		break;

		case "ispoved_1":
			dialog.text = "Все мы грешны.";
			link.l1 = "Да, но я... Я топил и грабил корабли, я убивал...";
			link.l1.go = "ispoved_2";
		break;

		case "ispoved_2":
			dialog.text = "Но ведь среди них не было тех, кто плавал под флагом " + NationNameGenitive(sti(NPChar.nation)) + "?";
			link.l1 = "Нет, конечно, святой отец.";
			link.l1.go = "ispoved_3";
			link.l2 = "Были и такие, святой отец.";
			link.l2.go = "ispoved_4";
		break;

		case "ispoved_3":
			dialog.text = "Тогда Господь прощает тебе твои грехи.";
			link.l1 = "Спасибо, святой отец, я пойду.";
			link.l1.go = "exit";
		break;

		case "ispoved_4":
			dialog.text = "Господь никогда не простит тебе этот грех!";
			link.l1 = "Прощайте, святой отец.";
			link.l1.go = "exit";
			npchar.quest.BadMeeting = lastspeak_date;
		break;

        case "work":
			dialog.text = "Какое дело привело тебя, сын мой.";
			link.l1 = "Я хотел бы задать вам один вопрос, падре...";
			link.l1.go = "quests"; // ссылка к НПС
            link.l2 = RandPhraseSimple("Я хочу поговорить о работе на благо церкви " + NationNameGenitive(sti(NPChar.nation)) + ".",
                                        "Как идут дела в приходе, не нужна ли какая помощь?");
			link.l2.go = "prihod";
			if (CheckAttribute(pchar, "GenQuest.LoanChest.TakeChest") && sti(pchar.GenQuest.LoanChest.TargetIdx) == sti(NPChar.index))
			{
	            link.l3 = RandPhraseSimple("Святой отец", "Падре") + ", я хотел бы поговорить с вами о финансовых вопросах."; //(пересылка в кредиты)
	            link.l3.go = "LoanForAll";
            }
			// -->
            if (stf(pchar.Health.HP) < 60.0)
            {
                link.l4 = "Я нуждаюсь в исцелении.";
                link.l4.go = "healthAdd_1";
            }
            //==> Вставка Эдди, квест Аззи, возможность сдачи лампы.
            if (CheckCharacterItem(pchar, "Azzy_bottle"))
            {
                link.l6 = "Святой Отец, у меня есть вещь, которую я хочу отдать вам. Это странный жестяной сосуд, посмотрите.";
                link.l6.go = "GiveLamp";
            }
			//<== Вставка Эдди, квест Аззи возможность сдачи лампы.
			//квест мэра - на связь с нашим шпионом
			if (CheckAttribute(pchar, "GenQuest.Intelligence") && pchar.GenQuest.Intelligence.SpyId == npchar.id && pchar.GenQuest.Intelligence == "")
			{
	            link.l7 = RandPhraseSimple("Святой отец", "Падре") + ", я здесь по поручению одного человека. Его зовут губернатор " + GetFullName(characterFromId(pchar.GenQuest.Intelligence.MayorId)) + ".";
	            link.l7.go = "IntelligenceForAll";
            }
			//==> успокоить нечисть в пещерах
			if (CheckAttribute(npchar, "DestroyGhost") && npchar.DestroyGhost == "GoodResult")
			{
				switch (npchar.quest.DestroyGhost.label)
				{
					case "пещера": sTemp = "пещера очищена"; break;
					case "грот": sTemp = "грот очищен"; break;
					case "подземелье": sTemp = "подземелье очищено"; break;
				}
				link.l5 = "Святой отец, " + sTemp + " от нечисти. Дело сделано, теперь жителей никто не побеспокоит.";
                link.l5.go = "GhostShip_S1";
			}
			//==> ночной сторож в церкви
			if (CheckAttribute(pchar, "questTemp.different.Church_NightGuard") && pchar.questTemp.different.Church_NightGuard == npchar.location)
			{
	            link.l8 = RandPhraseSimple("Святой отец", "Падре") + ", я хочу отчитаться о поручении поработать в качестве ночного сторожа.";
	            link.l8.go = "NightGuard_result";
            }
			//<== ночной сторож в церкви
			link.l99 = "Я передумал, меня ждут дела.";
			link.l99.go = "exit";
		break;

		case "prihod":
			//упокоение нечисти
			if (rand(2)==1 && pchar.questTemp.different == "free" && !CheckAttribute(npchar, "quest.DestroyGhost") && GetNpcQuestPastDayWOInit(npchar, "Ghost") > 7)
			{
                dialog.text = "Мне не совсем удобно об этом говорить, но у меня нет другого выхода. В жизни нет случайных вещей. И на всем лежит воля Всевышнего. Может быть, ты и есть то оружие Господне, которое необходимо нам для истребления нечисти, оскверняющей нашу землю.";
        		link.l1 = RandPhraseSimple("Мне не совсем понятно, святой отец, что вас так напугало? Чем вызван этот страх?",
                                            "Я прошу вас, продолжайте дальше, святой отец.");
        		link.l1.go = "DestroyGhost_1";
				SaveCurrentNpcQuestDateParam(npchar, "Ghost");
				break;
			}
			//караулить церковь ночью
			if (rand(4)==1 && pchar.questTemp.different == "free" && !CheckAttribute(npchar, "quest.GuardNightChurch") && !CheckAttribute(pchar, "questTemp.different.Church_NightGuard") && GetNpcQuestPastDayWOInit(npchar, "NightChurch") > 60 && GetHour() < 23)
			{
                dialog.text = "Сын мой, у меня есть для тебя несложное поручение. Нам нужен ночной сторож, но не старик с колотушкой и не беззащитная прихожанка. Все дело в том, что в последнее время 'братья' немного нервничают. Какие-то странные типы стали заходить в церковь, но не молятся, а только глазеют по сторонам...";
        		link.l1 = "И это вас беспокоит, святой отец?";
				link.l1.go = "NightChurch_1";
				SaveCurrentNpcQuestDateParam(npchar, "NightChurch");
				break;
			}
            dialog.text = "У нас в приходе все спокойно, сын мой. Спасибо за предложение.";
    		link.l1 = "Ну, раз все хорошо, я пойду по своим делам.";
    		link.l1.go = "exit";
		break;
		//--->> успокоить нечисть
		case "DestroyGhost_1":
			npchar.quest.DestroyGhost = "toCave"; //личный флаг священника на взятый квест
			//получим пещеру для чистки
			sTemp = GetArealByCityName(npchar.city);
			if (sTemp == "Cuba2") sTemp = "Cuba1";
			aref aPlace, aPlace_2;
			makearef(aPlace, NullCharacter.TravelMap.Islands.(sTemp).Treasure);
			int iQty = GetAttributesNum(aPlace)-1;
			aPlace_2 = GetAttributeN(aPlace, rand(iQty));
			npchar.quest.DestroyGhost.locationId = GetAttributeName(aPlace_2); //Id целевой пещеры
			sld = &locations[FindLocation(npchar.quest.DestroyGhost.locationId)];
			npchar.quest.DestroyGhost.label = GetConvertStr(sld.id.label, "LocLables.txt"); //тип подземелья для диалогов
			switch (npchar.quest.DestroyGhost.label)
			{
				case "пещера": sTemp = "Дело в том, что пещера, находящаяся возле нашего города, стала прибежищем нечисти!"; break;
				case "грот": sTemp = "Дело в том, что грот, находящийся возле нашего города, стал прибежищем нечисти!"; break;
				case "подземелье": sTemp = "Дело в том, что подземелье, находящееся возле нашего города, стало прибежищем нечисти!"; break;
			}			
			dialog.text = "Все больше и больше людей рассказывают о страшных происшествиях. " + sTemp + " Оттуда на свет Божий лезут такие твари, что и в страшном сне не приснятся. Люди напуганы, страх проникает в их сердца...";
			link.l1 = "Чем я могу вам помочь, святой отец?";
			link.l1.go = "DestroyGhost_2";
		break;
		case "DestroyGhost_2":
			dialog.text = "Вы - смелый человек. Станьте орудием Господа в моих руках! Уничтожьте нечисть! И даровано вам будет отпущение грехов ваших...";
			link.l1 = "Я готов, падре!";
			link.l1.go = "DestroyGhost_3";
			link.l2 = "Святой отец, я не могу принять ваше предложение. Как-нибудь в другой раз.";
			link.l2.go = "DestroyGhost_out";
		break;
		case "DestroyGhost_out":
			dialog.text = "Плохо, сын мой, очень плохо. Я надеялся на тебя!";
			link.l1 = "Я думаю, все уладится со временем...";
			link.l1.go = "exit";
			DeleteAttribute(npchar, "quest.DestroyGhost");
			ChangeCharacterReputation(pchar, -3);
		break;
		case "DestroyGhost_3":
			dialog.text = "Очень хорошо, сын мой. Отправляйся туда, и да пребудет с тобой сила!";
			link.l1 = "Все будет в порядке, святой отец.";
			link.l1.go = "exit";
			pchar.questTemp.different = "DestroyGhost";
			SetTimerFunction("SmallQuests_free", 0, 0, 1); //освобождаем разрешалку на миниквесты 
			//флаг квеста на локацию, сюда же пишем индекс дающего священника
			sld = &locations[FindLocation(npchar.quest.DestroyGhost.locationId)];
			sld.DestroyGhost = npchar.index; 
			sTitle = sld.id + "Church_DestroyGhost";
			AddQuestRecordEx(sTitle, "Church_DestroyGhost", "1");
			AddQuestUserDataForTitle(sTitle, "sLocation", GetStrSmallRegister(npchar.quest.DestroyGhost.label));
			AddQuestUserDataForTitle(sTitle, "sCity", XI_ConvertString("Colony" + npchar.city + "Gen"));
			AddQuestUserData(sTitle, "sCity", XI_ConvertString("Colony" + npchar.city + "Gen"));
			AddQuestUserData(sTitle, "sLocation", npchar.quest.DestroyGhost.label);
		break;

		case "GhostShip_S1":
			dialog.text = "Отлично, сын мой! Ты сделал богоугодное дело. Знай, что отношение к тебе " + NationNameGenitive(sti(NPChar.nation)) + " станет лучше.";
			link.l1 = "Я счастлив, падре...";
			link.l1.go = "GhostShip_S2";
		break;
		case "GhostShip_S2":
			dialog.text = "Я буду молиться за тебя. Иди сын мой, и будь честен перед своей совестью и Господом нашим.";
			link.l1 = "Хорошо, святой отец.";
			link.l1.go = "exit";
			switch (npchar.quest.DestroyGhost.label)
			{
				case "пещера": sTemp = "пещеру"; break;
				case "грот": sTemp = "грот"; break;
				case "подземелье": sTemp = "подземелье"; break;
			}
			AddSimpleRumourCity(LinkRandPhrase("Наш святой отец, " + GetFullName(npchar) + ", рассказал о том, что вы очистили от нечисти " + sTemp + " близ нашего города. Богоугодное дело, скажу я вам...", 
				"Вы слышали, некий капитан по просьбе нашего святого отца очистил от демонов " + sTemp + " рядом с нашим городом. Каков храбрец!", 
				"Капитан, это вы очистили " + sTemp + " рядом с нашим городом от нечисти? Знайте, что мы все вам очень благодарны!"), npchar.city, 10, 1, "");
			ChangeCharacterReputation(pchar, 5);
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), 15);
			AddCharacterExpToSkill(pchar, "Leadership", 30);
			sld = &locations[FindLocation(npchar.quest.DestroyGhost.locationId)];
			sTitle = sld.id + "Church_DestroyGhost";
			AddQuestRecordEx(sTitle, "Church_DestroyGhost", "3");
			CloseQuestHeader(sTitle);
			DeleteAttribute(npchar, "quest.DestroyGhost");
		break;
        //<<--- успокоить нечисть
		//--->> ночной сторож
		case "NightChurch_1":
			dialog.text = "Именно так. И поверь мне, что это неспроста. Когда происходят такие вещи, то это является верным признаком грядущего ограбления. Судя по всему, кому-то приглянулась церковная утварь\nНу что, ты согласен провести несколько ночей в церкви?";
			link.l1 = "Нет, пожалуй, откажусь, святой отец. Прошу меня извинить.";
			link.l1.go = "exit";
			link.l2 = "В принципе, я согласен. Но сколько ночей мне надо будет охранять церковь? Может, никто и не появится, а ваши странные типы - обычные головорезы, которые просто стесняются на людях проявлять свои религиозные чувства.";
			link.l2.go = "NightChurch_2";
		break;
		case "NightChurch_2":
			dialog.text = "Хм, может и так, но это маловероятно. В общем, тебе нужно будет находиться здесь три ночи. Если никто так и не появится, то будем считать, что твоя работа выполнена успешно.";
			link.l1 = "Что же, это меня устраивает. Я согласен!";
			link.l1.go = "NightChurch_3";
		break;
		case "NightChurch_3":
			dialog.text = "Тогда приступай с сегодняшней ночи. И помни, что ты должны находится в церкви безотлучно с полуночи до шести утра.";
			link.l1 = "Ясно, святой отец. Три ночи можете быть спокойны за свою утварь.";
			link.l1.go = "exit";
			pchar.questTemp.different = "Church_NightGuard";
			SetTimerFunction("SmallQuests_free", 0, 0, 1); //освобождаем разрешалку на миниквесты 
			pchar.questTemp.different.Church_NightGuard = npchar.location; //флаг срабатывания нуля часов
			pchar.questTemp.different.Church_NightGuard.NightQty = 0; //кол-вот отработанных ночей
			pchar.questTemp.different.Church_NightGuard.chance = rand(3); //рендом на появление грабителей
			pchar.questTemp.different.Church_NightGuard.fight = false; //флаг боевки
			SaveCurrentQuestDateParam("questTemp.different.Church_NightGuard"); //запишем дату
			sTitle = npchar.location + "Church_NightGuard";
			AddQuestRecordEx(sTitle, "Church_NightGuard", "1");
			AddQuestUserDataForTitle(sTitle, "sCity", XI_ConvertString("Colony" + npchar.city + "Gen"));
			AddQuestUserData(sTitle, "sCity", XI_ConvertString("Colony" + npchar.city + "Gen"));
		break;

		case "NightGuard_result":
			if (sti(pchar.questTemp.different.Church_NightGuard.NightQty) == 0)
			{
				dialog.text = "Сын мой, ты еще не сторожил ни одной ночи. Иди, выполняй свою работу, не мешай мне...";
				link.l1 = "Хорошо, святой отец.";
				link.l1.go = "exit";
			}
			if (sti(pchar.questTemp.different.Church_NightGuard.NightQty) > 0 && sti(pchar.questTemp.different.Church_NightGuard.NightQty) < 3)
			{
				dialog.text = "Сын мой, ты не выполнил работу до конца, мы договаривались о трех ночах...";
				link.l1 = "Да, я помню... Продолжу сторожить.";
				link.l1.go = "exit";
			}
			if (sti(pchar.questTemp.different.Church_NightGuard.NightQty) >= 3)
			{
				if (sti(pchar.questTemp.different.Church_NightGuard.fight))
				{
					dialog.text = "Сын мой, мне уже известно о том, что ты сумел поймать церковных воров. И хоть убийство в стенах храма Господня - грех, но Господь отпускет тебе его.";
					link.l1 = "Спасибо, святой отец...";
					link.l1.go = "NightGuard_fight";
				}
				else
				{
					dialog.text = "Сын мой, ты честно продежурил в церкви по ночам. Ничего странного не видел?";
					link.l1 = "Нет, святой отец, все было спокойно. Никого не видел вообще.";
					link.l1.go = "NightGuard_noLook";
				}
			}
		break;

		case "NightGuard_noLook":
			dialog.text = "Хм, похоже, мы ошибались... Ну что же, прекрасный результат! В качестве награды я не предлагаю тебе денег, но могу сказать, что грехи твои перед Господом нашим и " + NationNameAblative(sti(npchar.nation)) + " будут уменьшены.";
			link.l1 = "Спасибо, святой отец.";
			link.l1.go = "exit";
			sTitle = pchar.questTemp.different.Church_NightGuard + "Church_NightGuard";
			CloseQuestHeader(sTitle);
			DeleteAttribute(pchar, "questTemp.different.Church_NightGuard");
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), 5);
			AddSimpleRumourCity(LinkRandPhrase("Говорят, что вы дежурите в нашей церкви по ночам...", 
				"Ночное дежурство в церкви - дело богоугодное. Спасибо вам.", 
				"Наш святой отец, " + GetFullName(npchar) + ", упомянул, что вы несли караул по ночам в нашей церкви."), npchar.city, 10, 1, "");
		break;
		case "NightGuard_fight":
			dialog.text = "Помимо этого, я в силах уменьшить твои грехи перед " + NationNameAblative(sti(npchar.nation)) + ", что и сделаю. К тому же, я буду молиться за твое здоровье, и, полагаю, скоро ты почувствуешь исцеляющую силу молитвы Господу нашему!";
			link.l1 = "Огромное спасибо, святой отец!";
			link.l1.go = "exit";
			sTitle = pchar.questTemp.different.Church_NightGuard + "Church_NightGuard";
			CloseQuestHeader(sTitle);
			DeleteAttribute(pchar, "questTemp.different.Church_NightGuard");
			ChangeCharacterNationReputation(pchar, sti(NPChar.nation), 5);
			AddCharacterMaxHealth(pchar, 2);
			AddSimpleRumourCity(LinkRandPhrase("Говорят, что вы ночью вы поймали бандитов в нашей церкви! Огромное вам спасибо!", 
				"Вы знаете, все жители города благодарны вам за то, что вы не допустили разорение нашей церкви. Спасибо вам!", 
				"Наш святой отец, " + GetFullName(npchar) + ", только о том и говорит, как вы спасли нашу церковную утварь той ночью..."), npchar.city, 10, 1, "");
		break;

		//<<--- ночной сторож

        case "healthAdd_1":
            if (GetHealthMaxNum(pchar) == 6) // отлично
            {
                dialog.text = "Здоровье твое " + GetHealthNameMaxSmall(pchar) + ", а раны заживут сами. Нужно просто избегать кровопролитных стычек.";
    			link.l1 = "Обнадежили. Постараюсь избегать насилия некоторое время.";
    			link.l1.go = "exit";
            }
            else
            {
    			dialog.text = "Здоровье твое " + GetHealthNameMaxSmall(pchar) + ", и хоть раны заживут сами, тебе не помешает его поправить.";
    			link.l1 = "Что для этого нужно?";
    			link.l1.go = "healthAdd_2";
			}
			link.l2 = "А почему вообще мое здоровье портится?";
    		link.l2.go = "healthAdd_info";
		break;

		case "healthAdd_info":
			dialog.text = "Очень просто, сын мой, ты получаешь урон, когда тебя ранят. Это не проходит бесследно, здоровье твое подрывается каждой раной, и в результате ты становишься слаб и фехтовать уже не в силах.\n Команда твоя, видя слабость вожака, теряет к тебе уважение. Но зато ты делаешься более осторожным и умение защиты временно увеличивается. Своевременный отдых и минимизация ранений помогут оставаться всегда полным сил.";
			link.l1 = "Спасибо, вы меня просветили.";
			link.l1.go = "exit";
		break;

		case "healthAdd_2":
			dialog.text = "Молитвы и отказ от насилия, сын мой. Тогда Господь вернет тебе утерянное тобой.";
			link.l1 = "А вы не могли бы молится за меня?";
			link.l1.go = "healthAdd_3";
			link.l2 = "То есть никак? Хм... обнадежили, спасибо.";
			link.l2.go = "exit";
		break;

		case "healthAdd_3":
            if (npchar.quest.HealthMonth != iMonth)
            {
    			dialog.text = "Я? Пожалуй, это возможно. Но тебе необходимо внести пожертвование Святой Церкви.";
    			link.l1 = pcharrepphrase("Сколько?", "С радостью! Каков его размер?");
    			link.l1.go = "healthAdd_4";
    			link.l2 = "Я уж как-нибудь так...";
    			link.l2.go = "exit";
			}
			else
			{
                dialog.text = "Сын мой, я уже молюсь о твоем благополучии. Молитвы займут все мое время до конца месяца.";
    			link.l1    = "Спасибо, "+RandPhraseSimple("падре.", "святой отец.");
    			link.l1.go = "exit";
			}
		break;

		case "healthAdd_4":
			dialog.text = "Сто тысяч золотом будет вполне достаточной платой за чудо исцеления.";
			link.l1 = pcharrepphrase("Сколько?!! Да вы сумасшедший. Я заплачу такую сумму, когда разграблю пару церквей.", "Для меня это слишком много. Видно, правду говорят - здоровье за деньги не купишь.");
			link.l1.go = "exit";
            if (sti(pchar.Money) >= 100000)
            {
    			link.l2 = "Я согласен!";
    			link.l2.go = "healthAdd_5";
			}
		break;

		case "healthAdd_5":
            AddMoneyToCharacter(pchar, -100000);
            AddCharacterMaxHealth(pchar, 4); // можно не проверять, тк явно меньше 51
            npchar.quest.HealthMonth = iMonth;
			dialog.text = "Хорошо. Твое здоровье немного улучшится. Но тебе нужно избегать стычек и беречь себя, иначе сила молитвы сойдет на нет. Мне нужно целый месяц провести в молитвах о твоем бренном теле.";
			link.l1 = "Спасибо. Если что, я зайду через месяц снова.";
			link.l1.go = "exit";
		break;
		// диалог для капелата из тюряги
		case "Prison_Temp":
            NextDiag.TempNode = "Prison_Temp";
			dialog.text = "Нам не о чем более говорить.";
			link.l1 = "Да и не нужно.";
			link.l1.go = "Exit";
		break;

		case "Prison_Cell":
            NextDiag.TempNode = "Prison_Temp";
			dialog.text = "Заблудшая душа, не хочешь ли ты покаяться в грехах своих, очистить душу и облегчить участь свою?";
			link.l1 = "А вы, собственно, кто?";
			link.l1.go = "Prison_Cell_Info";
			link.l2 = "С радостью и смирением, падре.";
			link.l2.go = "Prison_Cell_Talk";
			link.l3 = "Убирайся прочь, святоша.";
			link.l3.go = "Prison_Cell_None";
		break;

		case "Prison_Cell_Info":
			dialog.text = "Я местный капеллан, облегчаю участь узников, исповедую до и отпеваю после исполнения смертного приговора или напутствую перед долгой дорогой на рудники.";
			link.l1 = "А другой выбор, кроме каторги и расстрела, бывает?";
			link.l1.go = "Prison_Cell_Info_2";
			link.l2 = "Я готов покаяться, падре.";
			link.l2.go = "Prison_Cell_Talk";
			link.l3 = "Я совершенно не виновен. Мне это все не нужно.";
			link.l3.go = "Prison_Cell_None";
		break;

		case "Prison_Cell_Info_2":
			dialog.text = "Конечно, сын мой, не все доживают до вынесения приговора, многим казематы остаются вечным приютом. Готов ли ты поведать о своих грехах?";
			link.l2 = "Я готов покаяться, падре.";
			link.l2.go = "Prison_Cell_Talk";
			link.l3 = "Я совершенно не виновен. Мне это все не нужно.";
			link.l3.go = "Prison_Cell_None";
		break;

		case "Prison_Cell_None":
			dialog.text = "Упорство в ереси и отступничество никогда не приведут тебя в Царствие Божие. Охолонись и покайся!";
			link.l1 = "Я настаиваю, мне это не нужно. Подите прочь!";
			link.l1.go = "Prison_Cell_death";//"Exit";
			//AddDialogExitQuest("MoveToPrison_Cell");
		break;

		case "Prison_Cell_Talk":
			dialog.text = "Я слушаю тебя, сын мой.";
			link.l2 = "Я грешен, падре, многие злодеяния на моем счету, я грабил и убивал невинных, но действовал я не один. Мне помогали верные соратники. Вы можете передать им от меня весточку?";
			link.l2.go = "Prison_Cell_Talk_2";
			link.l3 = "Я передумал. Оставьте меня.";
			link.l3.go = "Prison_Cell_None";
		break;

		case "Prison_Cell_Talk_2":
			dialog.text = "Так ты еще и главарь преступников? И просишь меня пособничать твоим коварным планам? Не бывать этому! Я немедля буду настаивать на твоем приговоре.";
			link.l1 = "Ну и дурак же ты, а еще рясу нацепил!";
			link.l1.go = "Prison_Cell_death";
		break;

		case "Prison_Cell_death":
			DialogExit();
			NextDiag.CurrentNode = NextDiag.TempNode;
   			sld = &Characters[sti(PChar.GenQuest.ToPrisonCmdIdx)];
			// to_do DeathExecutionStart(Pchar, sti(sld.nation));
		break;
		//квест пожертвования хозяйки борделя
		case "HostessChurch":
			dialog.text = "И кто же эта добрая душа, что жертвует?";
			link.l1 = "Хм... Это хозяйка местного борделя.";
			link.l1.go = "HostessChurch_call";
			link.l2 = "Я бы предпочел не называть имен. Ведь может же быть анонимное пожертвование!";
			link.l2.go = "HostessChurch_notCall";
			pchar.questTemp.different = "HostessChurch_return"; //флаг на возвращение
		break;
		case "HostessChurch_call":
			if (isBadReputation(pchar, 5)) 
			{
				if (rand(100) < GetCharacterSkill(pchar, "Fortune")) //рендом вешаем на удачу
				{
					dialog.text = "Сам грешник, еще за грешницу и просишь? О своей душе подумал бы лучше, ведь все мы перед Господом предстанем!";
					link.l1 = "Это точно. Надеюсь, Господь будет милостив ко мне... Так как насчет пожертвования?";
					link.l1.go = "HostessChurch_ok";
				}
				else
				{
					dialog.text = "Изыди, сатана!!! Забирай свои нечестивые деньги и убирайся отсюда!";
					link.l1 = "Святой отец, как же так?! Мы к вам со всей душой, а вы...";
					link.l1.go = "HostessChurch_bad";
				}
			}
			else
			{
				if (rand(10) < GetCharacterSPECIAL(pchar, "Charisma")) //рендом вешаем на харизму
				{
					dialog.text = "Я рад, что ты помогаешь грешникам обрести путь к Господу нашему. За это тебе воздастся на небесах!";
					link.l1 = "Это было бы здорово! Так что насчет пожертвования?";
					link.l1.go = "HostessChurch_ok";
				}
				else
				{
					dialog.text = "Я не могу принять деньги, добытые во грехе. Верни их этой блуднице, сын мой. Через деньги пути к Господу не отыскать.";
					link.l1 = "Очень жаль, что вы отвергаете ее. Очень жаль...";
					link.l1.go = "HostessChurch_bad_1";
				}
			}
		break;
		case "HostessChurch_notCall":
			if (rand(10) < GetCharacterSPECIAL(pchar, "Charisma"))
			{
				dialog.text = "Взять эти деньги, даже не зная, от кого они...";
				link.l1 = "Да, падре, именно так. Примите пожертвование от чистого сердца!";
				link.l1.go = "HostessChurch_ok_1";
			}
			else
			{
				dialog.text = "Взять эти деньги, даже не зная, от кого они? Ты с ума сошел?! А вдруг, на этих деньгах - кровь!!";
				link.l1 = "Святой отец, на всех деньгах - кровь...";
				link.l1.go = "HostessChurch_bad_2";
			}
		break;

		case "HostessChurch_ok":
			dialog.text = "Я принимаю его, сын мой. Пойди, можешь передать это блуднице.";
			link.l1 = "Хорошо, падре. Спасибо.";
			link.l1.go = "exit";
			AddMoneyToCharacter(pchar, -sti(pchar.questTemp.different.HostessChurch.money));
			AddCharacterExpToSkill(pchar, "Fortune", 20);
			//флаг удачная или неудачная попытка дачи
			characters[GetCharacterIndex(pchar.questTemp.different.HostessChurch.city + "_Hostess")].questChurch = "yes";
		break;
		case "HostessChurch_ok_1":
			dialog.text = "Я принимаю его, сын мой. Можешь передать подающему.";
			link.l1 = "Хорошо, падре. Спасибо.";
			link.l1.go = "exit";
			AddMoneyToCharacter(pchar, -sti(pchar.questTemp.different.HostessChurch.money));
			AddCharacterExpToSkill(pchar, "Fortune", 20);
			//флаг удачная или неудачная попытка дачи
			characters[GetCharacterIndex(pchar.questTemp.different.HostessChurch.city + "_Hostess")].questChurch = "yes";
		break;
		case "HostessChurch_bad":
			dialog.text = "Убирайся, исчадие ада!!! И не смей показываться мне на глаза!";
			link.l1 = "Хех, ну хорошо.";
			link.l1.go = "exit";
			AddCharacterExpToSkill(pchar, "Leadership", 50);
			npchar.quest.BadMeeting = lastspeak_date;
			//флаг удачная или неудачная попытка дачи
			characters[GetCharacterIndex(pchar.questTemp.different.HostessChurch.city + "_Hostess")].questChurch = "no";
		break;
		case "HostessChurch_bad_1":
			dialog.text = "Таково мое решение. Иди с миром, сын мой.";
			link.l1 = "Прощайте, падре.";
			link.l1.go = "exit";
			AddCharacterExpToSkill(pchar, "Leadership", 30);
			//флаг удачная или неудачная попытка дачи
			characters[GetCharacterIndex(pchar.questTemp.different.HostessChurch.city + "_Hostess")].questChurch = "no";
		break;
		case "HostessChurch_bad_2":
			dialog.text = "Это ты так думаешь, нечестивец! Убирайся из храма и не показывайся мне на глаза!";
			link.l1 = "Так? Да пожалуйста, уйду...";
			link.l1.go = "exit";
			AddCharacterExpToSkill(pchar, "Leadership", 40);
			npchar.quest.BadMeeting = lastspeak_date;
			//флаг удачная или неудачная попытка дачи
			characters[GetCharacterIndex(pchar.questTemp.different.HostessChurch.city + "_Hostess")].questChurch = "no";
		break;

	}
}
