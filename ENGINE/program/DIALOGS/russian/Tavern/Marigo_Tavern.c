// диалог по городам
void ProcessCommonDialogEvent(ref NPChar, aref Link, aref NextDiag)
{
    switch (Dialog.CurrentNode)
	{
		case "quests":
			dialog.text = NPCStringReactionRepeat("Все сплетни города "+ GetCityName(npchar.city) +" к вашим услугам. Что бы вы хотели узнать?",
                          "Мы только что поднимали это тему. Вы, вероятно, запамятовали...", "Сегодня вы уже третий раз говорите о каком-то вопросе...",
                          "Что ты заладил как попугай одно и то же...", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat("Знаешь, " + NPChar.name + ", как-нибудь в следующий раз.", "Точно, забыл что-то...",
                      "Да уж, действительно в третий раз...", "Да уж...", npchar, Dialog.CurrentNode);
			link.l1.go = "exit";
			// ==> Проверяем поле состояния квестов.
			switch(pchar.questTemp.State)
            {
                case "SeekChumakeiro_ToAaronHouse": //Голландская линейка, квест №1. поиски Чумакейро.
                    link.l1 = HeroStringReactionRepeat("Послушай, у вас в Мариго гостит некий Аарон Мендес Чумакейро. Знаешь такого?",
                    "Я про Аарона хотел спросить... уже выяснил, извини.", "Аарон, Аа-а-аро-о-он...", "С Аароном разобрались, извини...", npchar, Dialog.CurrentNode);
                    link.l1.go = DialogGoNodeRepeat("Step_H1_1", "", "", "", npchar, Dialog.CurrentNode);
                break;

            }
            // <== Проверяем поле состояния квестов.
 		break;
        //============ Голландская линейка, квест №1. поиски Чумакейро.
 		case "Step_H1_1":
    		dialog.text = "Знаю. Я ему подыскал подходящий дом для съема. Хороший человек, заплатил прилично...";
    		link.l1 = "Понятно... А где этот дом, который снял Аарон?";
    		link.l1.go = "Step_H1_2";
 		break;
 		case "Step_H1_2":
    		dialog.text = "Справа от резиденции нашего губернатора. Немного пройти надо и там небольшой дом увидишь, слегка обветшавший снаружи. Но внутри там все в порядке, " + npchar.name + " худого людям не делает.";
    		link.l1 = "Спасибо тебе, пойду навещу этот Аарона.";
    		link.l1.go = "Step_H1_3";
 		break;
 		case "Step_H1_3":
    		dialog.text = "Занимай очередь.";
    		link.l1 = "Как понять?";
    		link.l1.go = "Step_H1_4";
 		break;
 		case "Step_H1_4":
    		dialog.text = "Так и понять - занимай очередь навестить Аарона. О нем уже спрашивали и, видимо, отправились к нему на встречу.";
    		link.l1 = "Кто?";
    		link.l1.go = "Step_H1_5";
 		break;
 		case "Step_H1_5":
    		dialog.text = "Какой-то незнакомец, первый раз его видел у нас в городе. У Аарона сегодня день открытых дверей, хе-хе.";
    		link.l1 = "М-да, похоже на то... Ну да ладно, спасибо тебе еще раз. Прощай.";
    		link.l1.go = "exit";
 		break;


	}
	UnloadSegment(NPChar.FileDialog2);  // если где-то выход внутри switch  по return не забыть сделать анлод
}

