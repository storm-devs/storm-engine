// диалог по городам
void ProcessCommonDialogEvent(ref NPChar, aref Link, aref NextDiag)
{
 	switch(Dialog.CurrentNode)
	{
        case "quests":
            dialog.text = "Говорите, я слушаю.";
			link.l1 = "Я ошибся. Прощайте.";
			link.l1.go = "Exit";
			NextDiag.TempNode = "First time";
			// ==> Проверяем поле состояния квестов.
            if (pchar.questTemp.State == "WormEnglPlans_SaveOfficer")   // разговор по квест №3 голландской линейке.
            {
    			link.l2 = "Я хочу видеть лейтенанта Алана Маклейна.";
    			link.l2.go = "Step_H3_1";
            }
             // <== Проверяем поле состояния квестов.
		break;

        case "Step_H3_1":
            dialog.text = "Это невозможно.";
			link.l1 = "Все возможно, только для этого придется всех вас убить. К бою!";
			link.l1.go = "fight";
		break;
	}
	UnloadSegment(NPChar.FileDialog2);  // если где-то выход внутри switch  по return не забыть сделать анлод
}

