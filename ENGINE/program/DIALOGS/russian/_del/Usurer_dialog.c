//nclude "DIALOGS\Usurer_dialog.h"
void ProcessDialogEvent()
{
	int LoanSum, LoanInterest, LoanPeriod, LoanResult, iPastMonths, DepositSum, DepositInterest, DepositResult;
	ref NPChar, d;

	aref Link, Diag;
	string NPC_Area;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makeref(d, Dialog);
	makearef(Diag, NPChar.Dialog);

	NPC_Area = Npchar.quest.area;

	switch(Dialog.CurrentNode)
	{
		// -----------------------------------Диалог первый - первая встреча
		case "LoanGranted_exit":
			Diag.CurrentNode = Diag.TempNode;

			AddMoneyToCharacter(Pchar, makeint(Pchar.Quest.Loans.(NPC_Area).Sum));
			Pchar.Quest.Loans.(NPC_Area) = true;
			Pchar.Quest.Loans.(NPC_Area).StartDay = getDataDay();
			Pchar.Quest.Loans.(NPC_Area).StartMonth = getDataMonth();
			Pchar.Quest.Loans.(NPC_Area).StartYear = getDataYear();
			Pchar.Quest.Loans.(NPC_Area).StartTime = getTime();

			Pchar.quest.(NPC_Area).win_condition.l1 = "Timer";
			Pchar.quest.(NPC_Area).win_condition.l1.date.day = getDataDay();
			Pchar.quest.(NPC_Area).win_condition.l1.date.month = getDataMonth() + makeint(Pchar.Quest.Loans.(NPC_Area).Period);
			Pchar.quest.(NPC_Area).win_condition.l1.date.year = getDataYear();
			Pchar.quest.(NPC_Area).win_condition = "Loans_" + (NPC_Area);

			DialogExit();

		break;

		case "Deposit_exit":
			Diag.CurrentNode = Diag.TempNode;

		//	DepositSum = -DepositSum;
			AddMoneyToCharacter(Pchar, -(makeint(Pchar.Quest.Deposits.(NPC_Area).Sum)));
			Pchar.Quest.Deposits.(NPC_Area) = true;
			Pchar.Quest.Deposits.(NPC_Area).StartDay = getDataDay();
			Pchar.Quest.Deposits.(NPC_Area).StartMonth = getDataMonth();
			Pchar.Quest.Deposits.(NPC_Area).StartYear = getDataYear();
			Pchar.Quest.Deposits.(NPC_Area).StartTime = getTime();
			DialogExit();
		break;

		case "exit":
			Diag.CurrentNode = Diag.TempNode;
			DialogExit();
			DeleteAttribute(Pchar, "quest.Loans." + (NPC_Area));
			DeleteAttribute(Pchar, "quest.Deposits." + (NPC_Area));
		break;

		case "exit_NoChange":
			Diag.CurrentNode = Diag.TempNode;
			DialogExit();
		break;


		case "First time":
			if(NPChar.quest.meeting == "0")
			{
				NPChar.quest.meeting = 1;
				Dialog.snd1 = "voice\USDI\USDI001";
				Dialog.snd2 = "voice\USDI\USDI002";
				Dialog.snd3 = "voice\USDI\USDI003";
				d.Text = RandPhrase(TimeGreeting() + ", чем  могу вам помочь?", "Рад приветствовать вас, " + address_form.eng + ". Вам нужны мои услуги?", "Здравствуйте, " + address_form.eng + ". Кажется, я не имею чести вас знать...", &Dialog, Dialog.snd1, Dialog.snd2, Dialog.snd3);
				link.l1 = LinkRandPhrase("Меня зовут " + Pchar.name + " " + Pchar.lastname + ".", "Капитан " + Pchar.name + " " + Pchar.lastname + " к вашим услугам.", "Я " + PChar.name + ".");
				link.l1.go = "meeting";
			}
			else
			{
					Dialog.snd1 = "voice\USDI\USDI004";
					Dialog.snd2 = "voice\USDI\USDI005";
					Dialog.snd3 = "voice\USDI\USDI006";
					d.Text = RandPhrase("Рад видеть вас, капитан " + PChar.name + ". Если вам нужны мои услуги, то я готов вам их оказать немедленно.", TimeGreeting() + ". Вы по делу?", "Здравствуйте, " + address_form.eng + ". Что вам угодно? Я готов осуществить любые финансовые операции.", &Dialog, Dialog.snd1, Dialog.snd2, Dialog.snd3);
					link.l1 = LinkRandPhrase("Я хочу занять денег.", "Мне срочно понадобилась пара-другая пиастров.", "Как насчет небольшого кредита?");
					link.l1.go = "loan";
					if(makeint(Pchar.money) >= 100)
					{
						link.l2 = LinkRandPhrase("Я бы хотел отдать деньги в рост.", "Примете на хранение пиратскую добычу?", "Золотишко на черный день оставить можно?");
						link.l2.go = "deposit";
					}
					link.l3 = "До свидания, " + NPchar.name + ".";
					link.l3.go = "exit";

				if(CheckAttribute(Pchar, "Quest.Loans." + (NPC_Area)) && makeint(Pchar.Quest.Loans.(NPC_Area)) == true)
				{
					iPastMonths = GetPastTime("Month", makeint(Pchar.Quest.Loans.(NPC_Area).StartYear),makeint(Pchar.Quest.Loans.(NPC_Area).StartMonth),makeint(Pchar.Quest.Loans.(NPC_Area).StartDay), makefloat(Pchar.Quest.Loans.(NPC_Area).StartTime), getDataYear(),getDataMonth(),GetDataDay(), GetTime());
					Pchar.Quest.Loans.(NPC_Area).Result = makeint(Pchar.Quest.Loans.(NPC_Area).Sum) + ((makeint(Pchar.Quest.Loans.(NPC_Area).Sum)/100)*makeint(Pchar.Quest.Loans.(NPC_Area).Interest))*iPastMonths;

					Dialog.snd1 = "voice\USDI\USDI007";
					Dialog.snd2 = "voice\USDI\USDI008";
					Dialog.snd3 = "voice\USDI\USDI009";

					d.Text = RandPhrase(TimeGreeting() + "," + PChar.name + ". Вы пришли, чтобы со мной расплатиться? Ага... Так, вы должны мне " + Pchar.Quest.Loans.(NPC_Area).Result + " пиастров.", "Здравствуйте, капитан. Рад видеть вас в добром здравии. Готовы ли вы вернуть те " + Pchar.Quest.Loans.(NPC_Area).Result + " пиастров, что занимали у меня?", "Приветствую вас, " + address_form.eng + ". Надеюсь, ваше плавание было удачным, и вы принесли мне мои деньги. Считая проценты, вы сейчас должны мне " + Pchar.Quest.Loans.(NPC_Area).Result + " пиастров.", &Dialog, Dialog.snd1, Dialog.snd2, Dialog.snd3);
					if(makeint(PChar.money) >= makeint(Pchar.Quest.Loans.(NPC_Area).Result))
					{
						link.l1 = LinkRandPhrase("Я готов вернуть вам ваши деньги.", "Я в состоянии выплатить вам долг.", "Настало время, наконец, рассчитаться.");
						link.l1.go = "loan_return";
					}
					link.l2 = LinkRandPhrase("К сожалению, я пока не могу этого сделать.", "В следующий раз, " + NPChar.name + ".", "Увы, нет. Я вернусь, когда достану деньги.");
					link.l2.go = "Loan_Remind";
					link.l3 = "";
					link.l3.go = "exit_NoChange";

				}
				if(CheckAttribute(Pchar, "Quest.Deposits." + (NPC_Area)) && makeint(Pchar.Quest.Deposits.(NPC_Area)) == true)
				{
					iPastMonths = GetPastTime("Month", makeint(Pchar.Quest.Deposits.(NPC_Area).StartYear),makeint(Pchar.Quest.Deposits.(NPC_Area).StartMonth),makeint(Pchar.Quest.Deposits.(NPC_Area).StartDay), makefloat(Pchar.Quest.Deposits.(NPC_Area).StartTime), getDataYear(),getDataMonth(),GetDataDay(), GetTime());
					Pchar.Quest.Deposits.(NPC_Area).Result = makeint(Pchar.Quest.Deposits.(NPC_Area).Sum) + ((makeint(Pchar.Quest.Deposits.(NPC_Area).Sum)/100)*makeint(Pchar.Quest.Deposits.(NPC_Area).Interest))*iPastMonths;
					Dialog.snd1 = "voice\USDI\USDI010";
					Dialog.snd2 = "voice\USDI\USDI011";
					Dialog.snd3 = "voice\USDI\USDI012";
					d.Text = RandPhrase("Рад видеть вас, капитан " + PChar.name + ". Если вам нужны мои услуги, то я готов вам их оказать немедленно.", TimeGreeting() + ". Вы по делу?", "Здравствуйте, " + address_form.eng + ". Что вам угодно? Я готов осуществить любые финансовые операции.", &Dialog, Dialog.snd1, Dialog.snd2, Dialog.snd3);
					link.l1 = LinkRandPhrase("Я хочу забрать свой вклад.", "Пришло время забрать мои деньги, что у вас хранятся.", "Мне нужно мое золото и набежавшие проценты.");
					link.l1.go = "Deposit_return";
					link.l2 = "До свидания, " + NPchar.name + ".";
					link.l2.go = "exit_NoChange";
					link.l3 = "";
					link.l3.go = "exit_NoChange";
				}
			}
		break;

		case "Meeting":
			Dialog.snd1 = "voice\USDI\USDI013";
			Dialog.snd2 = "voice\USDI\USDI014";
			Dialog.snd3 = "voice\USDI\USDI015";
			d.Text = RandPhrase("Очень приятно. Я " + NPChar.name + " " + NPChar.lastname + ". Мое занятие - предоставление денег в долг под проценты.", "Мое имя " + NPChar.name + " " + NPChar.lastname + ". Я ростовщик. Если вам внезапно понадобятся средства - я буду рад вам помочь.", "Наверняка вы уже знаете, что меня зовут " + NPChar.name + " " + NPChar.lastname + ". Я довольно известная здесь личность...Впрочем, перейдем к делу - вам нужны мои услуги?", &Dialog, Dialog.snd1, Dialog.snd2, Dialog.snd3);
			link.l1 = LinkRandPhrase("Я хочу занять денег.", "Мне срочно понадобилась пара-другая пиастров.", "Как насчет небольшого кредита?");
			link.l1.go = "loan";
			if(makeint(Pchar.money) >= 100)
			{
				link.l2 = LinkRandPhrase("Я бы хотел отдать деньги в рост.", "Примете на хранение пиратскую добычу?", "Золотишко на черный день оставить можно?");
				link.l2.go = "deposit";
			}
			link.l3 = "До свидания, " + NPchar.name + ".";
			link.l3.go = "exit";
		break;

		case "loan":
			Dialog.snd = "voice\USDI\USDI016";
			d.Text = "О, всегда пожалуйста. Какая сумма вам необходима?";
			link.l1 = "Совсем небольшая.";
			link.l1.go = "Small";
			link.l2 = "Весьма значительная.";
			link.l2.go = "Medium";
			link.l3 = "Чем больше, тем лучше.";
			link.l3.go = "Large";
		break;

		case "small":
			Pchar.Quest.Loans.(NPC_Area).Sum = 500*makeint(Pchar.rank);
			Dialog.snd = "voice\USDI\USDI017";
			d.Text = "Отлично. С небольшими суммами всегда проще - меньше риска. Я могу предложить вам " + Pchar.Quest.loans.(NPC_Area).sum + " пиастров...";
			link.l1 = "Договорились. Под какие проценты?";
			link.l1.go = "Interest";
			link.l2 = "Увы, эта сумма не годится. Давайте-ка ее изменим.";
			link.l2.go = "Loan";
			link.l3 = "Думаю, мне лучше не влезать в долги. До свидания.";
			link.l3.go = "Exit";
		break;

		case "Medium":
			Pchar.Quest.Loans.(NPC_Area).Sum = 1500*makeint(Pchar.rank);
			Dialog.snd = "voice\USDI\USDI018";
			d.Text = "Не проблема. Надеюсь, что " + Pchar.Quest.Loans.(NPC_Area).sum + " пиастров помогут вам решить ваши проблемы. Это вполне значительная сумма.";
			link.l1 = "Договорились. Под какие проценты?";
			link.l1.go = "Interest";
			link.l2 = "Увы, эта сумма не годится. Давайте-ка ее изменим.";
			link.l2.go = "Loan";
			link.l3 = "Думаю, мне лучше не влезать в долги. До свидания.";
			link.l3.go = "Exit";
		break;

		case "Large":
			Pchar.Quest.Loans.(NPC_Area).Sum = 3000*makeint(Pchar.rank);
			Dialog.snd = "voice\USDI\USDI019";
			d.Text = "М-да... Рискованно... Но ладно, я готов предоставить вам заем на сумму " + Pchar.Quest.Loans.(NPC_Area).sum + ". Как вы понимаете, капитан, это действительно крупные деньги. Очень прошу вас серьезно отнестись к этому делу.";
			link.l1 = "Договорились. Под какие проценты?";
			link.l1.go = "Interest";
			link.l2 = "Увы, эта сумма не годится. Давайте-ка ее изменим.";
			link.l2.go = "Loan";
			link.l3 = "Думаю, мне лучше не влезать в долги. До свидания.";
			link.l3.go = "Exit";
		break;

		case "Interest":
			Pchar.Quest.Loans.(NPC_Area).Interest = 11 - makeint(Pchar.skill.commerce);
			Dialog.snd = "voice\USDI\USDI020";
			d.Text = Pchar.Quest.Loans.(NPC_Area).Interest + " в месяц. Основываясь на том, что я о вас знаю, я не могу предложить вам сейчас лучших условий.";
			link.l1 = "Меня это вполне устраивает. Осталось обговорить срок.";
			link.l1.go = "Period";
			link.l3 = "Думаю, мне лучше не влезать в долги. До свидания.";
			link.l3.go = "Exit";
		break;

		case "Period":
			Pchar.Quest.Loans.(NPC_Area).Period = makeint(makeint(Pchar.reputation)/20) + 1;
			Dialog.snd = "voice\USDI\USDI021";
			d.Text =  "Учитывая то, что я о вас слышал, " + Pchar.name + ", я буду ждать возврата этого долга в течение " + Pchar.Quest.Loans.(NPC_Area).Period + " месяцев. После чего, как вы понимаете, я приму меры.";
			link.l1 = "Что же, я с удовольствием принимаю ваши условия... И ваши деньги.";
			link.l1.go = "LoanGranted";
			link.l3 = "Увы, мы с вами не договорились. До свидания.";
			link.l3.go = "Exit";
		break;


		case "LoanGranted":
			Dialog.snd = "voice\USDI\USDI022";
			d.Text = "Искренне этому рад. Но позвольте мне вас предупредить. Я давно занимаюсь своим делом и знаю, как вернуть свои деньги. Так что если у вас и были какие мысли прикарманить мои деньги, лучше оставьте их\nНе хочу вас обидеть - просто предупреждаю.";
			link.l1 = "Хм... Ну-ну. Всего хорошего.";
			link.l1.go = "LoanGranted_exit";
		break;

		case "Loan_Remind":
			Dialog.snd = "voice\USDI\USDI023";
			d.Text = "Ну смотрите... Проценты капают. Да и времени у вас осталось не так уж много.";
			link.l1 = "Не волнуйтесь. До встречи.";
			link.l1.go = "exit_NoChange";
		break;

		case "loan_return":
			addMoneyToCharacter(Pchar, -(makeint(Pchar.Quest.Loans.(NPC_Area).Result)));
			DeleteAttribute(PChar, "quest.loans." + (NPC_Area));
			Pchar.quest.(NPC_Area).over = "Yes";
			Dialog.snd1 = "voice\USDI\USDI024";
			Dialog.snd2 = "voice\USDI\USDI025";
			Dialog.snd3 = "voice\USDI\USDI026";
			d.Text = RandPhrase("О, это просто чудесно! В любое время, как вам понадобятся деньги - я к вашим услугам.", "Благодарю. Очень приятно иметь дело с деловым человеком, умеющим вовремя вернуть долг. А то знаете, разное бывает...",  "О, не зря я был в вас столь уверен, " + address_form.fra + "! Надеюсь, это не последний раз, когда вы прибегаете к моим услугам!", &Dialog, Dialog.snd1, Dialog.snd2, Dialog.snd3);
			link.l1 = LinkRandPhrase("Я хочу занять денег.", "Мне срочно понадобилась пара-другая пиастров.", "Как насчет небольшого кредита?");
			link.l1.go = "loan";
			if(makeint(PChar.money) >= 100)
			{
				link.l2 = LinkRandPhrase("Я бы хотел отдать деньги в рост.", "Примете на хранение пиратскую добычу?", "Золотишко на черный день оставить можно?");
				link.l2.go = "deposit";
			}
			link.l3 = "До свидания, " + NPchar.name + ".";
			link.l3.go = "exit";
		break;

		case "deposit":
			Dialog.snd1 = "voice\USDI\USDI027";
			Dialog.snd2 = "voice\USDI\USDI028";
			Dialog.snd3 = "voice\USDI\USDI029";
			d.Text = RandPhrase("Я слышу разумного человека! Так сколько денег вы готовы отдать в рост?", "Хорошо. Поверьте мне, та сумма, что вы мне отдаете, будет ждать вас в целости и сохранности, и даже с процентами...А, кстати, какова эта сумма?", "Я смотрю, вы понимаете, что к чему в этой жизни. Сколько вы хотите отдать мне на хранение?", &Dialog, Dialog.snd1, Dialog.snd2, Dialog.snd3);
			link.l1 = "Четверть - " + makeint(makeint(PChar.money)/40)*10 + " пиастров.";
			link.l1.go = "quarter";
			link.l2 = "Половину - " + makeint(makeint(PChar.money)/20)*10 + " пиастров.";
			link.l2.go = "half";
			link.l3 = "Все, что у меня есть - " + makeint(makeint(PChar.money)/10)*10 + " пиастров.";
			link.l3.go = "All";
		break;

		case "quarter":
			Pchar.Quest.Deposits.(NPC_Area).Interest = makeint(Pchar.skill.commerce) + 1;
			Pchar.Quest.Deposits.(NPC_Area).Sum = makeint(makeint(PChar.money)/40)*10;
			Dialog.snd = "voice\USDI\USDI030";
			d.Text = "Хорошо. Процентов под эту сумму я готов вам предложить...мм...скажем, " + Pchar.Quest.Deposits.(NPC_Area).Interest + ". Разумеется, в месяц.";
			link.l1 = "Меня это устраивает";
			link.l1.go = "Deposit_placed";
			link.l2 = "Лучше я изменю сумму.";
			link.l2.go = "Deposit";
			link.l3 = "Кажется, мне не стоит расставаться с деньгами. Всего хорошего.";
			link.l3.go = "Exit";
		break;

		case "half":
			Pchar.Quest.Deposits.(NPC_Area).Interest = makeint(Pchar.skill.commerce) + 1;
			Pchar.Quest.Deposits.(NPC_Area).Sum = makeint(makeint(PChar.money)/20)*10;
			Dialog.snd = "voice\USDI\USDI031";
			d.Text = "Хорошо. Процентов под эту сумму я готов вам предложить... мм... скажем, " + Pchar.Quest.Deposits.(NPC_Area).Interest + ". Разумеется, в месяц.";
			link.l1 = "Меня это устраивает";
			link.l1.go = "Deposit_placed";
			link.l2 = "Лучше я изменю сумму.";
			link.l2.go = "Deposit";
			link.l3 = "Кажется, мне не стоит расставаться с деньгами. Всего хорошего.";
			link.l3.go = "Exit";
		break;

		case "All":
			Pchar.Quest.Deposits.(NPC_Area).Interest = makeint(Pchar.skill.commerce) + 1;
			Pchar.Quest.Deposits.(NPC_Area).Sum = makeint(makeint(PChar.money)/10)*10;
			Dialog.snd = "voice\USDI\USDI032";
			d.Text = "Хорошо. Процентов под эту сумму я готов вам предложить...мм...скажем, " + Pchar.Quest.Deposits.(NPC_Area).Interest + ". Разумеется, в месяц.";
			link.l1 = "Меня это устраивает";
			link.l1.go = "Deposit_placed";
			link.l2 = "Лучше я изменю сумму.";
			link.l2.go = "Deposit";
			link.l3 = "Кажется, мне не стоит расставаться с деньгами. Всего хорошего.";
			link.l3.go = "Exit";
		break;

		case "Deposit_placed":
			Dialog.snd = "voice\USDI\USDI033";
			d.Text = "Теперь вы можете быть уверены, что даже самая жестокая буря не оставит вас без средств к существованию. Если же вы решите забрать вклад, то приходите в любое время.";
			link.l1 = "Хорошо. Всего хорошего.";
			link.l1.go = "Deposit_Exit";
		break;

		case "Deposit_return":
			Dialog.snd = "voice\USDI\USDI034";
			d.Text = "Исходя из процентов, которые мы оговорили в прошлый раз, и, учитывая прошедшее время, я насчитал, что должен вам " + Pchar.Quest.Deposits.(NPC_Area).Result + " пиастров... Вы уверены, что хотите забрать эти деньги";
			link.l1 = "Абсолютно. Давайте их сюда.";
			link.l1.go = "Deposit_return_1";
			link.l2 = "Знаете, вы правы. Пусть они еще немного полежат. Всего хорошего.";
			link.l2.go = "Exit_noChange";
		break;

		case "Deposit_return_1":
			addMoneyToCharacter(Pchar, makeint(Pchar.Quest.Deposits.(NPC_Area).Result));
			DeleteAttribute(PChar,"quest.deposits." + (NPC_Area));
			Dialog.snd = "voice\USDI\USDI035";
			d.Text = "Ох... Даже жаль с ними расставаться. Я к ним уже как-то привык. Что же - приходите ко мне еще.";
			link.l1 = "Если понадобится - приду. Счастливо оставаться.";
			link.l1.go = "Exit";
		break;

		case "DeadMotherfucker":
			Dialog.Tempnode = "DeadMotherFucker_1";
			Dialog.snd = "voice\USDI\USDI035";
			d.Text = "Вы еще живы? Наверное, хотите вернуть мне деньги? К черту! Из-за вас я потерял столько, сколько вы мне в жизни не возместите! Я не успокоюсь, пока не увижу ваш труп!";
			link.l1 = "С подобными признаниями ты сам легко можешь стать трупом!";
			link.l1.go = "Exit";
		break;

		case "DeadMotherFucker_1":
			Dialog.snd = "voice\USDI\USDI035";
			d.Text = "Убирайтесь из моего дома! Глаза мои вас бы не видели!";
			link.l1 = "...";
			link.l1.go = "Exit";
		break;
	}
}

