// диалог по городам
void ProcessCommonDialogEvent(ref NPChar, aref Link, aref NextDiag)
{
    switch (Dialog.CurrentNode)
	{
		case "quests":
			dialog.text = NPCStringReactionRepeat(RandPhraseSimple("Вопросы, " + GetAddress_Form(NPChar) + "?", "Что вам угодно, " + GetAddress_Form(NPChar) + "?"), "Хм, уж не пристаете ли вы к мне, " + GetAddress_Form(NPChar) + "? Опять со странным вопросами...", "В течение этого дня вы уже третий раз говорите о каком-то вопросе... Это знаки внимания?",
                          "Опять вопросы будете задавать, " + GetAddress_Form(NPChar) + "?", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat(RandPhraseSimple("Я передумал...", "Сейчас мне не о чем говорить"), "Да нет, что ты, красавица...",
                      "Никак нет, дорогуша, склероз...", "Да нет, какие вопросы...", npchar, Dialog.CurrentNode);
			link.l1.go = "exit";
			//==> проверяем поле состояния квестов
            if (CheckAttribute(pchar, "questTemp.State.Usurer"))
            {
                dialog.text = "Вы что-то хотите мне сказать?";
    			link.l1 = "Да. Я хочу сказать, что ты замечательно выглядишь, красавица. Даже несмотря на слезки в глазах.";
    			link.l1.go = "Step_S2_1";
            }
            //<== проверяем поле состояния квестов
		break;
//*************************** Квест №2 испанки, задание Инквизиции ***************************
 		case "Step_S2_1":
    		dialog.text = NPCStringReactionRepeat("Ох...", "Ох, поговорили мы уже об этом...", "Послушайте, мы поговорили - и все...", "Что это вы пристаете к бедной девушке. Думаете, что горе у меня - и вы тут как тут, утешить?", "block", 0, npchar, Dialog.CurrentNode);
    		link.l1 = HeroStringReactionRepeat("Так кто же он, твой обидчик? Может, я смогу помочь?", "Да, верно.", "Хорошо, хорошо...", "Да ничего я не думаю, красавица. Все нормально...", npchar, Dialog.CurrentNode);
    		link.l1.go = DialogGoNodeRepeat("Step_S2_2", "none", "none", "none", npchar, Dialog.CurrentNode);
 		break;
 		case "Step_S2_2":
    		dialog.text = "Это сын ростовщика - Янсен! Он... Он...";
    		link.l1 = "(Какая удача!) Я как раз ищу этого человека. Что же он тебе сделал? И где я могу его найти?";
    		link.l1.go = "Step_S2_3";
 		break;
 		case "Step_S2_3":
    		dialog.text = "Он обманул меня. Наобещал золотые горы, жениться обещал, а сам улизнул! Сбежал вчера с пиратами на люггере в Форт Оранж!";
    		link.l1 = "Вот негодяй! Встречу - кишки выпущу.";
    		link.l1.go = "Step_S2_4";
 		break;
 		case "Step_S2_4":
    		dialog.text = "А вдруг он от этого умрет? Лучше передайте ему, что бы он не появлялся в таверне. Иначе я ему глаза выцарапаю!";
    		link.l1 = "Ну хорошо, передам.";
    		link.l1.go = "exit";
            AddQuestRecord("Spa_Line_2_Inquisition", "6");
            Pchar.quest.Inquisition_AttackLugger.win_condition.l1 = "location";
            Pchar.quest.Inquisition_AttackLugger.win_condition.l1.location = "Jamaica";
            Pchar.quest.Inquisition_AttackLugger.win_condition = "Inquisition_AttackLugger";
            SetTimerCondition("Inquisition_LuggerTimeOver", 0, 0, 20, false);
 		break;



	}
	UnloadSegment(NPChar.FileDialog2);  // если где-то выход внутри switch  по return не забыть сделать анлод
}

