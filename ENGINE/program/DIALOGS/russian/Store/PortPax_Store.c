// диалог по городам
void ProcessCommonDialogEvent(ref NPChar, aref Link, aref NextDiag)
{

	switch (Dialog.CurrentNode)
	{
		case "quests":
			dialog.text = NPCStringReactionRepeat("Спрашивай, чего ты хочешь?",
                          "Мы только что поднимали это тему. Вы, вероятно, запамятовали...", "Сегодня вы уже третий раз говорите о каком-то вопросе...",
                          "Послушай, это магазин, здесь люди покупают что-то. Не отвлекай меня!", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat("Знаешь, " + NPChar.name + ", как-нибудь в следующий раз.", "Точно, забыл что-то...",
                      "Да уж, действительно в третий раз...", "Гм, не буду...", npchar, Dialog.CurrentNode);
			link.l1.go = "exit";
            if (CheckAttribute(pchar, "questTemp.Azzy.GiveRum"))
            {
        		Link.l1 = "Послушай приятель, меня прислал к тебе некто Аззи.";
        	    Link.l1.go = "Step_Az6";
                if (pchar.questTemp.Azzy.GiveRum == "OK")
                {
            		Link.l1 = "Я хочу забрать положенный мне ром.";
            	    Link.l1.go = "Step_Az8";
                }
            }
            if (pchar.questTemp.Azzy == "HowToKnowAzzy")
            {
    			Link.l2 = "Послушай, мне срочно нужно найти Аззи!";
    			Link.l2.go = "Step_Az10";
            }
		break;
		//============================== Квест Аззи ===============================
 		case "Step_Az6":
        	dialog.text = "О-о-о, Сэр Аззи прислал тебя ко мне?! Очень рад слышать о нем - такой приятный и деловой человек! И что же он тебе сказал?";
    		link.l1 = "Человек?! Хм, ну ладно... Он сказал мне, что я могу брать у тебя ром бесплатно и сколько захочу.";
    		link.l1.go = "Step_Az7";
		break;

 		case "Step_Az7":
        	dialog.text = "Ну, не совсем так. Ты можешь брать у меня 300ц. рома один раз в день. И, разумеется, бесплатно.";
    		link.l1 = "Нормально, это меня устраивает. Приступим к процедуре?";
    		link.l1.go = "Step_Az8";
    		pchar.questTemp.Azzy.GiveRum = "OK";
		break;

 		case "Step_Az8":
        	dialog.text = NPCStringReactionRepeat("Я могу дать тебе 300ц. рома.",
                       "Ты уже брал сегодня положенный тебе ром.",
                       "Я же говорю тебе, что свой ром на сегодня ты выбрал. Приходи завтра.",
                       "Завтра, приходи завтра...", "block", 1, npchar, Dialog.CurrentNode);
    		link.l1 = HeroStringReactionRepeat("Хорошо, беру все!!!", "Верно, брал...", "Извини...", "Ну, тогда до завтра.", npchar, Dialog.CurrentNode);
    		link.l1.go = DialogGoNodeRepeat("Step_Az9", "none", "none", "none", npchar, Dialog.CurrentNode);
		break;

 		case "Step_Az9":
        	dialog.text = "Извольте провести погрузку в трюмы. Ром вам выдан.";
    		link.l1 = "Это хорошо. Спасибо, дружище.";
    		link.l1.go = "exit";
            AddCharacterGoods(Pchar, GOOD_RUM, 300);
		break;

 		case "Step_Az10":
        	dialog.text = NPCStringReactionRepeat("Я не знаю, где Аззи. Понятия не имею просто.",
                                    "Говорил уже, не знаю, где он.",
                                    "Повторяю, не знаю я, где Аззи.",
                                    "Еще раз говорю, что не знаю, где Аззи. Да и знать не желаю.", "block", 0, npchar, Dialog.CurrentNode);
    		link.l1 = HeroStringReactionRepeat("Но ты же получаешь от него ром, который я у тебя беру.", "Да, помню, говорил", "Повторять не надо, помню я.", "Жаль, очень жаль...", npchar, Dialog.CurrentNode);
    		link.l1.go = DialogGoNodeRepeat("Step_Az11", "none", "none", "none", npchar, Dialog.CurrentNode);
		break;

 		case "Step_Az11":
        	dialog.text = "Ваш ром я вообще не знаю, как мне привозят. Я просто на ночь оставляю сарай на заднем дворе открытым, а утром ром уже там. Третью часть я оставляю себе, две трети уходит тебе. Такова суть данной коммерческой сделки.";
    		link.l1 = "Вот черт... Значит надо подкараулить ночью тех, кто завозит тебе ром!";
    		link.l1.go = "Step_Az12";
		break;

 		case "Step_Az12":
        	dialog.text = "А вот этого я бы делать не стал. Мой помощник полюбопытствовал уже, спрятался днем в сарае, подсмотреть хотел. Только утром нашли его рядом с ромом тронутым, теперь сидит целыми днями, слюни пускает и мычит.\n"+
                          "Я не знаю, кто такой этот Аззи, но давно уже не рад дармовому рому. Страшно за себя, знаешь ли.\n"+
                          "А увидеть ты все одно ничего не увидишь. Запирал я уже сарай, обойдусь, думаю, без рома. А ром снова утром на месте. Вот так, дружище.";
    		link.l1 = "Да уж, понятно, что здесь Аззи я не найду.";
    		link.l1.go = "exit";
		break;
	}
	UnloadSegment(NPChar.FileDialog2);
}

