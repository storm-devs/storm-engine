
void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

	string sTemp;

	switch(Dialog.CurrentNode)
	{
		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;	
		case "exit_over":			
			npchar.lifeDay = 0;
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;	
		case "good_all":
			dialog.text = NPCStringReactionRepeat("Рад" + NPCharSexPhrase(npchar, "","а") + " видеть вас снова. Вы что-то еще хотели?", 
				"О-о, опять вы? Сегодня у нас уже был разговор. Что-то изменилось?", 
				"Хм, может хватит мне надоедать без дела?!",
                "Бр-р-р, тошнит уже от твоей физиономии...", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat("Нет, ничего такого...", 
				"Нет, все нормально.",
                "Да, конечно, извини...", 
				"Хм, ну это зря, физиономия у меня что надо. Горжусь ей!", npchar, Dialog.CurrentNode);
			link.l1.go = "Exit";
			NextDiag.TempNode = "good_all";
		break;
		case "All_disagree":
			npchar.lifeDay = 0;
			NextDiag.TempNode = "All_bye";
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
		case "All_bye":
			dialog.text = "Я не желаю с вами разговаривать, прощайте.";
			link.l1 = "Хех, ну и не надо.";
			link.l1.go = "Exit";
			NextDiag.TempNode = "All_bye";
		break;
		//================================== в Бриджтауне ======================================
		case "PortSpein":
			NextDiag.TempNode = "PortSpein";
			dialog.text = "Здравствуйте, капитан. Не хотите поговорить о том, что вас, без сомнения, интересует?";
			link.l1 = "Нет, не хочу.";
			link.l1.go = "exit_over";			
			link.l2 = "А откуда ты знаешь, что может меня заинтересовать?";
			link.l2.go = "PortSpein_1";
			DeleteAttribute(npchar, "talker");
			npchar.LifeDay = 20;
			SaveCurrentNpcQuestDateParam(npchar, "LifeTimeCreate");
		break;
		case "PortSpein_1":
			dialog.text = "Слухи, " +  GetAddress_Form(NPChar) + ", ими земля полнится...";
			link.l1 = "Хех, ну давай, поговорим.";
			link.l1.go = "PortSpein_2";
		break;
		case "PortSpein_2":
			dialog.text = "Полагаю, что вам будет интересна информация о местонахождении одной очень нужной вещицы.";
			link.l1 = "Бесспорно.";
			link.l1.go = "PortSpein_3";
		break;
		case "PortSpein_3":
			dialog.text = "Очень хорошо. Но я должен быть уверен, что эта вещь попадет именно тому, кто сумеет верно ей распорядится - я обещал это одному человеку. Поэтому я предлагаю вам ответить на один вопрос, чтобы я мог понять, действительно ли вы тот, кто мне нужен. Согласны?";
			link.l1 = "Хех.. Ну, согласен.";
			link.l1.go = "PortSpein_4";
			link.l2 = "Поищи кого-нибудь другого и будь в нем уверен. Я - пас.";
			link.l2.go = "All_disagree";
		break;
		case "PortSpein_4":
			sTemp = "q" + npchar.quest.numQuestion;
			dialog.text = "В таком случае, слушайте вопрос. " + NullCharacter.questions.(sTemp);
			Link.l1.edit = 1;
			Link.l1 = "";
			Link.l1.go = "PortSpein_check";	
		break;
		case "PortSpein_check":
			sTemp = "a" + npchar.quest.numQuestion;
			if (NullCharacter.questions.(sTemp) == GetStrSmallRegister(dialogEditStrings[1]))
			{
				dialog.text = "Правильный ответ!! Вы действительно знающий кэп.";
				link.l1 = "Хех, еще бы! Говори, что там у тебя за вещица.";
				link.l1.go = "PortSpein_ok";
			}
			else
			{
				dialog.text = "Ответ неверный. Может подумаешь и скажешь правильный ответ?";
				link.l1 = "Да, попробую еще раз.";
				link.l1.go = "PortSpein_5";
				link.l2 = "Нет, я не знаю ответа на этот вопрос.";
				link.l2.go = "PortSpein_6";
			}
		break;
		case "PortSpein_ok":
			dialog.text = "Это неплохой легкий клинок - германская рапира. Но она не у меня, я просто знаю, где ее можно раздобыть. В пиратском поселении Ла Вега нужно зайти за дом слева от ворот - там в земле спрятано это оружие. Удачи!";
			link.l1 = "Ну что же, я понял тебя. Спасибо за помощь.";
			link.l1.go = "exit_over";
			NextDiag.TempNode = "good_all";
 			pchar.GenQuestRandItem.LaVega_town = true;
			pchar.GenQuestRandItem.LaVega_town.randitem4 = "blade19";
			ReOpenQuestHeader("QuestionsMessages");
			AddQuestRecord("QuestionsMessages", "1");
			AddQuestUserData("QuestionsMessages", "sName", GetFullName(npchar));
		break;
		case "PortSpein_5":
			sTemp = "q" + npchar.quest.numQuestion;
			dialog.text = "Очень хорошо! Тогда повторю свой вопрос. " + NullCharacter.questions.(sTemp);
			Link.l1.edit = 1;
			Link.l1 = "";
			Link.l1.go = "PortSpein_check";
		break;
		case "PortSpein_6":
			dialog.text = "Очень жаль, но в таком случае я не могу отдать вам то, что предназначается хорошему кэпу.\nНо я готов выслушать ваш ответ в другое время, если вы сможете меня застать. Я буду в городе еще чуть больше недели.";
			link.l1 = "Хорошо, если узнаю ответ - попытаюсь опять.";
			link.l1.go = "exit";
			NextDiag.TempNode = "PortSpein_again";
		break;
		case "PortSpein_again":
			dialog.text = "А-а, вот и вы снова. Узнали ответ?";
			link.l1 = "Кажется, узнал.";
			link.l1.go = "PortSpein_5";
		break;
		//================================== Виллемстад ======================================
		case "Villemstad":
			NextDiag.TempNode = "Villemstad";
			dialog.text = "Здравствуйте, капитан. У меня к вам есть небольшое дело.";
			link.l1 = "Не желаю разговаривать.";
			link.l1.go = "exit_over";
			link.l2 = "Что вы говорите! И что это за дело такое невероятное?";
			link.l2.go = "Villemstad_1";
			DeleteAttribute(npchar, "talker");
			npchar.LifeDay = 20;
			SaveCurrentNpcQuestDateParam(npchar, "LifeTimeCreate");
		break;
		case "Villemstad_1":
			dialog.text = "Попрошу не иронизировать, я вдова флибустьера!";
			link.l1 = "Простите... Так что за дело у вас ко мне, " +  GetAddress_FormToNPC(NPChar) + "?";
			link.l1.go = "Villemstad_2";
		break;
		case "Villemstad_2":
			dialog.text = "Мы с мужем одно время жили в Пуэрто-Принсипе, но потом он ушел за призом и пропал...";
			link.l1 = "Печально.";
			link.l1.go = "Villemstad_3";
		break;
		case "Villemstad_3":
			dialog.text = "Ах, не говорите.\nНу так вот, кое-какие его вещи остались у меня, и одна из них уж наверняка должна пригодиться такому капитану, как вы.";
			link.l1 = "Понятно, сударыня. Я готов купить у вас это.";
			link.l1.go = "Villemstad_4";
		break;
		case "Villemstad_4":
			dialog.text = "Это не продается, это память о муже! Но я не хочу, чтобы она лежала без дела, я хочу, чтобы эта вещь приносила пользу настоящим морякам.\n"+
				"Поэтому я предлагаю вам ответить на один вопрос, чтобы я имела представление о вас как о специалисте.";
			link.l1 = "Хм, ну что же, задавайте свой вопрос, я готов.";
			link.l1.go = "Villemstad_5";
			link.l2 = "Вот еще, делать нечего. Нет, дорогуша, обойдемся как-нибудь.";
			link.l2.go = "All_disagree";
		break;
		case "Villemstad_5":
			sTemp = "q" + npchar.quest.numQuestion;
			dialog.text = "Очень хорошо. Итак, капитан, вот мой вопрос. " + NullCharacter.questions.(sTemp);
			Link.l1.edit = 1;
			Link.l1 = "";
			Link.l1.go = "Villemstad_check";	
		break;
		case "Villemstad_check":
			sTemp = "a" + npchar.quest.numQuestion;
			if (NullCharacter.questions.(sTemp) == GetStrSmallRegister(dialogEditStrings[1]))
			{
				dialog.text = "Отлично, это правильный ответ!";
				link.l1 = "Другого и быть не могло... Ну, а теперь к делу. Что там за вещица, что мне будет нужна?";
				link.l1.go = "Villemstad_ok";
			}
			else
			{
				dialog.text = "Неправильно, это неверный ответ на мой вопрос. Предлагаю попробовать еще раз.";
				link.l1 = "Да, думаю, нужно.";
				link.l1.go = "Villemstad_6";
				link.l2 = "Нет, не желаю я в гадалки играть.";
				link.l2.go = "Villemstad_7";
			}
		break;
		case "Villemstad_ok":
			dialog.text = "Это неплохая подзорная труба. Она закопана в земле за нашим бывшим домом в Пуэрто-Принсипе, слева от резиденции.";
			link.l1 = "Понятно. Ну что же, спасибо тебе и прощай.";
			link.l1.go = "exit_over";
			NextDiag.TempNode = "good_all";
 			pchar.GenQuestRandItem.PuertoPrincipe_town = true;
			pchar.GenQuestRandItem.PuertoPrincipe_town.randitem1 = "spyglass3";
			ReOpenQuestHeader("QuestionsMessages");
			AddQuestRecord("QuestionsMessages", "2");
			AddQuestUserData("QuestionsMessages", "sName", GetFullName(npchar));
		break;
		case "Villemstad_6":
			sTemp = "q" + npchar.quest.numQuestion;
			dialog.text = "Хорошо. Полагаю, мне следует напомнить вопрос. " + NullCharacter.questions.(sTemp);
			Link.l1.edit = 1;
			Link.l1 = "";
			Link.l1.go = "Villemstad_check";
		break;
		case "Villemstad_7":
			dialog.text = "Не желаешь - как хочешь. Прощай, бравый капитан...";
			link.l1 = "До свидания.";
			link.l1.go = "exit";
			NextDiag.TempNode = "Villemstad_again";
		break;
		case "Villemstad_again":
			dialog.text = "Не поняла! Что вам еще нужно?";
			link.l1 = "Хочу еще раз попробовать ответить на вашу загадку, " +  GetAddress_FormToNPC(NPChar) + ".";
			link.l1.go = "Villemstad_6";
		break;
		//================================== Порт-о-Принс ======================================
		case "PortPax":
			NextDiag.TempNode = "PortPax";
			dialog.text = "Здравствуйте, капитан. У меня есть предложение к вам.";
			link.l1 = "Обойдусь без твоего предложения.";
			link.l1.go = "exit_over";
			link.l2 = "Слушаю вас внимательно, " +  GetAddress_FormToNPC(NPChar) + ".";
			link.l2.go = "PortPax_1";
			DeleteAttribute(npchar, "talker");
			npchar.LifeDay = 20;
			SaveCurrentNpcQuestDateParam(npchar, "LifeTimeCreate");
		break;
		case "PortPax_1":
			dialog.text = "Вы знаете, так случилось, что мне довелось стать обладательницей некой вещи, которая нужна капитанам, в основном. Ну, а девушкам она абсолютно ни к чему.";
			link.l1 = "И что?";
			link.l1.go = "PortPax_2";
		break;
		case "PortPax_2":
			dialog.text = "Я хочу отдать ее вам. Но не просто так, а только если буду уверена, что вы правильно ей распорядитесь, а не продадите первому встречному лоточнику.";
			link.l1 = "Хм, скажите мне, " +  GetAddress_FormToNPC(NPChar) + ", что это за вещь, и я дам вам ответ.";
			link.l1.go = "PortPax_3";
		break;
		case "PortPax_3":
			dialog.text = "Эта вещь нужна каждому грамотному капитану, это уж мне известно.\nЯ задам вам один вопрос, и в случае правильного ответа вы получите этот предмет.";
			link.l1 = "Черт возьми! Ну что за странные игры в вопросы и ответы! Давайте, я вам просто заплачу, и дело концом.";
			link.l1.go = "PortPax_4";
		break;
		case "PortPax_4":
			dialog.text = "Нет, капитан, мой вопрос - ваш ответ. Согласны?";
			link.l1 = "Ну хорошо, послушаю для интереса.";
			link.l1.go = "PortPax_5";
			link.l2 = "Нет, мне это не интересно.";
			link.l2.go = "All_disagree";
		break;
		case "PortPax_5":
			sTemp = "q" + npchar.quest.numQuestion;
			dialog.text = "Очень хорошо. Итак, капитан, вот мой вопрос. " + NullCharacter.questions.(sTemp);
			Link.l1.edit = 1;
			Link.l1 = "";
			Link.l1.go = "PortPax_check";	
		break;
		case "PortPax_check":
			sTemp = "a" + npchar.quest.numQuestion;
			if (NullCharacter.questions.(sTemp) == GetStrSmallRegister(dialogEditStrings[1]))
			{
				dialog.text = "Прекрасно, капитан! Это правильный ответ!";
				link.l1 = "Естественно, я же специалист... Так что там за предмет, о котором шла речь?";
				link.l1.go = "PortPax_ok";
			}
			else
			{
				dialog.text = "Хм, не так, капитан. Вы ошиблись. Предлагаю повторить попытку.";
				link.l1 = "Попытка - не пытка. Давай повторим.";
				link.l1.go = "PortPax_6";
				link.l2 = "Нет, хватит с меня, дорогуша. В эти игры я больше играть не желаю.";
				link.l2.go = "PortPax_7";
			}
		break;
		case "PortPax_ok":
			dialog.text = "Это прекрасный клинок среднего веса 'Ураган'. Спрятан он в подземелье под Тортугой, в одном из тупиков. Придется поискать.";
			link.l1 = "Поищем, чего уж. Хороший клинок - залог успеха в схватке. Спасибо тебе.";
			link.l1.go = "exit_over";
			NextDiag.TempNode = "good_all";
 			pchar.GenQuestRandItem.Tortuga_Cave = true;
			pchar.GenQuestRandItem.Tortuga_Cave.randitem1 = "blade24";
			ReOpenQuestHeader("QuestionsMessages");
			AddQuestRecord("QuestionsMessages", "3");
			AddQuestUserData("QuestionsMessages", "sName", GetFullName(npchar));
		break;
		case "PortPax_6":
			sTemp = "q" + npchar.quest.numQuestion;
			dialog.text = "Да? Ну тогда, полагаю, мне следует напомнить вопрос. " + NullCharacter.questions.(sTemp);
			Link.l1.edit = 1;
			Link.l1 = "";
			Link.l1.go = "PortPax_check";
		break;
		case "PortPax_7":
			dialog.text = "Не желаешь? Ну прощай. И как такой бездарь капитаном работает?..";
			link.l1 = "Эй, эй, полегче!";
			link.l1.go = "exit";
			NextDiag.TempNode = "PortPax_again";
		break;
		case "PortPax_again":
			dialog.text = "Что вам нужно? Кажется, мы все обсудили.";
			link.l1 = "Ты знаешь, кажется я знаю правильный ответ.";
			link.l1.go = "PortPax_6";
		break;
		//================================== Сан-Хуан ======================================
		case "SanJuan":
			NextDiag.TempNode = "SanJuan";
			dialog.text = "Приветсвую вас, капитан. Хочу кое-что вам предложить.";
			link.l1 = "Нет, меня это не интересует.";
			link.l1.go = "exit_over";
			link.l2 = "Внимательно вас слушаю, " + GetAddress_FormToNPC(NPChar) + ".";
			link.l2.go = "SanJuan_1";
			DeleteAttribute(npchar, "talker");
			npchar.LifeDay = 20;
			SaveCurrentNpcQuestDateParam(npchar, "LifeTimeCreate");
		break;
		case "SanJuan_1":
			dialog.text = "Есть у меня одна одна очень нужная капитанам вещь, которая без сомнения заинтересует вас.";
			link.l1 = "Еще более внимательно вас слушаю. Что это за вещь?";
			link.l1.go = "SanJuan_2";
		break;
		case "SanJuan_2":
			dialog.text = "Документ, я бы сказал, достался мне по случаю и на определенных условиях. Я должен как можно быстрей передать его капитану, который в этом действительно нуждается и достоин того.";
			link.l1 = "Вот как? Ну, и что же это за таинственный документ?";
			link.l1.go = "SanJuan_3";
		break;
		case "SanJuan_3":
			dialog.text = "Я могу сказать вам это только после того, как вы ответите мне на один вопрос.";
			link.l1 = "Что еще за вопрос?";
			link.l1.go = "SanJuan_4";
		break;
		case "SanJuan_4":
			dialog.text = "Вопрос на общие темы для кэпов. Я должен четко представлять себе, с кем имею дело.";
			link.l1 = "Ну хорошо, завай свой вопрос на общие темы.";
			link.l1.go = "SanJuan_5";
			link.l2 = "Ни на какие дурацкие вопросы я отвечать не желаю.";
			link.l2.go = "All_disagree";
		break;
		case "SanJuan_5":
			sTemp = "q" + npchar.quest.numQuestion;
			dialog.text = "Очень хорошо. Итак, капитан, вот мой вопрос. " + NullCharacter.questions.(sTemp);
			Link.l1.edit = 1;
			Link.l1 = "";
			Link.l1.go = "SanJuan_check";	
		break;
		case "SanJuan_check":
			sTemp = "a" + npchar.quest.numQuestion;
			if (NullCharacter.questions.(sTemp) == GetStrSmallRegister(dialogEditStrings[1]))
			{
				dialog.text = "Великолепно, капитан! Совершенно правильно!";
				link.l1 = "Ну так!.. Так что там за таинственный документ?";
				link.l1.go = "SanJuan_ok";
			}
			else
			{
				dialog.text = "Нет, капитан, ответ не верный. Предлагаю все-таки дать правильный ответ.";
				link.l1 = "Повторим...";
				link.l1.go = "SanJuan_6";
				link.l2 = "Нет, хватит с меня вопросов.";
				link.l2.go = "SanJuan_7";
			}
		break;
		case "SanJuan_ok":
			int iNation = 5;
			for (int i=0; i<4 ; i++)
			{
				if (GetRelation2BaseNation(i) == RELATION_ENEMY)
				{
					iNation = i;
					break;
				}
			}
			if (iNation == 5) iNation = rand(3);
			GiveNationLicence(iNation, 40);
			dialog.text = "Это " + GetRusNameNationLicence(iNation) + " сроком на сорок дней! С удовольствием вручаю ее вам.";
			link.l1 = "Ого, довольно дорогой документ! Ну что же, спасибо вам, " + GetAddress_FormToNPC(NPChar) + ", за оказанную услугу. Прощайте.";
			link.l1.go = "exit_over";
			NextDiag.TempNode = "good_all";
		break;
		case "SanJuan_6":
			sTemp = "q" + npchar.quest.numQuestion;
			dialog.text = "Прекрасно. Считаю свои долгом напомнить вам вопрос. " + NullCharacter.questions.(sTemp);
			Link.l1.edit = 1;
			Link.l1 = "";
			Link.l1.go = "SanJuan_check";
		break;
		case "SanJuan_7":
			dialog.text = "Ну, хватит - так хватит. Прощайте, капитан...";
			link.l1 = "Желаю всего наилучшего.";
			link.l1.go = "exit";
			NextDiag.TempNode = "SanJuan_again";
		break;
		case "SanJuan_again":
			dialog.text = "Слушаю вас внимательно, капитан. Неужели решили повторить попытку и ответить на заданный вопрос?";
			link.l1 = "Да, кажется, мне ведом правильный ответ.";
			link.l1.go = "SanJuan_6";
		break;
		//================================== Кумана ======================================
		case "Cumana":
			NextDiag.TempNode = "Cumana";
			dialog.text = "Приветствую вас, капитан! У меня к вам есть небольшое дельце. Вы не уделите мне минуту внимания?";
			link.l1 = "Нет, не уделю.";
			link.l1.go = "exit_over";			
			link.l2 = "Да без проблем, минута твоя.";
			link.l2.go = "Cumana_1";
			DeleteAttribute(npchar, "talker");
			npchar.LifeDay = 20;
			SaveCurrentNpcQuestDateParam(npchar, "LifeTimeCreate");
		break;
		case "Cumana_1":
			dialog.text = "Очень хорошо. Хотел предложить вам маленькую сделку. У меня есть некий предмет, который может вас заинтересовать.";
			link.l1 = "Назови цену и что это за предмет.";
			link.l1.go = "Cumana_2";
		break;
		case "Cumana_2":
			dialog.text = "Денег за него я не возьму, вам нужно будет ответить мне на один вопрос. Если ответ будет верен - предмет ваш.";
			link.l1 = "Очень странно. А деньги тебе не нужны?";
			link.l1.go = "Cumana_3";
		break;
		case "Cumana_3":
			dialog.text = "Хех, деньги всем нужны, однако в этой сделке все решают ваши знания. Я не хочу дальше объяснять причины моего поведения, просто скажите - да или нет.";
			link.l1 = "Хм, пожалуй, что да. Самому интересно стало.";
			link.l1.go = "Cumana_4";
			link.l2 = "Нет, нет, и еще раз нет. Прощай.";
			link.l2.go = "All_disagree";
		break;
		case "Cumana_4":
			sTemp = "q" + npchar.quest.numQuestion;
			dialog.text = "Хех, на один вопрос вы уже ответили правильно. А теперь главное событие этого дня! Слушайте вопрос. " + NullCharacter.questions.(sTemp);
			Link.l1.edit = 1;
			Link.l1 = "";
			Link.l1.go = "Cumana_check";	
		break;
		case "Cumana_check":
			sTemp = "a" + npchar.quest.numQuestion;
			if (NullCharacter.questions.(sTemp) == GetStrSmallRegister(dialogEditStrings[1]))
			{
				dialog.text = "Великолепно! Это правильный ответ!";
				link.l1 = "Понятно дело... Так что там за предмет, счастливым обладателем которого я теперь являюсь?";
				link.l1.go = "Cumana_ok";
			}
			else
			{
				dialog.text = "Очень жаль, капитан. Мне действительно очень жаль... Ответ неверный! Попробуете еще раз?";
				link.l1 = "Попробую!";
				link.l1.go = "Cumana_6";
				link.l2 = "Нет, пожалуй...";
				link.l2.go = "Cumana_7";
			}
		break;
		case "Cumana_ok":
			dialog.text = "Это статуя, дающая необычайную возможность лучше проводить ремонт корабля. Спрятана она в бухте близ Порто Белло. Называется эта бухта - лагуна Чирики. Внимательно все там осмотри, и не бойся ноги замочить. Удачи, капитан!";
			link.l1 = "Спасибо. Знаешь, это действительно важная вещица для кэпа. Спасибо тебе.";
			link.l1.go = "exit_over";
			NextDiag.TempNode = "good_all";
 			pchar.GenQuestRandItem.Shore46 = true;
			pchar.GenQuestRandItem.Shore46.randitem1 = "STATUE1";
			ReOpenQuestHeader("QuestionsMessages");
			AddQuestRecord("QuestionsMessages", "4");
			AddQuestUserData("QuestionsMessages", "sName", GetFullName(npchar));
		break;
		case "Cumana_6":
			sTemp = "q" + npchar.quest.numQuestion;
			dialog.text = "Ну, тогда напоминаю вопрос! " + NullCharacter.questions.(sTemp);
			Link.l1.edit = 1;
			Link.l1 = "";
			Link.l1.go = "Cumana_check";
		break;
		case "Cumana_7":
			dialog.text = "Ну, не хочешь - как хочешь. Прощайте, капитан Незнайка...";
			link.l1 = "Чего?..";
			link.l1.go = "exit";
			NextDiag.TempNode = "Cumana_again";
		break;
		case "Cumana_again":
			dialog.text = "И что вам нужно от меня?";
			link.l1 = "Я хочу еще раз попробовать ответить на твою загадку.";
			link.l1.go = "Cumana_6";
		break;
		//================================== Порт Рояль ======================================
		case "PortRoyal":
			NextDiag.TempNode = "PortRoyal";
			dialog.text = "Добрый день, капитан. Послушайте, мне кажется, что вы именно тот, кто мне нужен...";
			link.l1 = "А вот я так не думаю.";
			link.l1.go = "exit_over";			
			link.l2 = "В чем дело, " + GetAddress_FormToNPC(NPChar) + "?";
			link.l2.go = "PortRoyal_1";
			DeleteAttribute(npchar, "talker");
			npchar.LifeDay = 20;
			SaveCurrentNpcQuestDateParam(npchar, "LifeTimeCreate");
		break;
		case "PortRoyal_1":
			dialog.text = "Я хочу предложить вам обмен. Дело в том, что у меня имеется вещица, которая мне совершенно ни к чему, ну а вам пригодиться уж точно.";
			link.l1 = "И что ты хочешь взамен?";
			link.l1.go = "PortRoyal_2";
		break;
		case "PortRoyal_2":
			dialog.text = "Ничего особенного. Я хочу знать, что эта вещица в надежных руках.";
			link.l1 = "Ну так знай - она будет в действительно надежных руках. Можешь не сомневаться.";
			link.l1.go = "PortRoyal_3";
		break;
		case "PortRoyal_3":
			dialog.text = "Охотно верю, но, все же, проверю. Ответите на один мой вопрос - и вещь ваша.";
			link.l1 = "Задавай свой вопрос, нет ничего проще!";
			link.l1.go = "PortRoyal_4";
			link.l2 = "Обойдусь как-нибудь...";
			link.l2.go = "All_disagree";
		break;
		case "PortRoyal_4":
			sTemp = "q" + npchar.quest.numQuestion;
			dialog.text = "Ну, вопрос достаточно прост. " + NullCharacter.questions.(sTemp);
			Link.l1.edit = 1;
			Link.l1 = "";
			Link.l1.go = "PortRoyal_check";	
		break;
		case "PortRoyal_check":
			sTemp = "a" + npchar.quest.numQuestion;
			if (NullCharacter.questions.(sTemp) == GetStrSmallRegister(dialogEditStrings[1]))
			{
				dialog.text = "Я даже и не сомневалась, что вы правильно ответите.";
				link.l1 = "Естественно, другого и быть не могло. Так, ну а что там за вещица, которая уже моя?";
				link.l1.go = "PortRoyal_ok";
			}
			else
			{
				dialog.text = "Ответ неверный, капитан. Мне нужен правильный ответ. Еще попытаетесь?";
				link.l1 = "Да, попытаюсь еще!";
				link.l1.go = "PortRoyal_6";
				link.l2 = "Нет, не желаю я в отгадки играть.";
				link.l2.go = "PortRoyal_7";
			}
		break;
		case "PortRoyal_ok":
			dialog.text = "Это тяжелый меч - Атилл. Найти его ты сможешь на Ямайке в бухте Портленд.";
			link.l1 = "Да, в бою это штука нужная, а девушке уж точно ни к чему. Спасибо тебе.";
			link.l1.go = "exit_over";
			NextDiag.TempNode = "good_all";
 			pchar.GenQuestRandItem.Shore36 = true;
			pchar.GenQuestRandItem.Shore36.randitem2 = "blade20";
			ReOpenQuestHeader("QuestionsMessages");
			AddQuestRecord("QuestionsMessages", "5");
			AddQuestUserData("QuestionsMessages", "sName", GetFullName(npchar));
		break;
		case "PortRoyal_6":
			sTemp = "q" + npchar.quest.numQuestion;
			dialog.text = "Хорошо. Полагаю, мне следует напомнить вопрос. " + NullCharacter.questions.(sTemp);
			Link.l1.edit = 1;
			Link.l1 = "";
			Link.l1.go = "PortRoyal_check";
		break;
		case "PortRoyal_7":
			dialog.text = "Не желаешь - как хочешь. Хм, а на вид такой солидный капитан...";
			link.l1 = "Что ты хочешь этим сказать?";
			link.l1.go = "exit";
			NextDiag.TempNode = "PortRoyal_again";
		break;
		case "PortRoyal_again":
			dialog.text = "Не поняла! Что вам еще нужно?";
			link.l1 = "Хочу еще раз попробовать ответить на твою загадку.";
			link.l1.go = "PortRoyal_6";
		break;
		//================================== Сантьяго ======================================
		case "Santiago":
			NextDiag.TempNode = "Santiago";
			dialog.text = "Приветствую вас в Сантьяго, капитан! Я хотел с вами поговорить по одному маленькому дельцу. Вы не против?";
			link.l1 = "Не желаю разговаривать.";
			link.l1.go = "exit_over";			
			link.l2 = "Ни в коей мере. Я к вашим услугам.";
			link.l2.go = "Santiago_1";
			DeleteAttribute(npchar, "talker");
			npchar.LifeDay = 20;
			SaveCurrentNpcQuestDateParam(npchar, "LifeTimeCreate");
		break;
		case "Santiago_1":
			dialog.text = "Прекрасно! Тогда приступим. Вышеуказанное дельце заключает в себе по сути сделку, или обмен, если хотите...";
			link.l1 = "Хочу, обмен хочу! Давате меняться.";
			link.l1.go = "Santiago_2";
		break;
		case "Santiago_2":
			dialog.text = "К-хе, ну что же, давайте. Итак, я могу предложить вам один, без сомнения, нужный вам предмет. Взамен мне от вас ничего не нужно, я лишь хочу получить ответ на один поставленный мной вопрос.";
			link.l1 = "И что это за вопрос?";
			link.l1.go = "Santiago_3";
		break;
		case "Santiago_3":
			dialog.text = "Общего характера, по вашей специфике. Понимаете, я лишь хочу проверить уровень ваших знаний. Это необходимое условие. Вы согласны?";
			link.l1 = "Хм, хоть я и не понимаю, зачем вам это, но, тем не менее, я согласен.";
			link.l1.go = "Santiago_4";
			link.l2 = "Хех, проверяльщик выискался! Катись отсюда, пока не поздно...";
			link.l2.go = "All_disagree";
		break;
		case "Santiago_4":
			sTemp = "q" + npchar.quest.numQuestion;
			dialog.text = "Замечательно! Итак, слушайте, пожалуйста, мой вопрос. " + NullCharacter.questions.(sTemp);
			Link.l1.edit = 1;
			Link.l1 = "";
			Link.l1.go = "Santiago_check";	
		break;
		case "Santiago_check":
			sTemp = "a" + npchar.quest.numQuestion;
			if (NullCharacter.questions.(sTemp) == GetStrSmallRegister(dialogEditStrings[1]))
			{
				dialog.text = "Отлично, это правильный ответ! Поздравляю.";
				link.l1 = "Спасибо за поздравления, мне очень приятно. И чтобы мне вдвойне было приятно, давайте я немного послушаю о предмете нашей сделки...";
				link.l1.go = "Santiago_ok";
			}
			else
			{
				dialog.text = "Хм, капитан, этот ответ меня устроить не может. Он неправильный! Прошу вас, попробойте еще раз...";
				link.l1 = "Конечно, попробую.";
				link.l1.go = "Santiago_6";
				link.l2 = "Нет, откажусь. Надоело.";
				link.l2.go = "Santiago_7";
			}
		break;
		case "Santiago_ok":
			dialog.text = "Это двуствольный пистолет! Вы можете отыскать его в гроте на Кубе, чтобы туда попасть, вам нужно выйти за город и прогуляться по джунглям. Но согласитесь, что такое оружие - вещь нужная вам в крайней степени.";
			link.l1 = "Да уж, с этим не поспоришь. Ну что же, придется прогулятся. Спасибо вам и всего хорошего.";
			link.l1.go = "exit_over";
			NextDiag.TempNode = "good_all";
 			pchar.GenQuestRandItem.Cuba_Grot = true;
			pchar.GenQuestRandItem.Cuba_Grot.randitem1 = "pistol6";
			ReOpenQuestHeader("QuestionsMessages");
			AddQuestRecord("QuestionsMessages", "6");
			AddQuestUserData("QuestionsMessages", "sName", GetFullName(npchar));
		break;
		case "Santiago_6":
			sTemp = "q" + npchar.quest.numQuestion;
			dialog.text = "Прекрасно! В таком случае, напомню его вам. " + NullCharacter.questions.(sTemp);
			Link.l1.edit = 1;
			Link.l1 = "";
			Link.l1.go = "Santiago_check";
		break;
		case "Santiago_7":
			dialog.text = "Вы отказываетесь? Вам надоело?! Хм, но смысл вопроса - ваша профессия! Впрочем, мне до этого дела нет, прощайте...";
			link.l1 = "Всего хорошего.";
			link.l1.go = "exit";
			NextDiag.TempNode = "Santiago_again";
		break;
		case "Santiago_again":
			dialog.text = "Чем я могу вам помочь?";
			link.l1 = "Хочу ответить на ваш вопрос.";
			link.l1.go = "Santiago_6";
		break;
	}
}
