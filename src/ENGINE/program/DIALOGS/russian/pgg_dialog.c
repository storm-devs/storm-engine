#include "DIALOGS\russian\Common_Duel.c"
void ProcessDialogEvent()
{
	int i, n;
	int iRnd = -1;
	bool bOk;
   	float locx, locy, locz;
	string sTmp, sLoc;
	ref NPChar, sld;
	aref link, NextDiag;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

    ProcessDuelDialog(NPChar, link, NextDiag);

	if (Dialog.CurrentNode == "First time" && CheckAttribute(NPChar, "meeting")) Dialog.CurrentNode = "Second time";
	switch(Dialog.CurrentNode)
	{
//	Dialog.Text = "Вы видите баг... сообщите о нем АТ, для navy.";
//	link.l1 = "Обязательно!";
//	link.l1.go = "exit.";
	case "First time":
		if (CheckAttribute(NPChar, "PGGAi.location.town.back") && CheckAttribute(pchar, "questTemp.PGGContra.Know"))
		{
			Dialog.text = "Слушай, проваливай отсюда! Сорвешь сделку, и я вырву тебе сердце!";
			link.l1 = RandPhraseSimple("Ты смеешь мне угрожать, собака?", "Ты подавишься своими словами, каналья!");
			link.l1.go = "Exit_Smugglers_Fight";
			link.l2 = "Да ладно, я знаю, что такое бизнес. До встречи.";
			link.l2.go = "Exit";
			break;
		}
//		Dialog.Text = "Добрый день, меня зовут " + GetFullName(NPChar) + ", а вас?";
		Dialog.Text = LinkRandPhrase("...только я и дьявол знаем это место и последний, кто останется в живых, заберет себе все!...", 
			 "...а Мейнард ответил: 'Я не жду от тебя пощады, и ты тоже не дождешься ее от меня'.", 
			 "...однажды в море, будучи немного пьяным, он предложил: 'Давайте здесь сейчас устроим себе сами ад и посмотрим, кто дольше выдержит'.") + 
			 PCharRepPhrase(LinkRandPhrase(" А вот и еще один пират!",
					" Господа, у нашего стола джентльмен удачи, я полагаю.",
					" Запахло порохом и кровью, флибустьер справа по борту!"), 
				LinkRandPhrase(" Редко встретишь порядочного капитана в этих морях.",
					" Благородство не в моде в наше время.",
					" А вот и честный капитан, не чета вам, головорезы! Хе-хе!"));
//		link.l1 = "А меня " + GetFullName(pchar) + ".";
		link.l1 = PCharRepPhrase(RandSwear() + RandPhraseSimple("Всем налить! Я капитан " + GetFullName(pchar) + ".", 
					"Я капитан посудины '" + PChar.Ship.Name + "' и пары сотен настоящих чертей! Хе-хе! Мое имя " + GetFullName(pchar) + "."), 
				RandPhraseSimple("Позвольте представиться, джентльмены. Я капитан " + GetFullName(pchar) + ".", 
					"Не имею чести быть вам представленным. Я капитан корабля '" + PChar.Ship.Name + "' под флагом " + NationNameGenitive(sti(PChar.nation)) + ". Мое имя " + GetFullName(pchar) + "."));
		link.l1.go = "Meeting";
		break;

	case "Meeting":
		Dialog.Text = "Ну, а меня здесь все знают! Я " + GetFullName(NPChar) + ". Приятно познакомиться, чем могу быть полезен?";
		link.l1 = RandPhraseSimple("Есть одно дельце!", "У меня к тебе разговор...");
		link.l1.go = "quest";
		if (sti(pchar.Ship.Type) != SHIP_NOTUSED && sti(NPChar.Ship.Type) != SHIP_NOTUSED)
		{
			sld = GetRealShip(sti(NPChar.Ship.Type));
			
			Dialog.Text = "Ну, а меня здесь все знают! Я - " + GetFullName(NPChar) + ", капитан " + xiStr(sld.BaseName + "Acc") + " " + NPChar.Ship.Name + ".";
			link.l2 = "Я тут подумал, не хочешь ко мне присоединиться? С двумя кораблями мы сможем хорошие дела провернуть.";
			link.l2.go = "companion";
		}
		if (sti(NPChar.Ship.Type) == SHIP_NOTUSED)
		{
			link.l3 = RandPhraseSimple("Не хочешь пойти офицером ко мне на корабль?", "Пожалуй, такой офицер как ты, мне бы не помешал. Что скажешь?");
			link.l3.go = "officer";
		}
		if (bBettaTestMode)
		{
			link.l7 = "(Бетта-Тест) Квесты.";
			link.l7.go = "BT_Quest";
			link.l8 = "(Бетта-Тест) Дуэли.";
			link.l8.go = "outraged";
		}
		link.l9 = "Ничего.";
		link.l9.go = "exit";
		NextDiag.TempNode = "Second time";
		NPChar.meeting = true;
		break;

	case "Second time":
		if (CheckAttribute(NPChar, "PGGAi.location.town.back") && CheckAttribute(pchar, "questTemp.PGGContra.Know"))
		{
			Dialog.text = "Слушай, проваливай отсюда! Сорвешь сделку, и я вырву тебе сердце!";
			link.l1 = RandPhraseSimple("Ты смеешь мне угрожать, собака?", "Ты подавишься своими словами, каналья!");
			link.l1.go = "Exit_Smugglers_Fight";
			link.l2 = "Да ладно, я знаю, что такое бизнес. До встречи.";
			link.l2.go = "Exit";
			break;
		}

		if (CheckAttribute(NPChar, "QuestTalk") && GetNpcQuestPastDayParam(NPChar, "QuestTalk") < 1)
		{
			Dialog.Text = "Я тебе уже все сказал на сегодня!";
			link.l1 = "Ах да....";
			link.l1.go = "exit";
			break;
		}

		Dialog.text = RandPhraseSimple("Что тебе надо " + GetFullName(pchar) + "?", "Кто это? А ты... Проваливай, я не в настроении...");
		link.l1 = RandPhraseSimple("Как на счет дельца?", "У меня к тебе разговор...");
		link.l1.go = "quest";

		if (PGG_ChangeRelation2MainCharacter(NPChar, 0) > 49)
		{
//			Dialog.Text = RandPhraseSimple("Кого я вижу?!! Да это же " + GetFullName(pchar) + ". Что новенького?", "Как поживаете? Могу я быть чем-нибудь полезен?");
			Dialog.Text = LinkRandPhrase("...только я и дьявол знаем это место и последний, кто останется в живых, заберет себе все!...", 
				 "...а Мейнард ответил: 'Я не жду от тебя пощады, и ты тоже не дождешься ее от меня'", 
				 "...однажды в море, будучи немного пьяным, он предложил: 'Давайте здесь сейчас устроим себе сами ад и посмотрим, кто дольше выдержит'.") + 
				 PCharRepPhrase(LinkRandPhrase(" Редко встретишь порядочного капитана в этих морях.",
						" Благородство не в моде в наше время.",
						" А вот и честный капитан, не чета вам, головорезы! Хе-хе!") + " Какие вести?", 
					LinkRandPhrase(" А вот и еще один пират!",
						" Джентльмены, у нашего стола джентльмен удачи, я полагаю.",
						" Запахло порохом и кровью, флибустьер справа по борту!") + " Что нового творится на архипелаге?");
			if (sti(pchar.Ship.Type) != SHIP_NOTUSED && sti(NPChar.Ship.Type) != SHIP_NOTUSED)
			{
				link.l2 = RandPhraseSimple("Я тут подумал, не хочешь ко мне присоединиться? С двумя кораблями мы сможем хорошие дела провернуть.", "Слушай, ты выглядишь опытным капитаном. Не хочешь плавать под моим началом?");
				link.l2.go = "companion";
			}
			if (sti(NPChar.Ship.Type) == SHIP_NOTUSED)
			{
				Dialog.Text = RandPhraseSimple("Эх горе мне горе... Корабль мой на дне!", "Кого я вижу?! Каким ветром занесло в наши края? А я вот без корабля остался...");
				link.l3 = RandPhraseSimple("Жаль... Ну может вскоре повезет больше.", "Вот это дела... Ну мне пора, прощай.");
				link.l3.go = "exit";
				if (FindFreeRandomOfficer() > 0)
				{
					link.l3 = RandPhraseSimple("Не хочешь пойти офицером ко мне на корабль?", "Пожалуй такой офицер как ты мне бы не помешал. Что скажешь?");
					link.l3.go = "officer";
				}
			}
//			link.l4 = "Что нового слышно?";
//			link.l4.go = "rumours_capitan";
		}
		if (PGG_ChangeRelation2MainCharacter(NPChar, 0) < 20 || bBettaTestMode)
		{
			link.l8 = PCharRepPhrase("Вот это рожа. Так и хочется ее разукрасить.", "А... наш знакомый мерзавчик. Не желаете ли пару лишних дырок в груди?");
			link.l8.go = "outraged";
		}
		link.l9 = "Ничего.";
		link.l9.go = "exit";
		NextDiag.TempNode = "Second time";
		break;

	case "no_ship":
		break;

//==================================================
// Officer 
//==================================================
	case "officer":
		//раз в день.
		if (CheckAttribute(NPChar, "Officer_Talk") && GetNpcQuestPastDayParam(NPChar, "Officer_Talk") < 1 && !bPGGLogShow)
		{
			Dialog.Text = RandPhraseSimple("Я тебе уже говорил, что мне не интересно.", "Мне казалось мы все обсудили.");
			link.l1 = RandPhraseSimple("Как я мог забыть?!", "И правда. Ну, бывай.");
			link.l1.go = "exit";
			break;
		}
		//если уже был офицером, то посылает нафиг.
		if (CheckAttribute(NPChar, "PGGAi.Officer"))
		{
			Dialog.Text = RandPhraseSimple("Хм.. пожалуй, нет. Второй раз нет желания.", "И ты думаешь, я клюну на эту удочку дважды?!!");
			link.l1 = RandPhraseSimple("Ну как хочешь...", "Что ж, счастливо оставаться.");
			link.l1.go = "exit";
			break;
		}
		//если отношение ниже 70, то в 70% случаев откажет...
		if (PGG_ChangeRelation2MainCharacter(NPChar, 0) < 70 && rand(100) < 70)
		{
			Dialog.Text = RandPhraseSimple("Хм.. пожалуй, нет...", "Нет, я капитан, и не хочу быть офицером.");
			link.l1 = RandPhraseSimple("Ну как хочешь...", "Что ж, счастливо оставаться.");
			link.l1.go = "exit";
			SaveCurrentNpcQuestDateParam(NPChar, "Officer_Talk");
			break;
		}
		//Проверка на ПИРАТЕС, не было! :) //navy 04.04.07
		if (FindFreeRandomOfficer() < 1)
		{
			Dialog.Text = RandPhraseSimple("Да у тебя и так полный комплект.", "Не думаю, что ты готов командовать мной.");
			link.l1 = RandPhraseSimple("Жаль...", "Значит не судьба.");
			link.l1.go = "exit";
			break;
		}
		if (!CheckAttribute(NPChar, "Quest.Officer.Price"))
		{
			NPChar.Quest.Officer.Price = 10000 + sti(NPChar.rank)*500*MOD_SKILL_ENEMY_RATE - MakeInt(GetSummonSkillFromNameToOld(pchar, SKILL_COMMERCE)*1000);
		}
		Dialog.Text = NPCharRepPhrase(NPChar, RandSwear() + 
				RandPhraseSimple("Это - то, что нужно! Наши клинки напьются вражеской крови! Одна проблемка, мне нужно " + NPChar.Quest.Officer.Price + " пиастров, чтобы расплатиться с долгами.", 
								"Ты выглядишь настоящим морским волком. И если ты готов заплатить " + NPChar.Quest.Officer.Price + " пиастров, то пожалуй, мы сработаемся."), 
				LinkRandPhrase("Ваше предложение весьма лестно для меня. Я согласен за весьма умеренную плату в размере " + NPChar.Quest.Officer.Price + " пиастров.", 
							"Хм... Вы выглядете бывалым моряком. Возможно, мне стоит принять предложение, если вы согласны заплатить мне " + NPChar.Quest.Officer.Price + " пиастров.", 
							"Думаю мы сработаемся капитан. Всего " + NPChar.Quest.Officer.Price + " пиастров и мы ударим по рукам."));

		MakeRandomLinkOrderTwo(link, PCharRepPhrase(RandSwear() + RandPhraseSimple("Меня это устраивает. Но, надеюсь, ты не каждый день будешь требовать столько.", "Это чертовски дорого, но, думаю, ты того стоишь."), 
				LinkRandPhrase("Я согласен. Считай себя зачисленным в команду.", "Хм... Пожалуй, я готов расстаться с этой суммой.", "Похоже, ты стоишь этих денег. Держи!")), "officer_hired",
									PCharRepPhrase(RandSwear() + RandPhraseSimple("Ты не стоишь столько. Бывай...", "Это чертовски дорого, я не готов выкидывать столько денег."), 
				LinkRandPhrase("Да.. жадность не порок...", "Да ты смеешься надо мной.. за такие деньги я могу купить десяток таких, как ты.", "Это грабеж... где я возьму тебе такую сумму?")), "exit");
		link.l3 = "Хм... а что ты представляешь из себя?";
		link.l3.go = "Exit_ShowParam";
		break;

	case "officer_hired":
		if (sti(pchar.money) < sti(NPChar.Quest.Officer.Price))
		{
			Dialog.Text = "Похоже у вас проблемы с наличностью, капитан.";
			link.l1 = "Ах.. действительно.";
			link.l1.go = "exit";
			break;
		}
		pchar.questTemp.HiringOfficerIDX = NPChar.index;
		AddMoneyToCharacter(pchar, -(makeint(NPChar.Quest.Officer.Price)));
		AddDialogExitQuestFunction("PGG_BecomeHiredOfficer");
		NPChar.loyality = MakeInt(PGG_ChangeRelation2MainCharacter(NPChar, 0)*0.3)

		DeleteAttribute(NPChar, "Quest.Officer");
		DeleteAttribute(NPChar, "PGGAi.Task");
		DeleteAttribute(NPChar, "PGGAi.LockService");

		Dialog.Text = RandPhraseSimple("Вот и отлично!", "Я не сомневался в вас, капитан.");
		link.l1 = "Не опаздывай к отплытию.";
		link.l1.go = "exit";
		break;

//==================================================
// Companion 
//==================================================
	case "companion":
		Dialog.Text = "Если вы читаете это, то значит вы поймали баг ;) Сообщите об этом Navy, подробно, как вы его поймали :)";
		link.l1 = "Обязательно!!!!";
		link.l1.go = "exit";
		//раз в день.
		if (CheckAttribute(NPChar, "Companion_Talk") && GetNpcQuestPastDayParam(NPChar, "Companion_Talk") < 1 && !bPGGLogShow)
		{
			Dialog.Text = RandPhraseSimple("Я тебе уже говорил, что мне не интересно.", "Мне казалось мы все обсудили.");
			link.l1 = RandPhraseSimple("Как я мог забыть?!", "И правда. Ну, бывай.");
			link.l1.go = "exit";
			break;
		}
		//разница в один класс.. нефиг халявить
		if (GetCharacterShipClass(NPChar) < GetCharacterShipClass(PChar)-1)
		{
			Dialog.Text = RandPhraseSimple("Плавать с тобой!?? Подрасти сначала!", "Не думаю что мне это интересно!");
			link.l1 = RandPhraseSimple("Ну как хочешь...", "Что ж, счастливо оставаться.");
			link.l1.go = "exit";
			SaveCurrentNpcQuestDateParam(NPChar, "Companion_Talk");
			break;
		}
		//если отношение ниже 70, то в 70% случаев откажет...
		if (PGG_ChangeRelation2MainCharacter(NPChar, 0) < 70 && rand(100) < 70)
		{
			Dialog.Text = RandPhraseSimple("Хм.. пожалуй, нет...", "Нет, спасибо. Не люблю плавать под чьим-то началом.");
			link.l1 = RandPhraseSimple("Ну как хочешь...", "Что ж, счастливо оставаться.");
			link.l1.go = "exit";
			SaveCurrentNpcQuestDateParam(NPChar, "Companion_Talk");
			break;
		}
		//есть ли место для компаньона.
		if (SetCompanionIndex(pchar, -1, sti(NPChar.index)) != -1)
		{
			Dialog.Text = NPCharRepPhrase(NPChar, RandSwear() + 
					RandPhraseSimple("Ты мне нравишься! По рукам! Я уже слышу звон пиастров в наших карманах!", "А ты парень не промах... Думаю мы сработаемся!"), 
					LinkRandPhrase("Ваше предложение мне нравится. Я соглас" + GetSexPhrase("ен","на") + ".", 
					"Это интересно. Я тоже думаю, что вместе мы сможем больше!", "Пожалуй, это будет занятно. По рукам."));
			link.l1 = RandPhraseSimple("Отлично, жду тебя в море!", "По рукам! Мы будем отличной командой.");
			link.l1.go = "exit";
			NPChar.PGGAi.IsPGG = false;
			NPChar.PGGAi.location.town = "none";
			NPChar.Dialog.TempNode = "hired";
			NPChar.Dialog.FileName = "Enc_Officer_dialog.c";
            NPChar.Payment = true;
            NPChar.Money   = 0;
            
            SetBaseShipData(NPChar);
            DeleteAttribute(NPChar,"ship.sails");
			DeleteAttribute(NPChar,"ship.masts");
			DeleteAttribute(NPChar,"ship.blots");
			
			Fantom_SetCannons(NPChar, "pirate");
			Fantom_SetBalls(NPChar, "pirate");
    
			DeleteAttribute(NPChar, "PGGAi.Task");
			DeleteAttribute(NPChar, "PGGAi.LockService");
			SetCharacterRemovable(NPChar, true);
			SaveCurrentNpcQuestDateParam(NPChar, "Companion.CheckRelation");
		}
		else
		{
			Dialog.Text = RandPhraseSimple("Похоже тебе уже достаточно компаньонов.", "У тебя и так много кораблей. Зачем тебе еще один?");
			link.l1 = "Да... Может ты и прав.";
			link.l1.go = "exit";
		}
		break;

	case "companion_leave":
		Dialog.Text = RandSwear() + " " + NPCharRepPhrase(NPChar,
					RandPhraseSimple("Ты мне надоел!!! Я выхожу из состава эскадры!", "Мое терпенье лопнуло! Я отчаливаю."), 
					RandPhraseSimple("Я устал от ваших выходок, 'адмирал'... Разрешите откланяться.", "Хватит с меня вашего командования. Я выхожу из состава эскадры."));
		link.l1 = PCharRepPhrase(RandPhraseSimple("Ну и проваливай на здоровье.", "Можешь катиться ко всем чертям."),
					RandPhraseSimple("Что ж, держать не буду.", "Вольному воля. Попутного ветра в паруса."));
		link.l1.go = "Exit_Companion_Leave";
		link.l2 = PCharRepPhrase(RandPhraseSimple("Э... как на счет небольшой премии?", "Каррамба!!! Может останешься? Я готов даже выплатить пермию твоей команде!"),
					RandPhraseSimple("Жаль расставаться... Может, небольшое вознаграждение решит наши разногласия?", "Как на счет прибавки к жалованию?"));
		link.l2.go = "companion_stay";
		break;

	case "companion_stay":
		if (CheckAttribute(NPChar, "Quest.Companion"))
		{
			Dialog.Text = RandPhraseSimple("Хватит с меня одного раза. Больше я не верю в твои обещания!", "Никакими деньгами меня не соблазнишь. Мое решение окончательно!");
			link.l1 = RandPhraseSimple("Ну что же, тогда всех благ.", "Тогда проваливай, и чтобы я тебя больше не видел!");
			link.l1.go = "Exit_Companion_Leave";
			break;
		}
		NPChar.Quest.Companion.Price = 2*GetSalaryForShip(NPChar);
		Dialog.Text = RandSwear() + " " + NPCharRepPhrase(NPChar,
					RandPhraseSimple("Премия? Отлично! Сумма в " + NPChar.Quest.Companion.Price + " пиастров, меня устроит.", 
						"Пожалуй, если ты выплатишь моей команде " + NPChar.Quest.Companion.Price + " пиастров, я уговорю их остаться."), 
					RandPhraseSimple("Хорошо! Я готов остаться за скромную премию в размере " + NPChar.Quest.Companion.Price + " пиастров!",
						NPChar.Quest.Companion.Price + " пиастров устроят на некоторое время меня и мою команду."));
		link.l1 = PCharRepPhrase(RandPhraseSimple("Да ты смеешься надо мной! Проваливай ко всем чертям.", "Ха... да я лучше потрачу эти деньги на что-то более полезное!"),
					RandPhraseSimple("Ну уж нет. Я не настолько богат!", "Прощай. Я не желаю платить так много."));
		link.l1.go = "Exit_Companion_Leave";
		if (sti(pchar.money) >= sti(NPChar.Quest.Companion.Price))
		{
			link.l2 = PCharRepPhrase(RandPhraseSimple("Вот и отлично! Держи премиальные!", "Я не сомневался, что дело только в деньгах!"),
					RandPhraseSimple("Прекрасно! Я рад, что ты остаешься!", "Вот обещанная премия. И теперь мы снова вместе."));
			link.l2.go = "Exit_Companion_Stay";
		}
		break;

	case "Exit_Companion_Leave":
		chrDisableReloadToLocation = false;
		RemoveCharacterCompanion(pchar, NPChar);

		NPChar.PGGAi.IsPGG = true;
		NPChar.RebirthPhantom = true;
		NPChar.PGGAi.location.town = GetCurrentTown();

		LAi_SetWarriorTypeNoGroup(NPChar);
		NextDiag.CurrentNode = "Second Time";
		DialogExit();
		break;

	case "Exit_Companion_Stay":
        GetCharacterPos(NPChar, &locx, &locy, &locz);
        sTmp = LAi_FindNearestFreeLocator("reload", locx, locy, locz);
		LAi_ActorGoToLocation(NPChar, "reload", sTmp, "none", "", "", "", 5.0);

		AddMoneyToCharacter(pchar, -(makeint(NPChar.Quest.Companion.Price)));
		NextDiag.CurrentNode = NextDiag.TempNode;
		DialogExit();
		break;

//==================================================
// Quests
//==================================================
//==== BETTA_TEST ====
	case "BT_Quest":
		Dialog.Text = "Выберите шаблон!";
		link.l1 = "Квест 1 Шаблон - морской.";
		link.l1.go = "BT_Quest_1";
		link.l2 = "Квест 1 Шаблон - сухопутный.";
		link.l2.go = "BT_Quest_2";
		break;

	case "BT_Quest_1":
		NPChar.PGGAi.ActiveQuest.QstNumber = 0;
		NPChar.PGGAi.ActiveQuest.QstNumber.Template = 1;
		Dialog.Text = "ВЫБРАН! Квест 1 Шаблон - морской.";
		link.l1 = "Ах да....";
		link.l1.go = "quest";
		break;
	case "BT_Quest_2":
		NPChar.PGGAi.ActiveQuest.QstNumber = 0;
		NPChar.PGGAi.ActiveQuest.QstNumber.Template = 0;
		Dialog.Text = "ВЫБРАН! Квест 1 Шаблон - сухопутный.";
		link.l1 = "Ах да....";
		link.l1.go = "quest";
		break;
//==== BETTA_TEST ====

	case "quest":
		chrDisableReloadToLocation = false;
		if (CheckAttribute(NPChar, "QuestTalk") && GetNpcQuestPastDayParam(NPChar, "QuestTalk") < 1)
		{
			Dialog.Text = "Я тебе уже все сказал!";
			link.l1 = "Ах да....";
			link.l1.go = "exit";
			break;
		}
		Dialog.Text = RandPhraseSimple("Какие дела??... Ик... Не видишь мы с друзьями отдыхаем!..Ик..",
									"Э... Никаких дел сегодня!");
		link.l1 = "Понятно...";
		link.l1.go = "exit";
		if (!PGG_IsQuestAvaible()) break; //квест нельзя брать.
		//if (!CheckAttribute(NPChar, "PGGAi.ActiveQuest") && !bBettaTestMode) break; //для релиза возможно надо будет закрыть. если закоментить, то ГГ сам может предложить дело
		if (CheckAttribute(NPChar, "PGGAi.ActiveQuest.QstNumber")) iRnd = sti(NPChar.PGGAi.ActiveQuest.QstNumber); //тип квеста, который хочет от нас ПГГ
		if (iRnd == -1 && !CheckAttribute(pchar, "GenQuest.PGG_Quest")) iRnd = rand(1); // может и не быть
		switch (iRnd)
		{
		case 0:
			if (sti(NPChar.Ship.Type) != SHIP_NOTUSED && sti(PChar.Ship.Type) != SHIP_NOTUSED && GetCharacterShipClass(PChar) <= 4 && GetCompanionQuantity(PChar) < COMPANION_MAX)
			{
				//квест от ПГГ
				/*if (CheckAttribute(NPChar, "PGGAi.ActiveQuest"))
				{ */
				if (!CheckAttribute(pchar, "GenQuest.PGG_Quest"))
				{  // не занят другими квестами ПГГ
					Dialog.Text = "Что за разговор? Хотя ты вовремя. Дело или дельце - это мы завсегда!";
					link.l1 = "Хм. Ты о чем?";
					link.l1.go = "Quest_1_Work";
					NPChar.PGGAi.ActiveQuest = 1; // чтоб говорил от своего имени
					PChar.GenQuest.PGG_Quest = 1; // чтоб не достовали другие ПГГ
				}
				/* }
				//квест от ГГ
				else
				{
					if ()
					{
						Log_TestInfo("Пошло тестовое предложение самого ГГ");

						Dialog.Text = LinkRandPhrase("...только я и дьявол знаем это место и последний, кто останется в живых, заберет себе все!...", 
							 "...а Мейнард ответил: 'Я не жду от тебя пощады, и ты тоже не дождешься ее от меня'", 
							 "...однажды в море, будучи немного пьяным, он предложил: 'Давайте здесь сейчас устроим себе сами ад и посмотрим, кто дольше выдержит'.") + 
							 PCharRepPhrase(LinkRandPhrase("А вот и еще один пират!",
									"Джентльмены, у нашего стола джентльмен удачи, я полагаю.",
									"Запахло порохом и кровью, флибустьер справа по борту!"), 
								LinkRandPhrase("Редко встретишь порядочного капитана в этих морях.",
									"Благородство не в моде в наше время.",
									"А вот и честный капитан, не чета вам, головорезы! Хе-хе!"));
						link.l1 = "Я не ору, а говорю дело есть! Прибыльное!";
						link.l1.go = "Quest_1_Work";
					}
				}  /**/
			}
			break;
		}
		SaveCurrentNpcQuestDateParam(NPChar, "QuestTalk");
		break;

	case "quest_onStay": // ПГГ сам подходит
		chrDisableReloadToLocation = false;
		Dialog.Text = "Поосторожнее!!! Ба! Да я тебя знаю, ты " + GetFullName(PChar) + ". Наслышан, наслышан.";
		if (!CheckAttribute(NPChar, "meeting") || !sti(NPChar.meeting))
		{
			link.l1 = PCharRepPhrase(RandSwear() + RandPhraseSimple("Я капитан " + GetFullName(pchar) + ".", 
						"Я капитан посудины '" + PChar.Ship.Name + "' и пары сотен настоящих чертей! Хе-хе! Мое имя " + GetFullName(pchar) + ".") + " А тебя я что-то не помню.", 
					RandPhraseSimple("Позвольте представиться, джентльмены. Я капитан " + GetFullName(pchar) + ".", 
						"Не имею чести быть вам представленным. Я капитан корабля '" + PChar.Ship.Name + "' под флагом " + NationNameGenitive(sti(PChar.nation)) + ". Мое имя " + GetFullName(pchar) + ".") + " А Вас я что-то не помню.");
//			link.l1 = "Хм... а ты кто, что-то я тебя не помню.";
			link.l1.go = "Quest_1_Meeting";
		}
		else
		{
			link.l1 = "Привет, " + GetFullName(NPChar) + ". Как дела?";
			link.l1.go = "Quest_1_Work";
		}
		SaveCurrentNpcQuestDateParam(NPChar, "QuestTalk");
		break;

	//=========== Первый квест ==========
	case "Quest_1_Meeting":
		sld = GetRealShip(sti(NPChar.Ship.Type));
		Dialog.Text = "Я - " + GetFullName(NPChar) + ", капитан " + xiStr(sld.BaseName + "Acc") + " " + NPChar.Ship.Name + NPCharRepPhrase(NPChar, ". Самый известный корсар всего испанского мэйна.", " Обычный моряк.");
		link.l1 = "Да, теперь я тебя запомню.";
		link.l1.go = "Quest_1_Work";
		NPChar.meeting = true;
		break;

	case "Quest_1_Work":
		iRnd = rand(3);
		PChar.GenQuest.PGG_Quest.Template = rand(1);
		if (CheckAttribute(NPChar, "PGGAi.ActiveQuest.QstNumber.Template"))
		{
			PChar.GenQuest.PGG_Quest.Template = NPChar.PGGAi.ActiveQuest.QstNumber.Template;
		}
		PChar.GenQuest.PGG_Quest.Parts = GetCompanionQuantity(PChar)+1;
		PChar.GenQuest.PGG_Quest.Nation = iRnd;
		PChar.GenQuest.PGG_Quest.Island = GetRandomIslandId();
		while(PChar.GenQuest.PGG_Quest.Island == Islands[GetCharacterCurrentIsland(pchar)].id)
		{
			PChar.GenQuest.PGG_Quest.Island = GetRandomIslandId();
		}
		PChar.GenQuest.PGG_Quest.Island.Shore = GetIslandRandomShoreId(PChar.GenQuest.PGG_Quest.Island);
		while(PChar.GenQuest.PGG_Quest.Island.Shore == "")
		{
			PChar.GenQuest.PGG_Quest.Island = GetRandomIslandId();
			PChar.GenQuest.PGG_Quest.Island.Shore = GetIslandRandomShoreId(PChar.GenQuest.PGG_Quest.Island);
			if (sti(PChar.GenQuest.PGG_Quest.Template)) 
			{
				if (!isLocationFreeForQuests(PChar.GenQuest.PGG_Quest.Island)) PChar.GenQuest.PGG_Quest.Island.Shore = "";
			}
			else
			{
				if (!isLocationFreeForQuests(PChar.GenQuest.PGG_Quest.Island.Shore)) PChar.GenQuest.PGG_Quest.Island.Shore = "";
			}
		}
		PChar.GenQuest.PGG_Quest.Island.Town = FindTownOnIsland(PChar.GenQuest.PGG_Quest.Island);
		PChar.GenQuest.PGG_Quest.Days = 3 + GetMaxDaysFromIsland2Island(Islands[GetCharacterCurrentIsland(pchar)].id, PChar.GenQuest.PGG_Quest.Island);
		PChar.GenQuest.PGG_Quest.Goods = GOOD_SLAVES + rand(2);
		if (CheckAttribute(NPChar, "PGGAi.ActiveQuest"))
		{
//			Dialog.Text = "Дело у меня к тебе, приятель. Знаю, можно тебе доверится, но в таверне обсуждать не возьмусь, ушей много лишних. Жду тебя у меня на борту. Помнишь, моя посудина зовется '" + NPChar.Ship.Name + "'.";
//			link.l1 = "Выкладывай сейчас, некогда мне по гостям расхаживать.";
			Dialog.Text = PCharRepPhrase(RandPhraseSimple("У меня есть остренькое дельце для Вас и ваших головорезов.", 
								"Объединим силы и  мы сможем взять отличный куш! Ну, конечно, придется кое-кого убить. Хе-хе."), 
							RandPhraseSimple("У меня есть к вам деловое предложение, капитан.", 
							"Капитан, ваша репутация хорошо известна и, поэтому, я хочу вам предложить участие в одном заманчивом деле.")) + 
						RandPhraseSimple("Только между нами. Давай-ка, обсудим все у меня в кают-компании на '" + NPChar.Ship.Name + "' за бочонком отличного эля. Сегодня.", "Здесь говорить об этом не резон, потолкуем у меня на судне '" + NPChar.Ship.Name + "' в тишине да в спокойствии. Не затягивай.");
			link.l1 = PCharRepPhrase(RandSwear() + RandPhraseSimple("К черту проволочки! Хорошее хочу знать быстро, а плохое еще быстрее!", 
								"Незачем откладывать! В таверне есть комната, там и обсудим!"), 
							RandPhraseSimple("Почему бы не обсудить ваше предложение прямо сейчас в комнате таверны?", 
								"Боюсь у меня нет желания посещать ваше судно. Если вам что-то нужно, выкладывайте здесь!"));
		}
		else
		{
			PChar.GenQuest.PGG_Quest.Nation = FindEnemyNation2NationWithoutPirates(sti(PChar.Nation));
			Dialog.Text = "Тссс... В таверне обсуждать дела не стоит, ушей много лишних. Жду тебя на моем корабле '" + NPChar.Ship.Name + "'. Там и поговорим.";
			link.l1 = "Некогда мне, да и бегать я не люблю.";
		}
		link.l1.go = "Quest_1_Work_1";
		link.l2 = PCharRepPhrase(RandPhraseSimple("Я приду! Но, если ты попусту потратишь мое время, я вырву тебе язык!", 
					RandSwear() + "Жди, надеюсь дело стоящее, и не вздумай шутить со мной!"), 
				RandPhraseSimple("Я с удовольствием принимаю ваше приглашение, капитан.", 
					"Вы меня заинтриговали. Разумеется, я приду."));
		link.l2.go = "Exit_Quest_1_AfterTavernTalk";
		break;

	case "Quest_1_Work_1":
		Dialog.Text = PCharRepPhrase(RandPhraseSimple("Да тебя, небось, от качки тошнит! Иди, найди себе мамашу с соской!!!", 
					"Что, приятель, в штаны наложил от страха?!! Вали! Ты мне не пара."), 
				RandPhraseSimple("Нет, так нет. Неволить не буду, значит не интересно тебе золото.", 
					"Или так, или никак. Тебе решать. На золото много охотников."));
		link.l1 = PCharRepPhrase(RandPhraseSimple("Ха! А ты с перчиком! И не боишься... Приду!", 
					"Умеешь настоять на своем, капитан. Жди."), 
				RandPhraseSimple("Хорошо, я согласен.", 
					"Вы очень убедительны. Я готов обсудить дела на вашем судне."));
		link.l1.go = "Exit_Quest_1_AfterTavernTalk";
		link.l2 = PCharRepPhrase(RandPhraseSimple("Ха! Думал заманить меня на свой корабль и убить? Не выйдет!", 
					"Слова твои не стоят и пенни! Не к чему мне с тобой дела иметь!"), 
				RandPhraseSimple("Сомнительное предложение. Я вынужден отказаться.", 
					"Приятно было познакомится, капитан " + GetFullName(NPChar) + ". Прощайте."));
		link.l2.go = "Quest_1_NotWork";
		break;

	case "Quest_1_NotWork":
		Dialog.Text = PCharRepPhrase(RandPhraseSimple("Рому мне! А ты убирайся с глаз моих!", 
					"Мы уже все обсудили, не так ли?"), 
				RandPhraseSimple("Идите своей дорогой, я занят.", 
					"Полагаю, у нас с вами нет общих тем."));
		link.l1 = RandSwear();
		link.l1.go = "exit_2";
		break;

	case "Quest_1_Work_2":
		Dialog.Text = "Я же сказал, что жду тебя на корабле!";
		link.l1 = "А... конечно...";
		link.l1.go = "exit";
		NextDiag.TempNode = "Quest_1_Work_2";
		break;

	case "Exit_Quest_1_AfterTavernTalk":
		PChar.GenQuest.PGG_Quest = 1;
		PChar.GenQuest.PGG_Quest.Stage = 0;
		PChar.GenQuest.PGG_Quest.PGGid = NPChar.id;

		Group_AddCharacter("PGGQuest", NPChar.id);
		Group_SetGroupCommander("PGGQuest", NPChar.id);
		Group_SetAddress("PGGQuest", Islands[GetCharacterCurrentIsland(PChar)].id, "Quest_Ships", "Quest_Ship_1");
		Group_SetTaskNone("PGGQuest");

		SetTimerConditionParam("PGGQuest1_RemoveShip_Timer", "PGGQuest1_RemoveShip_Timer", 0, 0, 0, MakeInt(GetHour() + 6), false);
		PChar.Quest.PGGQuest1_RemoveShip_Timer.function = "PGG_Q1RemoveShip";

		NPChar.Ship.Mode = "Pirate";
		NPChar.DeckDialogNode = "Quest_1_Ship";
		NPChar.Nation.Bak = NPChar.Nation;
		NPChar.Nation = GetCityNation(GetCurrentTown());
		NPChar.AlwaysFriend = true;
		NPChar.Abordage.Enable = false; //нельзя брать на абордаж
		SetCharacterRelationBoth(sti(PChar.index), sti(NPChar.index), RELATION_FRIEND);

		ReOpenQuestHeader("Gen_PGGQuest1");
		if (CheckAttribute(NPChar, "PGGAi.ActiveQuest"))
		{
			AddQuestRecord("Gen_PGGQuest1", "q1_Tavern");
		}
		else
		{
			AddQuestRecord("Gen_PGGQuest1", "q1_Tavern_1");
		}
		AddQuestUserData("Gen_PGGQuest1", "sShipName", NPChar.Ship.Name);

		NextDiag.CurrentNode = "Quest_1_Work_2";
		DialogExit();
		if (NPChar.Chr_Ai.Type == "actor") LAi_SetWarriorTypeNoGroup(NPChar);
		break;

	case "Quest_1_Ship":
		PChar.Quest.PGGQuest1_RemoveShip_Timer.Over = "yes";
		if (GetCompanionQuantity(PChar) == COMPANION_MAX)
		{
			Dialog.Text = RandPhraseSimple("Э нет... Так дело не пойдет... Не много ли тебе кораблей, приятель?", 
					"Ха! В такой большой компании секрета не удержишь! Проваливай.");
			link.l1 = RandPhraseSimple("Ну как хочешь!", 
					"Что ж, не очень то и хотелось.");
			link.l1.go = "Exit";
			PChar.Quest.PGGQuest1_RemoveEnd.win_condition.l1 = "ExitFromLocation";
			PChar.Quest.PGGQuest1_RemoveEnd.win_condition.l1.Location = PChar.location;
			PChar.Quest.PGGQuest1_RemoveEnd.function = "PGG_Q1RemoveShip";
			break;
		}
		PChar.GenQuest.PGG_Quest.Stage = 1;
		if (CheckAttribute(NPChar, "PGGAi.ActiveQuest"))
		{
//			Dialog.Text = "Ну, вот и славно, теперь я уверен лишних ушей нет, мои матросы уже в курсе. Предложение такое - взять немного бесхозного добра!";
//			link.l1 = "А что тут сложного? И почему ты делишься этим со мной?";
			Dialog.Text = RandPhraseSimple("Добро пожаловать на борт, капитан!", 
					"Ну, вот и славно, теперь я уверен лишних ушей нет, мои матросы уже в курсе.") + 
					" Предложение такое – взять немного бесхозного добра.";
			link.l1 = RandPhraseSimple("А что тут сложного? И почему ты делишься этим со мной?", 
					"- Бесхозного? Да ты, приятель, шутишь!");
		}
		else
		{
			Dialog.Text = "Ну, вот и славно, теперь я уверен лишних ушей нет, рассказывай!";
			link.l1 = "Предложение такое - взять немного бесхозного";
		}
		link.l1.go = "Quest_1_Ship_1";
		break;

	case "Quest_1_Ship_1":
		if (CheckAttribute(NPChar, "PGGAi.ActiveQuest"))
		{
			Dialog.Text = RandPhraseSimple("Ну, есть люди, которые считают его своим, а нужно убедить их в обратном.", 
					"Ну конечно, придется кое-кого убить. Но, ведь за этим дело не встанет?!");
			link.l1 = PCharRepPhrase(RandSwear() + RandPhraseSimple("Ближе к делу!", 
					"Это уже пахнет хорошей резней! Где они?"), 
				RandPhraseSimple("Хотелось бы узнать подробности вашего предложения.", 
					"Что конкретно вы хотите мне предложить?"));
			link.l1.go = "Quest_1_Ship_Detail";
			link.l2 = PCharRepPhrase(RandPhraseSimple("Что-то ты темнишь. Не хочу я с тобой в это ввязываться!", 
					"Выглядит просто, да я тебе не верю! Хочешь чужими руками жар загребать? Я не согласен!"), 
				RandPhraseSimple("Предложение заманчивое, но вынужден отказаться.", 
					"Нет, чужое я с детства не беру, предпочитаю, что бы дарили. Я пасс."));
			link.l2.go = "Quest_1_Ship_Refuse";
		}
		else
		{
			Dialog.Text = "А что тут сложного? И почему ты делишься этим со мной?";
			link.l1 = "Ну, есть люди, которые считают его своим, а нужно убедить их в обратном.";
			link.l1.go = "Quest_1_Ship_Detail";
		}
		break;

	case "Quest_1_Ship_Detail":
		sTmp = "Караван принадлежащий " + NationNameGenitive(sti(PChar.GenQuest.PGG_Quest.Nation)) + ", перевозящий ";
		if (sti(PChar.GenQuest.PGG_Quest.Goods) == GOOD_SLAVES)
		{
			sTmp += "Рабов";
			PChar.GenQuest.PGG_Quest.Goods.Text = "Рабов";
		}
		else
		{
			sTmp += XI_ConvertString(Goods[sti(PChar.GenQuest.PGG_Quest.Goods)].Name);
			PChar.GenQuest.PGG_Quest.Goods.Text = XI_ConvertString(Goods[sti(PChar.GenQuest.PGG_Quest.Goods)].Name);
		}

		if (PChar.GenQuest.PGG_Quest.Island.Town == "" || PChar.GenQuest.PGG_Quest.Island.Town == "Caiman" ||
			PChar.GenQuest.PGG_Quest.Island.Town == "Terks" || PChar.GenQuest.PGG_Quest.Island.Town == "Dominica")
		{
			sLoc = GetConvertStr(PChar.GenQuest.PGG_Quest.Island, "LocLables.txt"))
		}
		else
		{
			sLoc = XI_ConvertString("Colony" + PChar.GenQuest.PGG_Quest.Island.Town + "Gen");
		}
		
		if (sti(PChar.GenQuest.PGG_Quest.Template)) 
		{
			sTmp +=	", остановился недалеко от " + sLoc + " у бухты " + GetConvertStr(PChar.GenQuest.PGG_Quest.Island.Shore, "LocLables.txt") + " для пополнения припасов.";
		}
		else
		{
			sTmp += ", попал в шторм недалеко от " + sLoc + " и пошел на дно. Но они успели спасти часть груза и высадились в бухте " + GetConvertStr(PChar.GenQuest.PGG_Quest.Island.Shore, "LocLables.txt") + 
				" Сейчас они ждут прихода своей эскадры.";
		}
		
		Dialog.Text = sTmp;
		link.l1 = PCharRepPhrase(RandSwear() + RandPhraseSimple("Не тяни, выкладывай все поживее!", 
					"Я в тебе не ошибся, что ты еще знаешь?"), 
				RandPhraseSimple("Очень интересно, продолжайте!", 
					"Я весь во внимании!"));
		link.l1.go = "Quest_1_Ship_Detail_1";

		if (CheckAttribute(NPChar, "PGGAi.ActiveQuest"))
		{
			link.l2 = PCharRepPhrase(RandPhraseSimple("Дельце неплохое, но откажусь. Не резон мне с " + NationNameAblative(sti(PChar.GenQuest.PGG_Quest.Nation)) + " сорится.", 
					"Караван? Бесхозное? Да там тысяча солдат при нем! Не годится. Я ухожу."), 
				RandPhraseSimple("Нет, с " + NationNameAblative(sti(PChar.GenQuest.PGG_Quest.Nation)) + " я не воюю, не интересно.", 
					"Мой ответ – нет! Я не собираюсь портить отношения с " + NationNameAblative(sti(PChar.GenQuest.PGG_Quest.Nation)) + "!"));
			link.l2.go = NPCharRepPhrase(NPChar, "Quest_1_Ship_BadWay", "Quest_1_Ship_Refuse");
		}
		else
		{
			Dialog.Text = "Хм... Это уже пахнет хорошей резней. Детали?";
			link.l1 = sTmp;
		}
		if (sti(PChar.GenQuest.PGG_Quest.Template)) 
		{
			AddQuestRecord("Gen_PGGQuest1", "q1_Detail_A");
		}
		else
		{
			AddQuestRecord("Gen_PGGQuest1", "q1_Detail_B");
		}
		AddQuestUserData("Gen_PGGQuest1", "sNation", NationNameGenitive(sti(PChar.GenQuest.PGG_Quest.Nation)));
		AddQuestUserData("Gen_PGGQuest1", "sGoods", PChar.GenQuest.PGG_Quest.Goods.Text));
		AddQuestUserData("Gen_PGGQuest1", "sColony", sLoc);
		AddQuestUserData("Gen_PGGQuest1", "sShore", GetConvertStr(PChar.GenQuest.PGG_Quest.Island.Shore, "LocLables.txt"));
		break;

	case "Quest_1_Ship_Detail_1":
		if (CheckAttribute(NPChar, "PGGAi.ActiveQuest"))
		{
//			Dialog.Text = "У нас есть только " + FindRussianDaysString(sti(PChar.GenQuest.PGG_Quest.Days)) + " для того, чтобы перехватить их.";
			Dialog.Text = PCharRepPhrase(RandPhraseSimple("Через " + FindRussianDaysString(sti(PChar.GenQuest.PGG_Quest.Days)) + " караван уйдет и нам их уже не догнать.", 
						"У нас есть ровно " + FindRussianDaysString(sti(PChar.GenQuest.PGG_Quest.Days)) + " дней, чтобы успеть их вырезать и забрать добычу!"), 
					RandPhraseSimple("В нашем распоряжении " + FindRussianDaysString(sti(PChar.GenQuest.PGG_Quest.Days)) + " дней на всю операцию.", 
						"Нам стоит поторопится, " + PChar.Name + ". У нас в запасе всего " + FindRussianDaysString(sti(PChar.GenQuest.PGG_Quest.Days)) + "."));
			link.l1 = RandPhraseSimple("Мы успеем! Отплываем сегодня же.", "Не будем медлить. Прикажите отплывать сегодня же.");
			link.l1.go = "Exit_Quest_1_Accept";
			AddQuestRecord("Gen_PGGQuest1", "q1_Accept");
		}
		else
		{
			Dialog.Text = "Очень интересно, продолжай.";
			link.l1 = "У нас есть только " + FindRussianDaysString(sti(PChar.GenQuest.PGG_Quest.Days)) + " для того, чтобы перехватить их.";
			link.l1.go = "Quest_1_Ship_Accept";
			AddQuestRecord("Gen_PGGQuest1", "q1_Accept_1");
		}
		AddQuestUserData("Gen_PGGQuest1", "nDays", FindRussianDaysString(sti(PChar.GenQuest.PGG_Quest.Days)));
		break;

	case "Quest_1_Ship_Accept":
		Dialog.Text = "Не будем медлить! В путь!";
		link.l1 = "В путь!";
		link.l1.go = "Exit_Quest_1_Accept";
		break;

	case "Exit_Quest_1_Accept":
		if (sti(PChar.GenQuest.PGG_Quest.Stage) < 2)
		{
			PChar.GenQuest.PGG_Quest.Stage = 2;
			PGG_Q1RemoveShip("");

			if (sti(PChar.GenQuest.PGG_Quest.Template)) 
			{
				PGG_Q1PlaceShipsNearIsland();
			}
			else
			{
				Locations[FindLocation(PChar.GenQuest.PGG_Quest.Island.Shore)].DisableEncounters = true;

				PChar.Quest.PGGQuest1_CheckStartState.win_condition.l1 = "Location";
				PChar.Quest.PGGQuest1_CheckStartState.win_condition.l1.Location = PChar.GenQuest.PGG_Quest.Island.Shore;
				PChar.Quest.PGGQuest1_CheckStartState.function = "PGG_Q1CheckStartState";
			}
			PChar.Quest.PGGQuest1_PGGDead.win_condition.l1 = "NPC_Death";
			PChar.Quest.PGGQuest1_PGGDead.win_condition.l1.Character = PChar.GenQuest.PGG_Quest.PGGid;
			PChar.Quest.PGGQuest1_PGGDead.function = "PGG_Q1PGGDead";

			SetTimerCondition("PGGQuest1_Time2Late", 0, 0, sti(PChar.GenQuest.PGG_Quest.Days), false);
			PChar.Quest.PGGQuest1_Time2Late.function = "PGG_Q1Time2Late";

			NPChar.PGGAi.IsPGG = false;
			NPChar.PGGAi.location.town = "none";
			DeleteAttribute(NPChar, "PGGAi.Task");
			DeleteAttribute(NPChar, "PGGAi.LockService");
			DeleteAttribute(NPChar, "AlwaysFriend");

			SetCompanionIndex(PChar, -1, sti(NPChar.index));
			SetCharacterRemovable(NPChar, false);
			
			SetBaseShipData(NPChar);
            DeleteAttribute(NPChar,"ship.sails");
			DeleteAttribute(NPChar,"ship.masts");
			DeleteAttribute(NPChar,"ship.blots");
			
			Fantom_SetCannons(NPChar, "pirate");
			Fantom_SetBalls(NPChar, "pirate");
		}
		NextDiag.CurrentNode = "Quest_1_Ship_Accept";
		DialogExit();
		break;

	case "Quest_1_Ship_Refuse":
		Dialog.Text = RandSwear() + "Вы меня разочаровали, капитан. А вы думали, что я вас приглашу на "+ RandPhraseSimple("воскресную мессу?!!", "детский утренник!??"));
		link.l1 = PCharRepPhrase(RandPhraseSimple("Это твое дело. А решать мое.", 
					"Не хочу сушиться на солнышке из-за пары пиастров."), 
				RandPhraseSimple("Мое решение окончательное, капитан.", 
					"Нам не о чем больше говорить."));
		link.l1.go = "Exit_Quest_1_Refuse";
		break;

	case "Exit_Quest_1_Refuse":
		if (sti(PChar.GenQuest.PGG_Quest.Stage) != -1)
		{
			PChar.GenQuest.PGG_Quest.Stage = -1;
			PChar.Quest.PGGQuest1_RemoveShip.win_condition.l1 = "ExitFromLocation";
			PChar.Quest.PGGQuest1_RemoveShip.win_condition.l1.Location = PChar.location;
			PChar.Quest.PGGQuest1_RemoveShip.function = "PGG_Q1RemoveShip";

			AddQuestRecord("Gen_PGGQuest1", "q1_Close");
			CloseQuestHeader("Gen_PGGQuest1");
			DeleteAttribute(NPChar, "AlwaysFriend");
		}
		NPChar.Dialog.CurrentNode = "Quest_1_Ship_Refuse";
		NPChar.DeckDialogNode = "Quest_1_Ship_Refuse";
		DialogExit();
		break;

	case "Quest_1_Ship_BadWay":
		Dialog.Text = PCharRepPhrase(RandPhraseSimple("Да брось, все пройдет как по маслу и никто не узнает. Мы всех зарежем.", 
					"Узнал все и сам хочешь дельце провернуть? Не выйдет!"), 
				RandPhraseSimple("Куда это вы заторопились, капитан? Уж не к властям ли?", 
					"Весьма неумно влезть в логово тигра и думать, что сможешь запросто выбраться оттуда."));
		link.l1 = PCharRepPhrase(RandSwear() + RandPhraseSimple("Убирайся к морскому дьяволу! Я капитан " + GetFullName(PChar) + " и ты мне не указ! Я ухожу!", 
					"Клянусь дьяволом, ты меня выводишь из себя!"), 
				RandPhraseSimple("Ваше предложение мне неинтересно и говорить нам более не о чем. Прощайте.", 
					"Полагаю, нам не о чем более говорить. Меня ждут дела."));
		link.l1.go = "Quest_1_Ship_BadWay_1";
		break;

	case "Quest_1_Ship_BadWay_1":
		Dialog.Text = RandSwear() + PCharRepPhrase(RandPhraseSimple("Схватить его! Привяжите его к пушке! Посмотрим, какого цвета у него кишки!!!", 
					"Эй, Джонни! Хендерсон! Хватайте мерзавца! Не пускайте его к шлюпке!!!"), 
				RandPhraseSimple("Жаль, капитан! Надеюсь, в нашем трюме тебе будет уютно. Ну, а потом решим, что с тобой делать.", 
					"Белые рабы долго не живут, капитан,  так что решайте: или Хендс прострелит вам голову из мушкета или вы остаток жизни проведете в каменоломнях."));
		link.l1 = PCharRepPhrase(RandPhraseSimple("Я заставлю тебя жрать твои поганые внутренности, " + GetFullName(NPChar) + "!!!", 
					"А ну прочь с дороги! Мерзавцы! Раскрою голову как гнилую тыкву!"), 
				RandPhraseSimple("Ваше предложение неприемлемо... И весьма неблагоразумно!!!", 
					"А вы не очень гостепреимны... С дороги!!!"));
		link.l1.go = "Exit_Quest_1_DeckFight";
		break;

	case "Exit_Quest_1_DeckFight":
        GetCharacterPos(NPChar, &locx, &locy, &locz);
		DeleteAttribute(NPChar, "AlwaysFriend");
        sTmp = LAi_FindNearestFreeLocator("reload", locx, locy, locz);
		LAi_SetImmortal(NPChar, true);
		LAi_SetActorType(NPChar);
		LAi_ActorRunToLocation(NPChar, "reload", sTmp, "none", "", "", "", 5.0);

		LAi_LocationFightDisable(&Locations[FindLocation(pchar.location)], false);
		//группа может быть занята, а проверок по группам у нас нет... :(
		if (!CheckAttribute(pchar, "questTemp.PGGContra.Know"))
		{
			sTmp = "PGGTmp";
		}
		else
		{
			sTmp = "PGGQst";
		}
		PChar.GenQuest.PGG_Quest.GrpID = sTmp;

		for (i=1; i<5; i++)
		{
			sld = CharacterFromID("saylor_0" + i);
			LAi_group_MoveCharacter(sld, sTmp);
		}
		LAi_group_FightGroups(LAI_GROUP_PLAYER, sTmp, true);
		LAi_group_SetCheck(sTmp, "PGG_Q1AfterDeckFight");
		chrDisableReloadToLocation = true;

		PGG_ChangeRelation2MainCharacter(NPChar, -20);
		NextDiag.CurrentNode = NextDiag.TempNode;
		DialogExit();
		LAi_SetFightMode(pchar, true);
		break;

	case "Quest_1_Time2Late":
		chrDisableReloadToLocation = false;

        
		//перенес сюда.. х.з. вроде будет лучше (Баг Изгоя.)
		PChar.Quest.PGGQuest1_PGGDead.Over = "Yes";
		PChar.Quest.PGGQuest1_GroupDead.Over = "Yes";
		PChar.Quest.PGGQuest1_CheckStartState.Over = "Yes";

//		Dialog.Text = "Так, " + GetFullName(PChar) + ", что-то зря я с тобой связался, добыча утекла сквозь пальцы как вода. Прощай!";
		Dialog.Text = PCharRepPhrase(RandPhraseSimple("Гром и молния! Мы опоздали! А все из-за вас, ленивые олухи! Нечего нам с тобой теперь делить, " + GetFullName(PChar) + ". Поднять стаксель! Травить марс помалу! ", 
					"Так, " + GetFullName(PChar) + ", что-то зря я с тобой связался, добыча утекла сквозь пальцы как вода. Прощай!"), 
				RandPhraseSimple("Они исчезли! Виной всему наша нерасторопность! Нам с вами, не по пути " + GetFullName(PChar) + ".", 
					"Караван мы не догоним, можно сушить весла. Зря я с вами связался! Прощайте."))
		link.l1 = PCharRepPhrase(RandPhraseSimple("Да и черт с этим караваном! Ловцы жемчуга ничуть не хуже, а риска никакого!", 
					"Удача с нами, еще не вечер! Море изобилует жалкими торгашами, ждущими пока мы отнимем их денежки!"), 
				RandPhraseSimple("Действительно жаль, что такая блестящая операция закончилась полным фиаско! Прощайте!", 
					"Человек предполагает, а бог располагает... Прощайте!"));
		link.l1.go = "Exit_Quest_1_2Late";

		AddQuestRecord("Gen_PGGQuest1", "q1_Time2Late");
		CloseQuestHeader("Gen_PGGQuest1");
		break;

	case "Exit_Quest_1_2Late":
        LAi_SetPlayerType(PChar);
		if (sti(PChar.GenQuest.PGG_Quest.Stage) != -1)
		{
			PChar.GenQuest.PGG_Quest.Stage = -1;
			RemoveCharacterCompanion(pchar, NPChar);

			NPChar.PGGAi.IsPGG = true;
			NPChar.RebirthPhantom = true;

			LAi_SetWarriorTypeNoGroup(NPChar);
			LAi_group_MoveCharacter(NPChar, "GroupDeck");
			LAi_SetImmortal(NPChar, false);
			LAi_LocationFightDisable(&Locations[FindLocation("Ship_deck")], false);

			PChar.Quest.PGGQuest1_Clear.win_condition.l1 = "ExitFromLocation";
			PChar.Quest.PGGQuest1_Clear.win_condition.l1.Location = PChar.location;
			if (PChar.location == "Ship_deck")
			{
/*				PChar.Quest.PGGQuest1_RemoveShip.win_condition.l1 = "ExitFromSea";
				PChar.Quest.PGGQuest1_RemoveShip.function = "PGG_Q1RemoveShip";
				PChar.Quest.PGGQuest1_Clear.win_condition.l1 = "ExitFromSea";
*/				PChar.quest.Munity = "Deads";
			}
			PChar.Quest.PGGQuest1_Clear.function = "PGG_Q1EndClear";
		}
		NextDiag.CurrentNode = "Quest_1_Failed";
		DialogExit();
		break;

	case "Quest_1_CheckResult":
		if (PChar.location == PChar.GenQuest.PGG_Quest.Island.Shore)
		{
			Locations[FindLocation(PChar.GenQuest.PGG_Quest.Island.Shore)].DisableEncounters = false;
		}
		else
		{
			PChar.quest.Munity = "Deads";
		}
		chrDisableReloadToLocation = false;

		//не взяли груз...
		if (CheckAttribute(NPChar, "PGGAi.ActiveQuest"))
		{
			PChar.GenQuest.PGG_Quest.FailedPaySum = sti(PChar.GenQuest.PGG_Quest.Days)*10000;
//			Dialog.Text = "Нда, связался я с тобой. Все погубил. Ты теперь мне должен неустойку в размере " + FindRussianMoneyString(sti(PChar.GenQuest.PGG_Quest.FailedPaySum)) + ".";
			Dialog.Text = PCharRepPhrase(RandPhraseSimple("Кровожадный ублюдок! Все ушло на дно к морскому дьяволу! " + FindRussianMoneyString(sti(PChar.GenQuest.PGG_Quest.FailedPaySum)) + " пиастров на бочку и видеть тебя больше не желаю!", 
						"Ты себя адмиралом Нельсоном вообразил? Потопил всю добычу, идиот! Вези мне " + FindRussianMoneyString(sti(PChar.GenQuest.PGG_Quest.FailedPaySum)) + " пиастров и проваливай!"), 
					RandPhraseSimple("Вы здесь войну решили устроить! Это совершенно никуда не годится! Полагаю, мы все забудем, если вы уплатите нам " + FindRussianMoneyString(sti(PChar.GenQuest.PGG_Quest.FailedPaySum)) + " пиастров ", 
						"Ваши методы недопустимы в такого рода делах! Вы провалили все дело! Немедленно уплатите нашу долю в размере " + FindRussianMoneyString(sti(PChar.GenQuest.PGG_Quest.FailedPaySum)) + " пиастров и отправляйтесь на все четыре стороны! "));
			link.l1 = PCharRepPhrase(RandPhraseSimple("Если бы не я, ты бы сейчас крабов кормил, жадная образина!", 
						"Лучше было позволить флагману взять на абордаж твое корыто, тогда бы ты уже болтался на рее и не отравлял воздух! "), 
					RandPhraseSimple("Ваши требования неуместны, а намеки оскорбительны!", 
						"Договора о неустойке не было, стало быть и платить я не буду!"));
			link.l1.go = "Quest_1_NotPay";
			if (sti(pchar.money) >= sti(PChar.GenQuest.PGG_Quest.FailedPaySum))
			{
				link.l2 = PCharRepPhrase(RandPhraseSimple("Я не могу тебе дать сейчас бой и  ты это знаешь! Подавись своими пиастрами! Вот!", 
								"Пиастры!!! Что же, фортуна сейчас не на моей стороне. Я согласен."), 
							RandPhraseSimple("Ваши требования возмутительны, но выбора у меня, похоже, нет.", 
								"Ваши аппетиты чрезмерны, но я вынужден согласиться!"));
				link.l2.go = "Exit_Quest_1_Failed";
			}
		}
		else
		{
			PChar.GenQuest.PGG_Quest.Stage = -1;
			Dialog.Text = "Нда, связался я с тобой. Все погубил. Не умеешь -  не берись, и других не втягивай.";
			link.l1 = "Да ну! Сам же видел не получалось.";
			link.l1.go = "Exit_Quest_1_Failed";
		}

		//минимум треть если взял, то гуд!
		if (sti(PChar.GenQuest.PGG_Quest.Goods.Taken) > MakeInt(sti(PChar.GenQuest.PGG_Quest.Goods.Qty)/3))
		{
			PChar.GenQuest.PGG_Quest.Ok = 1;
//			Dialog.Text = "Итак, добыча составила " + PChar.GenQuest.PGG_Quest.Goods.Taken + " " + PChar.GenQuest.PGG_Quest.Goods.Text + ". Давай делить. ";
			Dialog.Text = PCharRepPhrase("Жаркое дельце! Добыча составила " + PChar.GenQuest.PGG_Quest.Goods.Taken + " " + PChar.GenQuest.PGG_Quest.Goods.Text + ".", 
					"Отлично сработали, капитан! Добыча составила " + PChar.GenQuest.PGG_Quest.Goods.Taken + " " + PChar.GenQuest.PGG_Quest.Goods.Text + ".");
			i = sti(PChar.GenQuest.PGG_Quest.Parts);
			PChar.GenQuest.PGG_Quest.Goods.Part = MakeInt(sti(PChar.GenQuest.PGG_Quest.Goods.Taken) / i);
			if (CheckAttribute(NPChar, "PGGAi.ActiveQuest"))
			{
				Dialog.Text = Dialog.Text + PCharRepPhrase(" Моя доля ", "Доля, приходящаяся на мое судно - ");
			}
			else
			{
				Dialog.Text = Dialog.Text + " Твоя доля ";
			}
			Dialog.Text = Dialog.Text + PChar.GenQuest.PGG_Quest.Goods.Part + ".";
			link.l1 = PCharRepPhrase(RandPhraseSimple("Справедливо! Уже грузят шлюпки!", "Да, все точно... Уговор дороже денег."), 
					RandPhraseSimple("Ваши расчеты правильны. Я согласен.", "В дележе вы безупречны."));
			link.l1.go = "Exit_Quest_1_End";
			link.l2 = PCharRepPhrase(RandPhraseSimple("И ты еще требуешь свою долю? Да тебя надо вздернуть  на рее и не мешкать!", 
						"Долю? Здесь каждый сам за себя, и если тебе ничего не досталось, то это твои проблемы!"), 
					RandPhraseSimple("Не думаю, что вы на что-то можете рассчитывать.", 
						"Мой ответ – нет! Я не собираюсь делиться с разными проходимцами!"));
			link.l2.go = "Quest_1_NotPay";
		}
		else
		{
			AddQuestRecord("Gen_PGGQuest1", "q1_Fail");
		}
		break;

	case "Quest_1_NotPay":
		Dialog.Text = PCharRepPhrase(RandPhraseSimple("Нет, так нет! Спорить не буду! Прощай!", 
					"Не будем проливать кровь и усугублять неудачу! В другой раз повезет больше!"), 
				RandPhraseSimple("Что же, прощайте, капитан, я не в том положении, чтобы настаивать...", 
					"Повреждения моего корабля и потери не позволяют мне настаивать на этом справедливом решении вопроса. Оставлю это на вашей совести, капитан!"));
		link.l1 = PCharRepPhrase(RandPhraseSimple("Попутного ветра!", "Семь футов под килем!"), 
				RandPhraseSimple("Прощайте! Полагаю, свежий морской воздух пойдет вам на пользу!", 
					"Надеюсь, что ваши неудачи не сделают ваш характер еще более несносным, прощайте!"));
		link.l1.go = "Exit_Quest_1_Failed";
		PChar.GenQuest.PGG_Quest.Stage = -1;

		bOk = makeint(NPChar.reputation) < 41 && PGG_ChangeRelation2MainCharacter(NPChar, 0) < 75;
		if (bOk || CheckAttribute(PChar, "GenQuest.PGG_Quest.Ok"))
		{
			PChar.GenQuest.PGG_Quest.Stage = 4;
			Dialog.Text = PCharRepPhrase(RandPhraseSimple("Ты не понял! Этим ты мог бы купить себе жизнь! Схватить ублюдка!", 
						"Ты, наверное, думаешь, что с тобой шутят? Пропустим тебя под килем, и сразу станешь сговорчивей!"), 
					RandPhraseSimple("Придется подсократить вашу жизнь, капитан! Не хотите по хорошему, познакомитесь со святым Мартином!", 
						"Вы не оставляете мне выбора, " + GetFullName(PChar) + "! Я возьму свое, хотите вы того или нет!"));
			link.l1 = PCharRepPhrase(RandPhraseSimple("Клянусь дьяволом, ты за это заплатишь!!", 
						"Йхо-хо! Иди и возьми меня, капитана " + GetFullName(PChar) + "! "), 
					RandPhraseSimple("Разрез будет глубокий и длинный! ", 
						"Вы хотите сразу умереть или немного помучится?"));
			link.l1.go = "Exit_Quest_1_FailedFight";
		}
		break;

	case "Exit_Quest_1_Failed":
		if (sti(PChar.GenQuest.PGG_Quest.Stage) != -1)
		{
			AddMoneyToCharacter(PChar, -(sti(PChar.GenQuest.PGG_Quest.FailedPaySum)));
			AddQuestRecord("Gen_PGGQuest1", "q1_FailPay");
		}
		CloseQuestHeader("Gen_PGGQuest1");
		RemoveCharacterCompanion(pchar, NPChar);

		if (PChar.Location == "Ship_deck")
		{
			PChar.Quest.PGGQuest1_FailedExitSea.win_condition.l1 = "ExitFromSea";
			PChar.Quest.PGGQuest1_FailedExitSea.function = "PGG_Q1EndClear";
		}
		else
		{
			PChar.Quest.PGGQuest1_FailedExitLoc.win_condition.l1 = "ExitFromLocation";
			PChar.Quest.PGGQuest1_FailedExitLoc.win_condition.l1.Location = PChar.location;
			PChar.Quest.PGGQuest1_FailedExitLoc.function = "PGG_Q1EndClear";
		}
		PGG_ChangeRelation2MainCharacter(NPChar, -5);

		LAi_SetImmortal(NPChar, true);
		//что б не били сопровождение.
		for (i = 1; i < sti(PChar.GenQuest.PGG_Quest.GrpID.Qty); i++)
		{
			sld = CharacterFromID("pirate_" + i);
			LAi_SetImmortal(sld, true);
		}

		NextDiag.CurrentNode = "Quest_1_Failed";
		DialogExit();
		if (NPChar.Chr_Ai.Type == "actor") LAi_SetWarriorTypeNoGroup(NPChar);
		break;

	case "Quest_1_Failed":
		Dialog.Text = "Не хочу больше иметь с тобой дел!";
		link.l1 = "Да и не нужно.";
		link.l1.go = "Exit";
		NextDiag.TempNode = "Quest_1_Failed";
		break;

	case "Quest_1_End":
		Dialog.Text = "Приятно иметь с тобой дело! Удачи.";
		link.l1 = "И тебе.";
		link.l1.go = "Exit";
		NextDiag.TempNode = "Quest_1_End";
		break;

	case "Exit_Quest_1_FailedFight":
		RemoveCharacterCompanion(pchar, NPChar);

		LAi_LockFightMode(pchar, false);
		LAi_LocationFightDisable(&Locations[FindLocation(PChar.location)], false);
		LAi_group_SetRelation("PGGTmp", LAI_GROUP_PLAYER, LAI_GROUP_ENEMY);	
		LAi_group_FightGroups(LAI_GROUP_PLAYER, "PGGTmp", true);
		LAi_group_SetCheck("PGGTmp", "PGG_Q1AfterDeckFight");
	
		PChar.GenQuest.PGG_Quest.GrpID = "PGGTmp";
		chrDisableReloadToLocation = true;

		Group_AddCharacter("PGGQuest", NPChar.id);
		Group_SetGroupCommander("PGGQuest", NPChar.id);
		Group_SetAddress("PGGQuest", PChar.GenQuest.PGG_Quest.GrpLocation, "Quest_Ships", PChar.GenQuest.PGG_Quest.GrpLoc);
		Group_SetTaskNone("PGGQuest");

		PGG_ChangeRelation2MainCharacter(NPChar, -15);

		GetCharacterPos(NPChar, &locx, &locy, &locz);
        sTmp = LAi_FindNearestFreeLocator("reload", locx, locy, locz);
		NextDiag.CurrentNode = "Second Time";
		DialogExit();

		Log_TestInfo("go to loc " + sTmp + " " + NPChar.location);
		NPChar.location = PChar.location;
		if (PChar.location != "Ship_Deck") sTmp = "reload1_back";

		PChar.questTemp.Chr2Remove = NPChar.id;
		LAi_ActorRunToLocator(NPChar, "reload", sTmp, "RemoveCharacterFromLocation", 5.0);

		LAi_SetImmortal(NPChar, true);
		LAi_SetFightMode(pchar, true);
		break;

	case "Exit_Quest_1_End":
		RemoveCharacterCompanion(PChar, NPChar);

		i = sti(PChar.GenQuest.PGG_Quest.Goods.Part);
/*		i *= (sti(PChar.GenQuest.PGG_Quest.Parts)-1);
		i += sti(PChar.GenQuest.PGG_Quest.StartGoods);
		n = GetCompanionQuantity(PChar);
		SetCharacterGoods(PChar, sti(PChar.GenQuest.PGG_Quest.Goods), i);
		for (i = 1; i < n; i++)
		{
			iRnd = GetCompanionIndex(PChar, i);
			if (iRnd != -1)
			{
				sld = GetCharacter(iRnd);
				if (GetRemovable(sld))
				{
					Log_TestInfo("" + sld.id);
					SetCharacterGoods(sld, sti(PChar.GenQuest.PGG_Quest.Goods), 0);
				}
			}
		}
*/
		RemoveCharacterGoods(PChar, sti(PChar.GenQuest.PGG_Quest.Goods), i);
/*		if (PChar.Location == "Ship_deck")
		{
			PChar.Quest.PGGQuest1_EndExitSea.win_condition.l1 = "ExitFromSea";
			PChar.Quest.PGGQuest1_EndExitSea.function = "PGG_Q1EndClear";
		}
		else
		{*/
			PChar.Quest.PGGQuest1_EndExitLoc.win_condition.l1 = "ExitFromLocation";
			PChar.Quest.PGGQuest1_EndExitLoc.win_condition.l1.Location = PChar.location;
			PChar.Quest.PGGQuest1_EndExitLoc.function = "PGG_Q1EndClear";
		//}
		AddQuestRecord("Gen_PGGQuest1", "q1_OkShare");
		CloseQuestHeader("Gen_PGGQuest1");

		//что б не били сопровождение.
		for (i = 1; i < sti(PChar.GenQuest.PGG_Quest.GrpID.Qty); i++)
		{
			sld = CharacterFromID("pirate_" + i);
			LAi_SetImmortal(sld, true);
		}

		LAi_SetImmortal(NPChar, true);
		NextDiag.CurrentNode = "Quest_1_End";
		DialogExit();
  		LAi_SetWarriorType(NPChar); // сброс группы ГГ
		LAi_group_MoveCharacter(NPChar, LAI_GROUP_PLAYER);
		break;

	case "Quest_1_SharePrise":
		chrDisableReloadToLocation = false;
		Dialog.Text = PCharRepPhrase("Настоящее побоище, жаль девок с ними не было! Добыча составила " + PChar.GenQuest.PGG_Quest.Goods.Taken + " " + PChar.GenQuest.PGG_Quest.Goods.Text + ".", "Неплохо, капитан! Добыча составила " + PChar.GenQuest.PGG_Quest.Goods.Taken + " " + PChar.GenQuest.PGG_Quest.Goods.Text + ".");

		i = sti(PChar.GenQuest.PGG_Quest.Parts);
		PChar.GenQuest.PGG_Quest.Goods.Part = MakeInt(sti(PChar.GenQuest.PGG_Quest.Goods.Taken) / i);
        SetCharacterGoods(PChar, sti(PChar.GenQuest.PGG_Quest.Goods), sti(PChar.GenQuest.PGG_Quest.Goods.Taken) + GetCargoGoods(PChar, sti(PChar.GenQuest.PGG_Quest.Goods)));
		if (CheckAttribute(NPChar, "PGGAi.ActiveQuest"))
		{
			Dialog.Text = Dialog.Text + " Моя доля ";
		}
		else
		{
			Dialog.Text = Dialog.Text + " Твоя доля ";
		}
		Dialog.Text = Dialog.Text + PChar.GenQuest.PGG_Quest.Goods.Part + ".";
		link.l1 = PCharRepPhrase(RandPhraseSimple("Справедливо, разрази меня гром!", "Да, все точно, разрази меня гром!"), 
			RandPhraseSimple("Ваши расчеты правильны. Я согласен.", "В дележе вы безупречны."));
		link.l1.go = "Exit_Quest_1_End";
		break;
//===================================
//              Exits 
//===================================
	case "Exit":
		NextDiag.CurrentNode = NextDiag.TempNode;
		DialogExit();
		if (NPChar.Chr_Ai.Type == "actor") LAi_SetWarriorTypeNoGroup(NPChar);
		break;
		
	case "Exit_2":
		if (CheckAttribute(pchar, "GenQuest.PGG_Quest")) DeleteAttribute(pchar, "GenQuest.PGG_Quest"); //fix
		NextDiag.CurrentNode = NextDiag.TempNode;
		DialogExit();
		if (NPChar.Chr_Ai.Type == "actor") LAi_SetWarriorTypeNoGroup(NPChar);
		break;		

	case "Exit_Smugglers_Fight":
		NextDiag.CurrentNode = NextDiag.TempNode;
		DialogExit();

		LAi_group_FightGroups(LAI_GROUP_PLAYER, "PGGTmp", true);
		LAi_SetFightMode(pchar, true);
		break;

	case "Exit_ShowParam":
		NextDiag.CurrentNode = "Second Time";
//		NPChar.quest.meeting = true;
		DialogExit();

		PChar.SystemInfo.OnlyShowCharacter = true;
		LaunchCharacter(NPChar);
		break;
	}
}
/* TEMPLATE
		Dialog.Text = "";
		link.l1 = "";
		link.l1.go = "";
		link.l2 = "";
		link.l2.go = "";
		link.l3 = "";
		link.l3.go = "";
*/