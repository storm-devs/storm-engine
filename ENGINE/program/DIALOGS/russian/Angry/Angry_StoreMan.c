// 28.11.05 Эдди, общий angry-диалог для торговцев
void GeneralAngryDialog(aref _pchar, aref npchar, aref Link)
{
    if (CheckAttribute(npchar, "angry") && !CheckAttribute(npchar, "angry.ok"))
    {
        npchar.angry.ok = 1;
        switch (npchar.angry.kind)
        {
            case "repeat":
                Dialog.CurrentNode = "AngryRepeat";
            break;
        }
    }
	switch(Dialog.CurrentNode)
	{
		case "AngryRepeat":
            dialog.text = LinkRandPhrase(GetFullName(_pchar) + " в моем магазине вы более не обслуживаетесь. Не буду разговаривать.", "Не буду общаться, мой магазин для тебя закрыт.", "Ты меня здорово разозлил, так что в моем магазине тебя обслуживать не будут.");
			link.l1 = LinkRandPhrase("Обойдемся без твоего магазина.", "Ну, если тебе деньги не нужны...", "Хм, затаримся в другом месте.");
		    link.l1.go = "AngryExitAgain";
    		link.l2 = RandPhraseSimple("Послушай, ну так получилось... Давай как-то налаживать отношения, мне нужны услуги твоего магазина.", "Слушай, торговля с тобой для меня важна. Может забудем эту глупую вражду?");
			link.l2.go = "CheckTimeOver";
    		link.l3 = "Что я могу сделать, чтобы ты изменил мнение обо мне в лучшую сторону?";
			link.l3.go = "AngryStory";
    	break;

		case "CheckTimeOver": //примирение через некоторое время
			dialog.text = RandPhraseSimple("После дождичка в четверг...", "Нет уж, такие клиенты мне ни к чему.");
			link.l1 = RandPhraseSimple("Ну, как знаешь.", "Вот упрямый осел!");
			link.l1.go = "AngryExitAgain";
            if (CheckAttribute(npchar, "angry.terms"))
            {
                if (GetNpcQuestPastDayParam(npchar, "angry.terms") > sti(npchar.angry.terms))
                {
         			dialog.text = "Хм, действительно пора бы нам примириться, уже прошло достаточно времени... Ну что ж, мой магазин вновь к твоим услугам.";
        			link.l1 = RandPhraseSimple("Здорово!", "Превосходно!");
        			link.l1.go = "node_1";
        			CharacterDelAngry(npchar);
                }
            }
		break;

		case "AngryExitAgain":
            DialogExit();
            DeleteAttribute(npchar, "angry.ok");
		break;

		case "AngryStory":
			dialog.text = "А что ты можешь предложить полезное для меня и моего магазина?";
			link.l1 = "Деньги.";
			link.l1.go = "ProposalMoney";
			link.l2 = "Предмет какой-нибудь. В качестве подарка, разумеется...";
			link.l2.go = "ProposalItem";
		break;
//================ блок вариантов к примерению =================
		case "ProposalMoney":
			dialog.text = NPCStringReactionRepeat("Ну что ж, думаю, что сумма в " + npchar.angry.QtyMoney + " монет сможет возместить мне моральный ущерб.", "Повторяю, сумма размером в " + npchar.angry.QtyMoney + " сможет нас помирить.", "Еще раз для тех, кто очень плохо понимает. " + npchar.angry.QtyMoney + " - и мы снова лучшие друзья.", "none", "block", 0, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat("Такой дорогостоящий моральный ущерб я оплачивать не буду.", "Слышал, но платить не буду.", "Все ясно, но платить и не собираюсь.", "none", npchar, Dialog.CurrentNode);
			link.l1.go = "AngryExitAgain";
            if (sti(_pchar.money) >= sti(npchar.angry.QtyMoney))
            {
    			link.l2 = "Я готов все возместить.";
    			link.l2.go = "GiveMoney";
            }
		break;

		case "GiveMoney":
			dialog.text = "Очень рад вашему благоразумию, друг мой. Двери моего магазина для тебя вновь открыты.";
            link.l1 = RandPhraseSimple("Здорово!", "Превосходно!");
			link.l1.go = "node_1";
			AddMoneyToCharacter(_pchar, -sti(npchar.angry.QtyMoney));
			CharacterDelAngry(npchar);
		break;

		case "ProposalItem":
			dialog.text = NPCStringReactionRepeat("Ты в магазине находишься, я предметы продаю. Ничего не нужно.", "none", "none", "none", "block", 0, npchar, Dialog.CurrentNode);
            link.l1 = HeroStringReactionRepeat("Очень жаль...", "none", "none", "none", npchar, Dialog.CurrentNode);
			link.l1.go = "AngryExitAgain";
		break;

	}
}

