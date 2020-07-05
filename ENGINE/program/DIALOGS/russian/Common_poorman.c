#include "DIALOGS\russian\Rumours\Common_rumours.c"
void ProcessDialogEvent()
{
	ref NPChar, sld;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

    // вызов диалога по городам -->
    /*NPChar.FileDialog2 = "DIALOGS\" + LanguageGetLanguage() + "\Poorman\" + NPChar.City + "_Poorman.c";
    if (LoadSegment(NPChar.FileDialog2))
	{
        ProcessCommonDialog(NPChar, Link, NextDiag);
		UnloadSegment(NPChar.FileDialog2);
	}*/
    // вызов диалога по городам <--

	ProcessCommonDialogRumors(NPChar, Link, NextDiag);//homo 25/06/06

	switch(Dialog.CurrentNode)
	{
		case "First time":
			if (npchar.quest.meeting == "0")
			{
				//если ГГ начал мочить бродяг -->
				if (pchar.questTemp.LSC == "PoorMurderBegin")
				{
					dialog.text = LinkRandPhrase("Я слышал, что ты убивал нищих в других городах. Зачем ты это делаешь?", 
						"Мне кажется, что я знаю кто ты. Зачем ты убиваешь нас?", 
						"Что тебе нужно от нас? Что мы тебе сделали? Зачем убивать безобидных нищих?");
					link.l1 = LinkRandPhrase("Все дело в деньгах...", "Мне пообещали за эту работу оч-ч-чень большое вознаграждение!", "Такова жизнь... А вообще, все дело в корысти, друг мой.");
					link.l1.go = "exit";
					break;
				}
				//<-- если ГГ начал мочить бродяг
				dialog.text = LinkRandPhrase("Здраствуйте. Меня зовут " + GetFullName(npchar) + ". Не думаю, что вам было бы приятно со мной познакомиться, но, возможно, вы запомните мое имя...", 
					"Приветствую вас, " + GetAddress_Form(NPChar) +  ". Меня зовут " + GetFullName(npchar) + ".", 
					"Мое имя - " + GetFullName(npchar) + ", " + GetAddress_Form(NPChar) + ". Рад с вами познакомиться.");
				link.l1 = RandPhraseSimple("Приветствую.", "Здраствуйте.");
				link.l1.go = "First time";
				npchar.quest.meeting = "1";
			}			
			else
			{
				//если ГГ начал мочить бродяг -->
				if (pchar.questTemp.LSC == "PoorMurderBegin")
				{
					dialog.text = LinkRandPhrase("Я слышал, что ты убивал нищих в других городах. Зачем ты это делаешь?", 
						"Мне кажется, что я знаю кто ты. Зачем ты убиваешь нас?", 
						"Что тебе нужно от нас? Что мы тебе сделали? Зачем убивать безобидных нищих?");
					link.l1 = LinkRandPhrase("Все дело в деньгах...", "Мне пообещали за эту работу оч-ч-чень большое вознаграждение!", "Такова жизнь... А вообще, все дело в корысти, друг мой.");
					link.l1.go = "exit";
					break;
				}	
				//<-- если ГГ начал мочить бродяг
				dialog.text = NPCStringReactionRepeat("Эх, вот видишь, до чего докатился...", 
					"Жить подоянием непросто...", 
					"Хотелось бы мне вырваться из нищеты!",
					"Опять ты?..", "block", 1, npchar, Dialog.CurrentNode);
				link.l1 = HeroStringReactionRepeat("Вижу. Ну, ничего страшного.", 
					"Оно и понятно. На эти деньги не разгуляешься, поди...",
					"Тогда тебе нужно не протирать здесь штаны, а что-то делать для этого.", 
					"Ага. Неужели надоел?", npchar, Dialog.CurrentNode);
				link.l1.go = DialogGoNodeRepeat("exit", "exit", "exit", "pester", npchar, Dialog.CurrentNode);
				link.l2 = RandPhraseSimple("Что тебе нужно?", "Чего ты хочешь?");
				link.l2.go = "Whants";
				link.l3 = LinkRandPhrase("Что-нибудь интересное мне расскажешь?", 
					"Что нового в городе?", "Эх, с удовольствием послушал бы последние сплетни...");
				link.l3.go = "rumours_poor";
				// --------------- линейка ГПК ---------------
				//разговарием с нищим о наезде
				if (pchar.questTemp.LSC == "SignOnPoorMurder" || pchar.questTemp.LSC == "PoorIsGood")
				{
					if (CheckAttribute(pchar, "questTemp.LSC.qtyTalk.headPoormanId") && pchar.questTemp.LSC.qtyTalk.headPoormanId == npchar.id)
					{
						link.l1 = "Я слышал, что ты хотел поговорить со мной.";
						link.l1.go = "PoorHeadMan"; 
						npchar.quest.talk = true; //больше не говорить
						break;
					}
					if (!CheckAttribute(npchar, "quest.talk"))
					{
						if (!CheckAttribute(pchar, "questTemp.LSC.qtyTalk")) pchar.questTemp.LSC.qtyTalk = 0;
						pchar.questTemp.LSC.qtyTalk = sti(pchar.questTemp.LSC.qtyTalk) + 1; //инкремент на базары по душам
						if (sti(pchar.questTemp.LSC.qtyTalk) > 4) pchar.questTemp.LSC.qtyTalk = 4;
						switch (pchar.questTemp.LSC.qtyTalk)
						{
						case "1":
							link.l1 = "Послушай, мне очень интересно, нищие в городах - банда головорезов, что ли?";
							npchar.quest.talk = true; //больше не говорить
							break;
						case "2":
							link.l1 = "Слушай, у меня к тебе есть довольно необычное дело.";
							npchar.quest.talk = true; //больше не говорить
							break;
						case "3":
							link.l1 = "Знаешь, у меня к тебе есть один вопрос. Очень странный, на первый взгляд...";
							npchar.quest.talk = true; //больше не говорить
							break;
						case "4":
							link.l1 = "Хочу задать тебе один вопрос, приятель.";
							break;
						}
						link.l1.go = "GoodTalk_" + pchar.questTemp.LSC.qtyTalk;
					}
				}
				//нашел письмо в сундуке Виллемстада
				if (pchar.questTemp.LSC == "CanFoundStuvesantKey" && CheckCharacterItem(pchar, "letter_LSC") && pchar.questTemp.LSC.qtyTalk.headPoormanId == npchar.id)
				{
					link.l1 = "Послушай, мне кажется, что я нашел в резиденции Стювесанта нечто весьма интересное!";
					link.l1.go = "FoundLetter";
				}
				//базар о том, что нашел причину заказа на нищих
				if (pchar.questTemp.LSC == "readyGoLSC" && pchar.questTemp.LSC.qtyTalk.headPoormanId == npchar.id)
				{
					link.l1 = "Ты знаешь, мне удалось распутать этот клубок!";
					link.l1.go = "GoLSC";
				}
			}
			NextDiag.TempNode = "First time";
		break;
		case "pester":
			dialog.text = RandPhraseSimple("Да нет, почему сразу надоел? Я не особо занят, как ты сам видишь...", 
				"Не надоел, но порядком утомил. Я хоть и нищий, но тоже человек.");
			link.l1 = "Хех, ясно...";
			link.l1.go = "exit";
		break;
		//выходы
		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
		//милостыня
		case "Whants":
			if (!CheckAttribute(npchar, "wants_date") || GetNpcQuestPastDayParam(npchar, "wants_date") >= 1 || bBettaTestMode)
    		{
				dialog.text = PCharRepPhrase("Хм, " + GetAddress_Form(NPChar) +  ", о вас ходят нелицеприятные слухи. Но я верю в людей, даже в таких как вы. Я прошу дать мне денег на еду и питье, сжальтесь.",
					"Прошу вас, " + GetAddress_Form(NPChar) +  "! Не обойдите вниманием несчастного человека, подайте на пропитание...");
				link.l1 = RandPhraseSimple("Ничего тебе не дам.", "Обойдешься.");
				link.l1.go = "exit";
				Link.l2 = "Хорошо. И сколько тебе нужно денег?";
				Link.l2.go = "Whants_1";
			}
			else
			{
				dialog.text = PCharRepPhrase("От тебя? Ничего.", "Ничего не нужно, " + GetAddress_Form(NPChar) + ", спасибо.");
				link.l1 = RandPhraseSimple("Хм, ну ладно.", "Знать хорошо живется тебе, приятель.");
				link.l1.go = "exit";
			}
		break;
		case "Whants_1":
			dialog.text = "И от гроша ломаного не откажусь, " + GetAddress_Form(NPChar) + ". Выбирайте сами, сколько можете мне подать...";
			Link.l1.edit = 2;			
			link.l1 = "";
			link.l1.go = "Whants_2";
		break;
		case "Whants_2":
			SaveCurrentNpcQuestDateParam(npchar, "wants_date");
			int iTemp = sti(dialogEditStrings[2]);
			if (iTemp <= 0 || sti(pchar.money) < iTemp)
			{
				dialog.text = "Грешно смеяться надо больными людьми...";
				link.l1 = "Ха-ха-ха, а ты что думал, юродивый, я тебе денег дам?!";
				link.l1.go = "exit";
				ChangeCharacterReputation(pchar, -2);
				break;
			}
			if (iTemp > 0 && iTemp <= 100)
			{
				dialog.text = "Спасибо за " + FindRussianMoneyString(iTemp) + ", " + GetAddress_Form(NPChar) + ". Я куплю хлеба на эти деньги...";
				link.l1 = "Давай, иди подкрепись, бродяга.";
				link.l1.go = "exit";
				pchar.money = sti(pchar.money) - iTemp;
			}
			if (iTemp > 100 && iTemp <= 1000)
			{
				dialog.text = "Спасибо вам, " + GetAddress_Form(NPChar) + ". Этих денег мне хватит на неделю!";
				link.l1 = "Рад был помочь.";
				link.l1.go = "exit";
				OfficersReaction("good");
				pchar.money = sti(pchar.money) - iTemp;
			}
			if (iTemp > 1000 && iTemp <= 5000)
			{
				dialog.text = "Благодарю вас, " + GetAddress_Form(NPChar) + ". Я всем расскажу о вашей доброте!";
				link.l1 = "Ну, это не обязательно...";
				link.l1.go = "exit";
				ChangeCharacterReputation(pchar, sti(iTemp/2000+0.5));
				pchar.money = sti(pchar.money) - iTemp;
			}
			if (iTemp > 5000 && iTemp <= 50000)
			{
				dialog.text = "Спасибо, вам, досточтимый " + GetAddress_Form(NPChar) + ". Да хранит вас Господь...";
				link.l1 = "Да, покровительство Господа мне не помешает уж точно!";
				link.l1.go = "exit";
				ChangeCharacterNationReputation(pchar, sti(NPChar.nation), sti(iTemp/10000+0.5));
				pchar.money = sti(pchar.money) - iTemp;
			}
			if (iTemp > 50000 && iTemp <= 100000)
			{
				dialog.text = "Спасибо, вам, досточтимый " + GetAddress_Form(NPChar) + ". Желаю вам удачи!";
				link.l1 = "Спасибо, приятель.";
				link.l1.go = "exit";
				AddCharacterExpToSkill(pchar, "Leadership", sti(iTemp/5000));
				AddCharacterExpToSkill(pchar, "Fortune", sti(iTemp/5000));
				pchar.money = sti(pchar.money) - iTemp;
			}
			if (iTemp > 100000)
			{
				dialog.text = "Послушайте, " + GetAddress_Form(NPChar) + ", вы в своем уме? Вы отдаете мне " + FindRussianMoneyString(iTemp) + "! Я не возьму такие деньги, чувствую, здесь какой-то подвох... Уходите!";
				link.l1 = "Ну, как знаешь...";
				link.l1.go = "exit";
				ChangeCharacterReputation(pchar, -2);
			}
		break;
		//линейка ГПК, разговоры по душам с нищими после заказа 
		case "GoodTalk_1":
			dialog.text = "К-хе, к-хе... Чего?!!";
			link.l1 = "Ну-у-у, как тебе сказать... В общем, есть тут люди, которым вы как кость в горле.";
			link.l1.go = "GoodTalk_1_1";
		break;
		case "GoodTalk_1_1":
			dialog.text = "Я даже не знаю, что и сказать... Сам наверное понимаешь, что мы люди бездомные, отношение аристократии к нам пренебрежительное.";
			link.l1 = "Настолько, чтобы желать вашей смерти?";
			link.l1.go = "GoodTalk_1_2";
		break;
		case "GoodTalk_1_2":
			dialog.text = "Смерти?! Не-е-ет, это вряд ли. Послушай, что за странные речи ты ведешь? Меня это пугает.";
			link.l1 = "Хех, и правильно делает, что пугает. В общем, ты не знаешь, кто могут быть ваши доброжелатели, я так понимаю.";
			link.l1.go = "GoodTalk_1_3";
		break;
		case "GoodTalk_1_3":
			dialog.text = "Не знаю, конечно! Ты же с ними общался.";
			link.l1 = "Общался я через представителя, так сказать. Меня интересует, кто за этим стоит... Ну ладно, будь осторожен, друг.";
			link.l1.go = "GoodTalk_1_4";
		break;
		case "GoodTalk_1_4":
			dialog.text = "Да уже теперь постараюсь, хоть возможностей у меня к этому немного.";
			link.l1 = "Понимаю. Но я тебя предупредил.";
			link.l1.go = "exit";
		break;

		case "GoodTalk_2":
			dialog.text = "Ну, давай, излагай суть.";
			link.l1 = "Понимаешь, мне бы очень хотелось знать, кому вы напакостили настолько, что вас просто видеть уже не могут?";
			link.l1.go = "GoodTalk_2_1";
		break;
		case "GoodTalk_2_1":
			dialog.text = "Видеть не могут? А кого это - нас?";
			link.l1 = "Всех нищих в Карибском море.";
			link.l1.go = "GoodTalk_2_2";
		break;
		case "GoodTalk_2_2":
			dialog.text = "Даже не знаю, что и сказать... И что, очень серьезно напакостили?";
			link.l1 = "Ага. Настолько, что желают вашей смерти.";
			link.l1.go = "GoodTalk_2_3";
		break;
		case "GoodTalk_2_3":
			dialog.text = "Хо-о-о, вот это да!.. Ну ты сам, наверное, понимаешь, что вреда от нас для окружающих - вонь да блохи. Если ради этого кто-то готов нас убивать, то я вообще ничего не понимаю в этой жизни!";
			link.l1 = "Вот я и говорю, что мне непонятна мотивация такой ненависти.";
			link.l1.go = "GoodTalk_2_4";
		break;
		case "GoodTalk_2_4":
			dialog.text = "Извини, но мне она тоже совершенно неясна. Я в полной растерянности...";
			link.l1 = "Понятно. Ну что же, извини, что отвлек от дел.";
			link.l1.go = "GoodTalk_2_5";
		break;
		case "GoodTalk_2_5":
			dialog.text = "Да ничего. Я бы не сказал, что так уж сильно занят...";
			link.l1 = "Ну, тем не менее.";
			link.l1.go = "exit";
		break;

		case "GoodTalk_3":
			dialog.text = "Слушаю тебя, спрашивай.";
			link.l1 = "Я тут столкнулся с довольно неожиданным обстоятельством... В общем, дело в том, что в Карибском море есть люди, очень недовольные вашим братом.";
			link.l1.go = "GoodTalk_3_1";
		break;
		case "GoodTalk_3_1":
			dialog.text = "Ага, я знаю, кто ты!";
			link.l1 = "Хм, ну и кто я?";
			link.l1.go = "GoodTalk_3_2";
		break;
		case "GoodTalk_3_2":
			dialog.text = "Ты тот самый капитан, что выясняет, кто нас ненавидит и, якобы, желает нашей смерти. Слухи уже пошли среди нашего брата.";
			link.l1 = "Да, это я и есть. Но я говорю правду без всяких 'якобы'. И я бы порекомендовал вам воспринимать все это серьезно - большие деньги предлагаются за эту работу.";
			link.l1.go = "GoodTalk_3_3";
		break;
		case "GoodTalk_3_3":
			dialog.text = "Как бы то ни было, с тобой на эту тему хочет поговорить один из нас.";
			link.l1 = "И кто это?";
			link.l1.go = "GoodTalk_3_4";
		break;
		case "GoodTalk_3_4":
			pchar.questTemp.LSC.qtyTalk.headPoormanId = GetPoorHeadManId(npchar);
			sld = characterFromId(pchar.questTemp.LSC.qtyTalk.headPoormanId);
			dialog.text = "Его зовут " + GetFullName(sld) + ", он бродяжничает в " + XI_ConvertString("Colony" + sld.city + "Dat") + ".";
			link.l1 = "Понятно. Ну что же, пожалуй, навещу его при случае.";
			link.l1.go = "exit";
			AddQuestRecord("ISS_PoorsMurder", "3");
			AddQuestUserData("ISS_PoorsMurder", "sCity", XI_ConvertString("Colony" + sld.city + "Acc"));
			AddQuestUserData("ISS_PoorsMurder", "sName", GetFullName(sld));
		break;

		case "GoodTalk_4":
			dialog.text = "Я слушаю тебя.";
			link.l1 = "Как ты думаешь, кто может быть исчезновении нищих отсюда?";
			link.l1.go = "GoodTalk_4_1";
		break;
		case "GoodTalk_4_1":
			sld = characterFromId(pchar.questTemp.LSC.qtyTalk.headPoormanId);
			dialog.text = "Я знаю тебя, но ответить на твой вопрос не могу. Тебе нужно идти в " + XI_ConvertString("Colony" + sld.city + "Acc") + ", там найдешь одного из наших по имени " + GetFullName(sld) + ". Он хотел с тобой переговорить по этому поводу.";
			link.l1 = "Понятно. Я так и поступлю.";
			link.l1.go = "exit";
		break;
		//основной бродяга
		case "PoorHeadMan":
			dialog.text = "Если ты капитан " + GetFullName(pchar) + ", то так и есть.";
			link.l1 = "Именно так меня и зовут.";
			link.l1.go = "PoorHeadMan_1";
		break;
		case "PoorHeadMan_1":
			dialog.text = "Ну что же, это хорошо. Насколько я смог понять, ты пытаешься узнать о людях, которым мы здорово досадили. Причем настолько сильно, что они готовы нас убивать. Все правильно?";
			link.l1 = "Ну да. Добавлю только, что не убивать, а нанять убийцу.";
			link.l1.go = "PoorHeadMan_2";
		break;
		case "PoorHeadMan_2":
			dialog.text = "Хм, это существенный фактор в деле... А как звали того человека, что говорил с тобой об этом?";
			link.l1 = "Оливер Траст.";
			link.l1.go = "PoorHeadMan_3";
		break;
		case "PoorHeadMan_3":
			dialog.text = "Это имя мне незнакомо... В общем, есть у меня одно соображение.";
			link.l1 = "Излагай.";
			link.l1.go = "PoorHeadMan_4";
		break;
		case "PoorHeadMan_4":
			dialog.text = "Была одна история в Виллемстаде, на Кюрасао, которая, по моему мнению, имеет некоторое сходство с тем, что произошло с тобой. В общем, у нас был один сотоварищ по несчастию, который решил бомжевать у голландцев, на Кюрасао. Звали его Тизер Дэн.\n"+
				"Так вот, прожил он там около месяца, а по прошествии этого времени попался на глаза генерал-губернатору Петеру Стэвезанту. С тех пор начались у Тизера проблемы то с солдатами в городе, то с комендантом... В итоге выгнали его из Виллемстада, и больше о нем ничего не известно.";
			link.l1 = "Хм, и какое это отношение имеет к моему делу?";
			link.l1.go = "PoorHeadMan_5";
		break;
		case "PoorHeadMan_5":
			dialog.text = "Слушай дальше. Начали мы узнавать, что к чему с Тизером, куда он пропал, и выяснили кое-что о Стэвезанте. Об этом мало кто знает, но Стэвезант - самый крупный поставщик рабов в Новый свет из Африки!\n"+
				"Он работает в тесном сотрудничестве с Голландской Вест-Индской компанией, и они такие дела крутят - закачаешься.";
			link.l1 = "Ну и что?";
			link.l1.go = "PoorHeadMan_6";
		break;
		case "PoorHeadMan_6":
			dialog.text = "А то, что почти все наши - бывшие рабы. Ты думаешь мы бездельники и лентяи? Ничего подобного, просто по стечению обстоятельств нам довелось пройти через этот ад и потерять здоровье. Белым здесь часто дают свободу, так как нужны колонисты\n"+
				"Но далеко не каждый бывший раб может пахать землю и корчевать пни после месяцев, проведенных в трюме галеона по дороге сюда. А Стэвезант, судя по всему, очень не любит бывших рабов, которые так и не занялись ничем полезным.";
			link.l1 = "Хм, звучит все это несколько надуманно...";
			link.l1.go = "PoorHeadMan_7";
		break;
		case "PoorHeadMan_7":
			dialog.text = "Возможно, но это все, что я знаю и могу предположить. Других ненавистников нищих, столь же богатых и влиятельных, я не знаю. Так что если ты хочешь что-то узнать об этом деле, то, мне кажется, искать нужно в резиденции Виллемстада.";
			link.l1 = "И что я должен там искать по твоему мнению?";
			link.l1.go = "PoorHeadMan_8";
		break;
		case "PoorHeadMan_8":
			dialog.text = "Бумаги, друг мой, деловую переписку. Все, что я тебе тут наговорил, действительно надумано. Мне кажется, что за предложением этого Траста стоит что-то гораздо более весомое, нежели обычная неприязнь кого бы то ни было.";
			link.l1 = "Хм, я тоже так думаю. Что-то здесь серьезное, учитывая тотальный охват замысла и сумму гонорара.";
			link.l1.go = "PoorHeadMan_9";
		break;
		case "PoorHeadMan_9":
			dialog.text = "Ну, я сказал все, что хотел. Спасибо за то, что выслушал. Если найдешь что-нибудь стоящее внимания - сразу обращайся ко мне, будем думать... И спасибо за то, что не причинил нам вреда.";
			link.l1 = "Да не за что. Тебе спасибо за информацию.";
			link.l1.go = "exit";
			pchar.questTemp.LSC = "toVillemstadResidence";
			AddQuestRecord("ISS_PoorsMurder", "4");
			//сработает прерывание, чтобы показать геймеру, где нужный сундук
			pchar.quest.LSC_enterStuvesantBox.win_condition.l1 = "locator";
			pchar.quest.LSC_enterStuvesantBox.win_condition.l1.location = "Villemstad_TownhallRoom";
			pchar.quest.LSC_enterStuvesantBox.win_condition.l1.locator_group = "box";
			pchar.quest.LSC_enterStuvesantBox.win_condition.l1.locator = "private2";
			pchar.quest.LSC_enterStuvesantBox.function = "LSC_enterStuvesantBox";
		break;
		//базар с основным после нахождения письма
		case "FoundLetter":
			dialog.text = "Ну давай, посмотрим, что там у тебя...";
			link.l1 = "Вот письмо, которе я раздобыл в резиденции Стэвезанта. Это переписка между ним и Голландской Вест-Индской компанией. Из него явно следует, что затеяла уничтожение нищих именно данная торговая компания.";
			link.l1.go = "FoundLetter_1";
		break;
		case "FoundLetter_1":
			dialog.text = "Угу, вижу...";
			link.l1 = "Там какой-то остров упоминается. И предметы роскоши. И все это почему-то на бродягах зациклено. Расскажи-ка мне, приятель, в чем тут дело.";
			link.l1.go = "FoundLetter_2";
			TakeItemFromCharacter(pchar, "letter_LSC");
		break;
		case "FoundLetter_2":
			dialog.text = "Хм, даже не знаю, как сказать... Полагаю, что упоминаемый остров - это мифический Остров Справедливости, столь популярный среди нищих.";
			link.l1 = "Чего?";
			link.l1.go = "FoundLetter_3";
		break;
		case "FoundLetter_3":
			dialog.text = "Ну, бродяги, понимаешь сам, люди обездоленные - разбитые судьбы, поломанные жизни. И хоть нет ни здоровья, ни жилья, ни денег, но хочется верить во что-то хорошее в будущем. Большинство наших верят в существование так называемого Острова Справедливости, зачастую это вера спасает им жизнь.";
			link.l1 = "Расскажи подробней об этом Острове. Все, что знаешь и о чем говорят у вас.";
			link.l1.go = "FoundLetter_4";
		break;
		case "FoundLetter_4":
			dialog.text = "Говорят, что где-то в округе есть Остров, но не природного происходения. Так вот, на этом острове царит закон и справедливость, жизнь там благоприятствует хорошему человеку. Сам-то я в это мало верю, на земле нет справедливости...";
			link.l1 = "Хм, а ведь в письме указано, что этот Остров существует! Сам читай: 'Выясняется, что слухи об острове имеют под собой реальную почву...'";
			link.l1.go = "FoundLetter_5";
		break;
		case "FoundLetter_5":
			dialog.text = "Ну, не занаю...";
			link.l1 = "А с чего вообще слухи-то пошли? С чего все началось?";
			link.l1.go = "FoundLetter_6";
		break;
		case "FoundLetter_6":
			dialog.text = "Так с Тизера Дэна и началось. Он немного не в себе был - пережил несколько кораблекрушений. Ему все наши говорили - не лезь в Кюрасао, а он все одно решил там жить... В общем, он утверждал, что жил на этом Острове насколько месяцев, вот и наболтал сказок.";
			link.l1 = "Хм, а Голландская Вест-Индская компания полагает, что не таких уж и сказок... А где сейчас этот Тизер, говоришь?";
			link.l1.go = "FoundLetter_7";
		break;
		case "FoundLetter_7":
			dialog.text = "Пропал. Мы его разыскать не сумели. Исчез без следа.";
			link.l1 = "Ты, помнится, говорил, что в Виллемстаде у него проблемы начались. Из города его выгоняли, вроде бы...";
			link.l1.go = "FoundLetter_8";
		break;
		case "FoundLetter_8":
			dialog.text = "Да-да, именно так. После того, как он попался на глаза Стэвезанту.";
			link.l1 = "Судя по всему, мне опять надо брать курс на Кюрасао...";
			link.l1.go = "FoundLetter_9";
		break;
		case "FoundLetter_9":
			dialog.text = "Ну что же, удачи тебе в этом деле! Спасибо за помощь, дружище.";
			link.l1 = "Будь здоров, приятель.";
			link.l1.go = "exit";
			pchar.questTemp.LSC = "toTalkStuvesant";
			AddQuestRecord("ISS_PoorsMurder", "7");
			AddQuestUserData("ISS_PoorsMurder", "sName", GetFullName(npchar));
		break;
		//базар с основным после завала Оливера Траста
		case "GoLSC":
			dialog.text = NPCStringReactionRepeat("Отлично! Ну, расскажи, в чем все дело?", 
				"Мы уже говорили на эту тему.", 
				"Хм, пообщались уже...",
                "Послушай, долгие расставания - это для женщин.", "block", 0, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat("Знаешь, ты оказался прав в предположении, которые высказал в самом начале нашего общения. Все дело действительно в Тизере Дэне и его Острове.", 
				"Да, верно.",
                "Точно!", 
				"Хм, ты прав...", npchar, Dialog.CurrentNode);
			link.l1.go = DialogGoNodeRepeat("GoLSC_1", "", "", "", npchar, Dialog.CurrentNode);
		break;
		case "GoLSC_1":
			dialog.text = "Поясни, пожалуйста!";
			link.l1 = "Я пока не могу много говорить. Но скажу, что Остров Тизера, о котором он так много рассказывал - не выдумка. Это правда, и я намерен отправится туда!";
			link.l1.go = "GoLSC_2";
		break;
		case "GoLSC_2":
			dialog.text = "Поверить не могу!..";
			link.l1 = "Есть и очень плохая новость - Тизер Дэн мертв.";
			link.l1.go = "GoLSC_3";
		break;
		case "GoLSC_3":
			dialog.text = "А-а, черт! Жаль...";
			link.l1 = "Я хоть никогда и не видел Тизера раньше, но мне стало понятно, что это был за человек. Действительно жаль.";
			link.l1.go = "GoLSC_4";
		break;
		case "GoLSC_4":
			dialog.text = "Значит, ты попытаешься найти этот Остров? Тогда я отдаю тебе ключ Тизера.";
			link.l1 = "Что за ключ?";
			link.l1.go = "GoLSC_5";
		break;
		case "GoLSC_5":
			dialog.text = "Точно я не знаю. Тизер отдал мне его на хранение до того, как отправился на Кюрасао. Говорил, что ключ Диффиндура, а что это означает - понятия не имею.";
			link.l1 = "Загадочный ключ! Интересно...";
			link.l1.go = "GoLSC_6";
		break;
		case "GoLSC_6":
			dialog.text = "По его наказу я должен отдать этот ключ тому, кто захочет найти Остров. Еще Тизер говорил, что этот ключ должен решить проблемы нового гражданина Острова. Что это означает - понятия не имею, но с удовольствием выполняю просьбу.";
			link.l1 = "Спасибо, дружище. И прощай.";
			link.l1.go = "exit";
			ChangeItemName("keyQuestLSC", "itmname_keyQuestLSC_Tizer");
			ChangeItemDescribe("keyQuestLSC", "itmdescr_keyQuestLSC_Tizer");
			sld = ItemsFromID("keyQuestLSC");
			DeleteAttribute(sld, "shown");
			GiveItem2Character(pchar, "keyQuestLSC");			
		break;
	}
}

string GetPoorHeadManId(ref NPChar)
{
    ref ch;
	int n;
    string storeArray[30];	locations[n].private9.key = "key3";
	locations[n].private9.items.jewelry1 = 30;
    int howStore = 0;
	for(n=0; n<MAX_CHARACTERS; n++)
	{
		makeref(ch,Characters[n]);
        if (CheckAttribute(ch, "Dialog.Filename") && ch.Dialog.Filename == "Common_poorman.c" && NPChar.id != ch.id && !CheckAttribute(ch, "quest.talk"))
		{
            storeArray[howStore] = ch.id;
            howStore++;
		}
    }
    if (howStore == 0) return "none";
    else return storeArray[rand(howStore-1)];
}
