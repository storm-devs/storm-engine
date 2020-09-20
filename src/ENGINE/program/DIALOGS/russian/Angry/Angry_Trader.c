// 18.12.05 Эдди, файл торговцев на улицах
void GeneralAngryDialog(aref _pchar, aref npchar, aref Link)
{
    if (CheckAttribute(npchar, "angry") && !CheckAttribute(npchar, "angry.ok"))
    {
        npchar.angry.ok = 1;
        switch (npchar.angry.kind)
        {
            case "quest":
                Dialog.CurrentNode = "AngryQuest";
            break;
        }
    }
	switch(Dialog.CurrentNode)
	{
		case "AngryQuest":
            dialog.text = RandPhraseSimple("Я не хочу с тобой больше иметь дело.", "Больше я тебе ничего продавать не буду.");
			link.l1 = RandPhraseSimple("Ну как знаешь. По всему видать, деньги тебе не нужны.", "Невелика потеря...");
		    link.l1.go = "AngryExitAgain";
		    //===>> квест Аскольда, розыск порошка мумии.
            if (_pchar.questTemp.Ascold == "Seek_powder" && npchar.id == _pchar.questTemp.Ascold.TraderName)
            {
                dialog.text =  "Ты очень настойчив, видимо этот порошок тебе действительно нужен.";
    			link.l1 = "Именно так. А теперь я хочу знать, есть ли он у тебя на самом деле. И не бойся, все останется между нами.";
    		    link.l1.go = "AngStep_1";
            }
            if (CheckAttribute(npchar, "angry.terms")) //мирилка по квесту Аскольда через 10 дней.
            {
                if (GetNpcQuestPastDayParam(npchar, "angry.terms") > sti(npchar.angry.terms) && _pchar.questTemp.Ascold != "Seek_powder")
                {
         			dialog.text = NPCharSexPhrase(npchar, "Очень рассчитываю на то, что ты не будешь меня утомлять странными просьбами. Пожалуй, я готов вести дела с тобой.", "Очень надеюсь на то, что ты не будешь приставать ко мне с этими ужасными трупными покупками. Пожалуй, я готова торговать с тобой.");
        			link.l1 = RandPhraseSimple("Здорово!", "Превосходно!");
        			link.l1.go = "First time";
        			CharacterDelAngry(npchar);
                }
            }
    	break;

		case "AngryExitAgain":
            DialogExit();
            DeleteAttribute(npchar, "angry.ok")
		break;


//=========================>>>>>>>>>>>> блок квестов
		case "AngStep_1":
            dialog.text = "Есть, но не для всех, сам понимаешь. Но тебе продам. Тысяча монет, и он твой.";
            if (sti(_pchar.money) >= 1000)
            {
    			link.l1 = "Хорошо, вот твои деньги.";
    		    link.l1.go = "AngryExitAgain";
    		    _pchar.questTemp.Ascold = "PowderWasSeek";
    		    CharacterDelAngry(npchar);
    		    AddMoneyToCharacter(_pchar, -1000);
    		    GiveItem2Character(_pchar,"Powder_mummie");
            }
            else
            {
    			link.l1 = "Сейчас у меня с собой нет такой суммы. Как только соберу - сразу же навещу тебя.";
    		    link.l1.go = "AngryExitAgain";
            }
    	break;
//<<<<<<<<<<<<========================= блок квестов
	}
}

