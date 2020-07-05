
void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;
	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);
	
	switch(Dialog.CurrentNode)
	{
		case "First time":
			if (GetNationRelation2MainCharacter(sti(NPChar.nation)) == RELATION_ENEMY)
			{
					dialog.text = RandPhraseSimple("Братья, в Инквизиции враг!!!", "Поднимайте тревогу, братья!!");
					link.l1 = "Да уж, враги Инквизиции не дремлют...";
					link.l1.go = "fight";			
			}
			else
			{
				dialog.text = NPCStringReactionRepeat(RandPhraseSimple("Здравствуй, сын мой. Какими судьбами у нас?..", "Рад видеть тебя в нашей обители, сын мой. Зачем пожаловал?"),
							"Сын мой, мы уже говорили с тобой сегодня. Тебе нужно что-то еще?", "Сын мой, послушников нашего ордена не отличает терпение. Еще раз тебя спрашиваю: тебе что-нибудь нужно?",
							RandPhraseSimple("Негоже находится в праздности самому и отвлекать других от работы. Более я не произнесу ни слова...", "Сын мой, я не желаю вести бессмысленные разговоры. Тебе я не скажу более ни слова."), "block", 1, npchar, Dialog.CurrentNode);
				link.l1 = HeroStringReactionRepeat(RandPhraseSimple("Да так, заскочил по делам, святой отец...", "Все по делам, падре. Тружусь, аки пчела - весь в заботах..."), 
							"Да нет, святой отец...", "Нет, падре, просто хотел поболтать...", RandPhraseSimple("Хм...", "Ну, как знаете, святой отец..."), npchar, Dialog.CurrentNode);
				link.l1.go = DialogGoNodeRepeat("exit", "none", "none", "NoMoreTalkExit", npchar, Dialog.CurrentNode);				
			}
			if (CheckAttribute(npchar, "protector.CheckAlways")) //гарды на камерах
			{
				if (GetNationRelation2MainCharacter(sti(NPChar.nation)) == RELATION_ENEMY)
				{					
					dialog.text = "Черт возьми, это шпион! Держи его!!!";
					link.l1 = "Каррамба!!";
					link.l1.go = "fight";
					StartIncquisitioAttack();
				}
				else
				{
					dialog.text = LinkRandPhrase("Это охраняемая камера, вход сюда категорически запрещен!!!", "Даже и не думай сюда входить - карается смертью.", "Один шаг внутрь этой камеры - и тебе конец...");
					link.l1 = RandPhraseSimple("Ясно, солдат.", "Понятно...");	
					link.l1.go = "exit";
				}
			}
			if (findsubstr(npchar.id, "Prisoner", 0) != -1) //заключенные
			{
				if (LAi_group_IsActivePlayerAlarm())
				{
					dialog.text = LinkRandPhrase("Хо-хо, до чего же весело смотреть, как 'святоши' отправляются прямиком в ад!", "Господи, неужели хоть кто-то осмелился бросить им вызов!!", "Я уже не жилец на этом свете - пытки... Но ты доставил мне последнее удовольствие в жизни. Спасибо...");
					link.l1 = RandPhraseSimple("Хех!", "Аргх...");
					link.l1.go = "exit";
					//==> квест №7, базар о Роке Бразильце. 
					if (pchar.questTemp.State == "Fr7RockBras_toSeekPlace")
					{
						link.l1 = "Послушай, ты не знаешь здесь такого человека - Рок Бразилец?";
						link.l1.go = "Step_F7_2";
					}
					//<== квест №7, базар о Роке Бразильце. 
				}
				else
				{
					dialog.text = LinkRandPhrase("Нет ничего людского у этих 'святош'. Попомни мое слово - гореть им в аду за наши муки...", "Ежедневные истязания и пытки! А ведь я же ни в чем не виноват перед Господом!!!", "Вчера умер мой сосед по камере, они два дня пытали его на дыбе... А сколько мне осталось?..");
					link.l1 = RandPhraseSimple("Хм... Ну, не знаю...", "Что же, понимаю...");
					link.l1.go = "exit";
					//==> квест №7, базар о Роке Бразильце. 
					if (pchar.questTemp.State == "Fr7RockBras_toSeekPlace")
					{
						link.l1 = "Послушай, ты не знаешь здесь такого человека - Рок Бразилец?";
						link.l1.go = "Step_F7_1";
					}
					//<== квест №7, базар о Роке Бразильце. 
				}
			}
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;

		case "NoMoreTalkExit":
			LAi_CharacterDisableDialog(npchar);
			DialogExit();
		break;

		case "fight":
			DialogExit();
			NextDiag.CurrentNode = NextDiag.TempNode;
			LAi_group_Attack(NPChar, Pchar);
		break;

//**************************** Фр. линейка. Квест №7, спасение Рока Бразильца ********************************
 		case "Step_F7_1":
			dialog.text = "Знаю, в крайней камере сидит, под охраной вон того солдата. Особенное место...";
			link.l1 = "Спасибо тебе!";
			link.l1.go = "exit";
		break;
 		case "Step_F7_2":
			dialog.text = "В крайней камере справа от входа, там где стоял часовой, пока ты его не убил...";
			link.l1 = "Ага, понятно... Спасибо тебе.";
			link.l1.go = "exit";
		break;

	}
}
