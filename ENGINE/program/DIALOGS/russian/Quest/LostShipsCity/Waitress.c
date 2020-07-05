
void ProcessDialogEvent()
{
	ref NPChar, sld;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

	string NodeName = Dialog.CurrentNode;
	string NodePrevName = "";
	if (CheckAttribute(NextDiag, "PrevNode")) NodePrevName = NextDiag.PrevNode;

	switch(Dialog.CurrentNode)
	{
        case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
		
		case "First time":
			NextDiag.TempNode = "First time";
			if (LAi_grp_playeralarm > 0)
			{
       			dialog.text = "Я не желаю с тобой разговаривать! Ты напал на граждан!!";
				link.l1 = "Как знаешь..."
				link.l1.go = "exit";
				break;
			}
			if(NPChar.quest.meeting == "0")
			{
				npchar.quest.meeting = "1";
				if (CheckAttribute(loadedLocation, "storm"))
				{
					dialog.Text = "Да-да, я знаю вас! Этот шторм погубил ваш корабль. Что ж, такова ваша судьба, и теперь она объединена с нашей! Мы все заключены на этом острове...";
					Link.l1 = "Да уж, успел заметить...";
					link.l1.go = "FT_1";
				}
				else
				{				
					dialog.Text = "Да-да, я знаю вас! Это ваш корабль затонул тут последним. Что ж, такова ваша судьба, и теперь она объединена с нашей! Мы все заключены на этом острове...";
					Link.l1 = "Да уж, успел заметить...";
					Link.l1.go = "FT_1";
				}
			}
			else
			{
				dialog.text = "Что вы хотели, месье капитан?";
				Link.l1 = "Хочу задать тебе пару вопросов, " + npchar.name + ".";
				Link.l1.go = "int_quests";
				link.l10 = "Да так, ничего, собственно...";
				link.l10.go = "exit";
			}
		break;
		case "FT_1":
			dialog.text = "Ну что же, давайте знакомиться. Меня зовут " + GetFullName(npchar) + ". А вас?";
			link.l1 = "Мое имя - " + GetFullName(pchar) + ". Я капитан... К-хе, бывший капитан...";
			link.l1.go = "FT_2";
		break;
		case "FT_2":
			dialog.text = "Так вы капитан того корабля, что затонул?! Представляю, как вас мучает совесть за гибель своих людей. Ох, мне очень жаль, простите...";
			link.l1 = "Да без проблем, собственно...";
			link.l1.go = "FT_3";
		break;
		case "FT_3":
			dialog.text = "Я буду называть вас 'месье капитанан'! Вы не против?";
			link.l1 = "Не против, " + npchar.name + ". Называй как хочешь...";
			link.l1.go = "exit";
		break;
		//вопросы
		case "int_quests":
			dialog.text = "Задавайте, капитан.";
			if (!CheckAttribute(NextDiag, NodeName+".l3"))
			{
				link.l3 = "Здесь как-то неуютно, ты не находишь? Еще штормит тут у вас...";
				link.l3.go = "ansewer_3";
			}
			if (!CheckAttribute(NextDiag, NodeName+".l1"))
			{
				link.l1 = "А ты как сюда попала?";
				link.l1.go = "ansewer_1";
			}
			if (!CheckAttribute(NextDiag, NodeName+".l2"))
			{
				link.l2 = "Ты не хочешь выбраться отсюда?";
				link.l2.go = "ansewer_2";
			}
			if (!CheckAttribute(NextDiag, NodeName+".l4"))
			{
				link.l4 = "Послушай, а что за кланы у вас тут процветают? Я имею ввиду 'нарвалов' и 'касперов'.";
				link.l4.go = "ansewer_4";
			}
			link.l10 = "Ты знаешь, что-то я забыл, о чем хотел спросить. Извини, что отвлек...";
			link.l10.go = "exit";
		break;
		case "ansewer_3":
			dialog.text = "Штормит, да... Но вы же моряк, вам не привыкать.";
			link.l1 = "Это да, но все же... мрачновато как-то.";
			link.l1.go = "ansewer_3_1";
			NextDiag.(NodePrevName).l3 = true;
		break;
		case "ansewer_3_1":
			dialog.text = "Не падайте духом, это бывает у всех, кто сюда попадает. Все образуется, главное не делать глупостей.";
			link.l1 = "Глупостей?";
			link.l1.go = "ansewer_3_2";
		break;
		case "ansewer_3_2":
			dialog.text = "Не ссорьтесь с адмиралом, соблюдайте Закон - и все будет хорошо.";
			link.l1 = "Понятно... Спасибо за поддержку.";
			link.l1.go = "exit";
		break;
		case "ansewer_1":
			dialog.text = "Так же, как и все здесь. Наш корабль затонул, я сумела выплыть. Вообще знаете, с кораблей, что идут на дно, очень мало народу выплывает. Тут какое-то течение странное, что-ли... В общем, не знаю я, но факт, как говорится, налицо.";
			link.l1 = "Ясно...";
			link.l1.go = "exit";
			NextDiag.(NodePrevName).l1 = true;
		break;
		case "ansewer_2":
			dialog.text = "Хи-хи, все хотят выбраться отсюда. Только это невозможно.";
			link.l1 = "Ты уверена?";
			link.l1.go = "ansewer_2_1";
			NextDiag.(NodePrevName).l2 = true;
		break;
		case "ansewer_2_1":
			dialog.text = "Да, к сожалению. Корабли приходят только сюда, отсюда никто уже не уплывает. А строить новые корабли запрещено Законом.";
			link.l1 = "Понятно.";
			link.l1.go = "exit";
		break;
		case "ansewer_4":
			dialog.text = "Хм... Я тебе скажу одну очень простую ведь, и, надеюсь, ты ее запомнишь как следует. Не лезь к ним!!";
			link.l1 = "А в чем дело?";
			link.l1.go = "ansewer_4_1";
			NextDiag.(NodePrevName).l4 = true;
		break;
		case "ansewer_4_1":
			dialog.text = "Дело в том, что тебя просто убьют. Раньше было такое не раз с новенькими, так не повторяй ошибок предшественников. Мне очень неприятно будет видеть твой плавающий за бортами труп.";
			link.l1 = "Спасибо, учту.";
			link.l1.go = "exit";
		break;
	}
	NextDiag.PrevNode = NodeName;
}
 
