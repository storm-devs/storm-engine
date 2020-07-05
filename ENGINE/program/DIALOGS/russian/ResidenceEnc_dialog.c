
void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag, arRld, arDis;
	int i, Qty;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);
	
	switch(Dialog.CurrentNode)
	{
		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;

		case "exit_setOwner":
			LAi_SetOwnerTypeNoGroup(npchar);
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;

		case "fight":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
			LAi_SetOwnerTypeNoGroup(npchar);
			LAi_group_Attack(NPChar, Pchar);
			if (rand(3) != 1) SetNationRelation2MainCharacter(sti(npchar.nation), RELATION_ENEMY);
			AddDialogExitQuest("MainHeroFightModeOn");
		break;
		//--------------------------------- служанка в резиденции ---------------------------------
		case "ResWoman":
			NextDiag.TempNode = "ResWoman";
			if (isBadReputation(pchar, 40))
			{
				dialog.text = LinkRandPhrase("Шел бы ты отсюда, " + GetAddress_Form(NPChar) + ". Нечего шляться такому типу в доме у губернатора...", "Уходи, разбойник! Как вообще ты сюда попал?..", "Ну и времена настали. Всякая мерзость шастает по дому губернатора " + XI_ConvertString("Colony" + npchar.city + "Gen") + "!");
				link.l1 = RandPhraseSimple("Полегче, дура...", "Аккуратней в выражениях...");
				link.l1.go = "exit";
			}
			else
			{
				dialog.text = LinkRandPhrase("Здравствуйте. Я прислуживаю губернатору. Меня зовут " + GetFullName(NPChar) + ".", "Я занимаюсь уборкой в этой комнате, " + GetAddress_Form(NPChar) + ".", "Вы знаете, " + GetAddress_Form(NPChar) + ", наш губернатор - хороший человек, и неплохо платит прислуге...");
				link.l1 = RandPhraseSimple("Очень хорошо.", "Хм, прекрасно...");
				link.l1.go = "exit";
			}
		break;
		case "Woman_FackYou":
			dialog.text = "Ой, что же это делается?! А я-то дура, хожу, ничего не замечаю... Охрана!!";
			link.l1 = "Заткнись, дура...";
			link.l1.go = "exit_setOwner";
			LAi_group_Attack(NPChar, Pchar);
			if (rand(3) != 1) SetNationRelation2MainCharacter(sti(npchar.nation), RELATION_ENEMY);
		break;
		//--------------------------------- слуга в резиденции ---------------------------------
		case "ResMan":
			NextDiag.TempNode = "ResMan";
			if (isBadReputation(pchar, 40))
			{
				dialog.text = LinkRandPhrase("Пошел вон отсюда, мерзавец!", "Немедленно убрайся из дома губернатора, негодяй!", "Мне нет дела до того, что тебе нужно в доме губернатора " + XI_ConvertString("Colony" + npchar.city + "Gen") + ". Но дам тебе совет - убирайся отсюда!");
				link.l1 = RandPhraseSimple("Полегче, родной...", "Смотри не подавись своими словами!");
				link.l1.go = "exit";
			}
			else
			{
				dialog.text = LinkRandPhrase("Здравствуйте. Я - слуга губернатора. Меня зовут " + GetFullName(NPChar) + ".", "Я присматриваю здесь за порядком, " + GetAddress_Form(NPChar) + ".", "Вы знаете, " + GetAddress_Form(NPChar) + ", наш губернатор - хороший человек, и неплохо платит прислуге...");
				link.l1 = RandPhraseSimple("Очень хорошо.", "Хм, прекрасно...");
				link.l1.go = "exit";
			}
		break;
		case "Man_FackYou":
			dialog.text = "Да ты никак вздумал обворовать губернатора?! С ума сошел!.. Охрана, держи вора!!";
			link.l1 = "Закрой пасть...";
			link.l1.go = "fight";
			LAi_group_Attack(NPChar, Pchar);
			if (rand(3) != 1) SetNationRelation2MainCharacter(sti(npchar.nation), RELATION_ENEMY);
		break;
		//--------------------------------- солдат в резиденции ---------------------------------
		case "ResGuard":
			NextDiag.TempNode = "ResGuard";
			if (isBadReputation(pchar, 40))
			{
				dialog.text = LinkRandPhrase("Шел бы отсюда, приятель...", "Проваливай отсюда!", "Ты, часом, дверью не ошибся? Это дом губернатора " + XI_ConvertString("Colony" + npchar.city + "Gen") + ". Тебе лучше уйти!");
				link.l1 = RandPhraseSimple("Уйду, когда захочу!", "Не лезь не в свое дело...");
				link.l1.go = "exit";
			}
			else
			{
				dialog.text = LinkRandPhrase("Сегодня меня поставили в наряд к губернатору. Неплохое место...", "Я охраняю дом губернатора, " + GetAddress_Form(NPChar) + ".", "Моя задача - охранять дом губернатора.");
				link.l1 = RandPhraseSimple("Здорово.", "Хм, понятно...");
				link.l1.go = "exit";
			}
		break;
	}
}
