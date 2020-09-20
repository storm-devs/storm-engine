// реплики солдат на палубе при разговоре в море
// форт-стража и комендант форта
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
        case "exit":
			DialogExit();
			NextDiag.CurrentNode = NextDiag.TempNode;
		break;
		
        case "First time":
			dialog.text = "Поговорите с нашим капитаном. Мне нечего Вам сказать";
			Link.l1 = "Хорошо";
			Link.l1.go = "exit";
			
			NextDiag.TempNode = "first time";
		break;
		//  на палубе -->
		case "On_Deck":
			dialog.text = "Поговорите с нашим капитаном. Мне нечего Вам сказать";
			Link.l1 = "Хорошо";
			Link.l1.go = "exit";

			NextDiag.TempNode = "On_Deck";
		break;
        //  на палубе <--
        case "On_Fort":
			dialog.text = RandPhraseSimple("Поговорите с начальником. Мне нечего Вам сказать.", "Я на посту. Все вопросы коменданту форта.");
			Link.l1 = "Хорошо";
			Link.l1.go = "exit";

			NextDiag.TempNode = "On_Fort";
		break;
		
		case "On_Fort_Head":
			dialog.text = RandPhraseSimple("Я сейчас страшно занят. Оставьте меня в покое.",
                                           "Не мешайте мне работать. Нам не о чем говорить.");
			Link.l1 = "Жаль...";
			Link.l1.go = "exit";

			NextDiag.TempNode = "On_Fort_Head";
		break;
		
		case "Morgan_wait_you":
			Dialog.defAni = "dialog_stay1";
			Dialog.defCam = "1";
			Dialog.defSnd = "dialogs\0\017";
			Dialog.defLinkAni = "dialog_1";
			Dialog.defLinkCam = "1";
			Dialog.defLinkSnd = "dialogs\woman\024";
			Dialog.ani = "dialog_stay2";
			Dialog.cam = "1";
			Dialog.snd = "dialogs\0\009";

			dialog.text = RandPhraseSimple("Морган хотел тебе сказать 'пару ласковых'... Он где-то здесь, на палубе.",
                                           "Хех, найди Моргана. Тебя ждут неприятные новости...");
			Link.l1 = "Хорошо.";
			Link.l1.go = "exit";

			NextDiag.TempNode = "Morgan_wait_you";
		break;

	}
}
