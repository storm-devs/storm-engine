// диалог по городам
void ProcessCommonDialogEvent(ref NPChar, aref Link, aref NextDiag)
{
    switch (Dialog.CurrentNode)
	{
		case "quests":
			dialog.text = NPCStringReactionRepeat(RandPhraseSimple("Что вы хотели? Спрашивайте.", "Я слушаю вас, что за вопрос?"), "Второй раз за день вы пытаетесь задать ворпос...", "В третий раз за день вы опять пытаетесь задать вопрос...",
                          "Да когда же это кончится?! У меня дел полно по управлению делами города, а ты все вопросы пытаешься задать!", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat(RandPhraseSimple("Я передумал...", "Не сейчас, не место и не время..."), "Да, верно... Но не сейчас, позже...",
                      "Задам, задам... Только позже...", "Извините, " + GetAddress_FormToNPC(NPChar) + "...", npchar, Dialog.CurrentNode);
			link.l1.go = "exit";
			// ==> Проверяем поле состояния квестов.
			switch(pchar.questTemp.State)
            {
                case "TakeFoodCuracao_toOrangeMayor": //Голландская линейка, квест №2, доставка продовольствия из форта Оранж.
                    link.l2 = "Я прибыл сюда по распоряжению Питера Стэвезанта. У вас для Кюрасао приготовлена партия товара, я должен купить и доставить ее. Прошу вас ознакомиться с документами на предоставление специальных цен.";
                    link.l2.go = "Step_H2_1";
                break;
                
            }
             // <== Проверяем поле состояния квестов.
		break;
		
        case "Step_H2_1":
			dialog.text = "Так, давайте прочтем... Ну что же, с бумагами все в порядке. Всю партию товара вы можете получить в местном магазине, я немедленно распоряжусь выдать ее вам.\n"+
                          "Партия состоит из кофе, черного, красного и сандалового дерева. Общий вес партии товара, приготовленного к отправке - 6800 центнеров. Рассчитайтесь в магазине - и можете приступать к погрузке.";
			link.l1 = "Очень хорошо.";
			link.l1.go = "exit";
            TakeItemFromCharacter(pchar, "Trust");
            BackItemDescribe("Trust");
            AddQuestRecord("Hol_Line_2_TakeFoodCuracao", "2");
			pchar.questTemp.State = "TakeFoodCuracao_toOrangeStore";
        	pchar.GenQuest.StoreGoods.StoreIdx = FortOrange_STORE;
        	pchar.GenQuest.StoreGoods.HL2_DiscountPrices = true;
		break;

        case "DefenceOrange": //Голландская линейка, квест №4, защита форта Оранж.
            dialog.text = "Как же я рад вас видеть, как же рад!...";
            link.l2 = "Я здесь по поручению Стэвезанта. Кажется, мы успели...";
            link.l2.go = "Step_H4_1";
        break;
        
        case "Step_H4_1":
			dialog.text = "Черт возьми, вы не просто успели, вы просто спасли нас! Форт Оранж благодарит вас от всей души! Мое почтение Питеру Стэвезанту.";
			link.l1 = "Обязательно передам. Всего хорошего.";
			link.l1.go = "exit";
			NextDiag.TempNode = "First time";
		break;

	}
	UnloadSegment(NPChar.FileDialog2);  // если где-то выход внутри switch  по return не забыть сделать анлод
}

