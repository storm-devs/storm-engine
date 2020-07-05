// диалог по городам
void ProcessCommonDialogEvent(ref NPChar, aref Link, aref NextDiag)
{
    switch (Dialog.CurrentNode)
	{
		case "quests":
			dialog.text = NPCStringReactionRepeat(RandPhraseSimple("Какие вопросы?", "Что вам угодно, " + GetAddress_Form(NPChar) + "?"), "Совсем недавно вы пытались задать мне вопрос " + GetAddress_Form(NPChar) + "...", "В течение этого дня вы уже третий раз говорите о каком-то вопросе...",
                          "Послушайте, если вы не по делам порта, то не стоит меня вопросами донимать.", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat(RandPhraseSimple("Я передумал...", "Сейчас мне не о чем говорить"), "Хм, что-то с памятью моей стало...",
                      "Да уж, действительно в третий раз...", "Извините, но портовые дела меня сейчас не интересуют.", npchar, Dialog.CurrentNode);
			link.l1.go = "exit";
            // ==> квест №7 голл.линейки, догнать похитителей депеши
            if (pchar.questTemp.State == "DelivLettTortuga_toPortMan")
            {
    			link.l1 = "Д'Ожерон отправил меня к тебе, чтобы ты немедленно указал мне, что за судно покинуло воды Тортуги в течение ближайших двух часов!";
    			link.l1.go = "Step_H7_1";
            }
            if (pchar.questTemp.State == "Sp6TakeMess_toTotuga")
            {
                link.l1 = "Я уполномочен засвидетельствовать вам свое почтение и ознакомить с моими документами. Извольте прочесть мою торговую доверенность Французской Ост-Индской компании.";
                link.l1.go = "Step_S6_1";
            }
            if (pchar.questTemp.State == "Sp6TakeMess_waitMessanger")
            {
                link.l1 = "Послушайте, не было еще ожидаемого мной голландского корабля?";
                link.l1.go = "Step_S6_6";
            }
            if (pchar.questTemp.State == "Fr4SoleiRoyal_toPortMan")
            {
                link.l1 = "Согласно предписанию д'Ожерона вам нужно передать в мое распоряжение линейный корабль первого класса 'Солей Руаяль'.";
                link.l1.go = "Step_F4_1";
            }
		break;

//************************************* Голл. линейка, квест №7, доставка депеши *******************************
		case "Step_H7_1":
            dialog.text = "Только одно - бриг 'Ла Рошель'. Он ушел на Пуэрто-Рико по направлению к Сан-Хуану. С ним ушел и торговый представитель Французской Ост-Индской компании.";
			link.l1 = "Какой представитель?!";
			link.l1.go = "Step_H7_2";
		break;
		case "Step_H7_2":
            dialog.text = "Я же говорю: представитель Французской Ост-Индской компании. Он прибыл сюда несколько дней назад и сразу предъявил доверенность компании. Дела свои он сделал и хотел отправиться отсюда сразу к вам, на Кюрасао, для чего искал попутное судно. Я и направил его к вам.\n"+
                          "Но в последний момент планы у него, видимо, изменились, и он ушел с бригом, на котором и пришел на Тортугу, собственно.";
			link.l1 = "Черт возьми, похоже, что это и есть Антонио...";
			link.l1.go = "Step_H7_3";
		break;
		case "Step_H7_3":
            dialog.text = "Совершенно правильно, Анотио де... де... Не вспомню уже.";
			link.l1 = "Я должен немедленно догнать этот проклятый бриг. Это вопрос жизни и смерти.";
			link.l1.go = "Step_H7_4";
		break;
		case "Step_H7_4":
            dialog.text = "Неужели? Что же, это возможно. В таком случае не теряйте времени.";
			link.l1 = "Спасибо тебе за инфомацию, хотя чувства благодарности за встречу с Антонио у меня к тебе нет...";
			link.l1.go = "exit";
            AddQuestRecord("Hol_Line_7_DelivLettTortuga", "5");
			pchar.questTemp.State = "DelivLettTortuga_NotFoundLetter";
            Pchar.quest.DelivLettTortuga_AttackBrig.win_condition.l1 = "location";
            Pchar.quest.DelivLettTortuga_AttackBrig.win_condition.l1.location = "PuertoRico";
            Pchar.quest.DelivLettTortuga_AttackBrig.win_condition = "DelivLettTortuga_AttackBrig";
            SetTimerCondition("DelivLettTortuga_BrigTimeOver", 0, 0, 5, false);
		break;
//************************************* Испанская линейка, квест №6, перехват вестового *******************************
		case "Step_S6_1":
            dialog.text = NPCStringReactionRepeat("Хм, вижу. Но какое это отношение имеет ко мне? Я не занимаюсь торговыми операциями...",
                          "Да, да, я знаю уже...", "Что? Опять вы?", "Ну сколько можно? Показывали вы мне свою бумагу...", "block", 0, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat("Я это знаю. У меня к вам просьба другого свойства... Дело в том, что я практически закончил здесь дела, по которым прибыл. Далее я должен прибыть в Виллемстад, что на острове Кюрасао. Так вот, я бы хотел сделать это на попутном судне. Может вы подскажете мне, нет ли у вас в порту достаточно быстроходных судов, идущих на Кюрасао?",
                     "А-а-а, ну ладно...", "Да, я...", "Да? Ну хорошо...", npchar, Dialog.CurrentNode);
			link.l1.go = DialogGoNodeRepeat("Step_S6_2", "none", "none", "none", npchar, Dialog.CurrentNode);
		break;
 		case "Step_S6_2":
            dialog.text = "Хм, пока нет. Но, полагаю, в скором времени появятся - голландцы из Виллемстада здесь довольно часто бывают.";
			link.l1 = "Торговцы?";
			link.l1.go = "Step_S6_3";
		break;
 		case "Step_S6_3":
            dialog.text = "Бывают и торговцы, но я бы вам порекомендовал подождать корабль с вестовым из Кюрасао. Они никогда не задерживаются здесь надолго, час-два в резиденции у нашего генерал-губернатора - и сразу обратно в Виллемстад. И наш генерал-губернатор за это время никого не принимает...";
			link.l1 = "О, это прекрасно, идеальный вариант для меня. Не будете ли вы так любезны сообщить мне, когда такой корабль войдет в порт Тортуги?";
			link.l1.go = "Step_S6_4";
		break;
 		case "Step_S6_4":
            dialog.text = "Хорошо. Где вы остановились, месье?";
			link.l1 = "Я снял комнату в таверне.";
			link.l1.go = "Step_S6_5";
		break;
 		case "Step_S6_5":
            dialog.text = "Прекрасно. Как только голландский корабль бросит якорь у нас в порту, я немедленно отошлю к вам в таверну посыльного с этой информацией.";
			link.l1 = "Очень хорошо. Огромное вам спасибо, месье. Вы оказали услугу нашей компании, мы этого не забудем.";
			link.l1.go = "exit";
            AddQuestRecord("Spa_Line_6_TakeMessangeer", "2");
            pchar.questTemp.State = "Sp6TakeMess_waitMessanger";
            if (CheckAttribute(pchar, "questTemp.State.Sp6Ok"))
            {
                SetTimerCondition("Sp6TakeMess_waitingBoy", 0, 0, rand(7)+1, false);
                DeleteAttribute(pchar, "questTemp.State.Sp6Ok");
            }
            else
            {
                pchar.questTemp.State.Sp6Ok = 1;
            }
		break;
 		case "Step_S6_6":
            dialog.text = "Нет, еще не было. Я же сказал вам, что отправлю посыльного. Не беспокойтесь, я помню о вас.";
			link.l1 = "Спасибо, месье. И простите за назойливость...";
			link.l1.go = "exit";
		break;
//************************************* Французская линейка, квест №4, Сулей Руаяль *******************************
 		case "Step_F4_1":
    		if (GetCompanionQuantity(pchar) == 1)
            {
                dialog.text = "Да, я уже уведомлен, месье. Принимайте командование. По чести сказать, командовать таким кораблем - счастье даже для адмирала...";
    			link.l1 = "Полностью согласен с этим утверждением, сударь... А теперь позвольте откланяться - дела.";
    			link.l1.go = "Step_F4_2";
            }
            else
            {
                dialog.text = "Вам необходимо избавится от дополнительных судов в вашей эскадре. Потом вы можете принимать любые меры, которые считаете нужными, однако на момент передачи корабля вы должны быть свободными от сторонних обязательств.";
    			link.l1 = "Хорошо, я все сделаю.";
    			link.l1.go = "exit";
            }
		break;	
 		case "Step_F4_2":
    		pchar.questTemp.State = "Fr4SoleiRoyal_toBasTer";
			SaveCurrentQuestDateParam("questTemp");
			AddQuestRecord("Fra_Line_4_SoleiRoyal", "2");
			ref sld = GetCharacter(NPC_GenerateCharacter("SoleiRoyalCaptain", "off_fra_2", "man", "man", 35, FRANCE, -1, true));
			sld.Ship.Type = CreateBaseShip(SHIP_SOLEYRU);
			sld.Ship.Name = "Сулей Руаяль";
			SetBaseShipData(sld);
			SetCrewQuantityFull(sld);
			Fantom_SetBalls(sld, "pirate");	
			FantomMakeCoolFighter(sld, 35, 80, 70, BLADE_LONG, "pistol3", 100);	
			sld.CompanionEnemyEnable = false;
			sld.Abordage.Enable = false; // НЕЛЬЗЯ!
			SetCompanionIndex(pchar, -1, sti(sld.index));
			SetShipRemovable(sld, false);
            Pchar.quest.Fr4SoleiRoyal_Fight.win_condition.l1 = "location";
            Pchar.quest.Fr4SoleiRoyal_Fight.win_condition.l1.location = "Dominica";
            Pchar.quest.Fr4SoleiRoyal_Fight.win_condition = "Fr4SoleiRoyal_Fight";
			DialogExit();
		break;	

	}
	UnloadSegment(NPChar.FileDialog2);  // если где-то выход внутри switch  по return не забыть сделать анлод
}

