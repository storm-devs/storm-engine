// диалог по городам
void ProcessCommonDialogEvent(ref NPChar, aref Link, aref NextDiag)
{
    switch (Dialog.CurrentNode)
	{
		case "quests":
			dialog.text = NPCStringReactionRepeat(RandPhraseSimple("Какие вопросы, сын мой?", "Спрашивай, я слушаю тебя..."), "Я слушаю тебя, говори, сын мой...", "И в третий раз говорю тебе, сын мой: задавай свой вопрос.",
                          "Столько обязанностей у церковнослужителя, а тут еще и ты донимаешь, сын мой...", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat(RandPhraseSimple("Я передумал...", "Не сейчас, падре..."), "Да, пока особо и нечего сказать...",
                      "Задам, задам... Только позже...", "Простите, святой отец...", npchar, Dialog.CurrentNode);
			link.l1.go = "exit";
			//============= Изабелла Росселини ================
            if (CheckAttribute(Pchar, "RomanticQuest.PriestAsk"))
			{
				link.l2 = "Могу я узнать, часто ли ходит в церковь жена дона Сальватора?";
				link.l2.go = "Romantic_1";
			}
		break;

        case "Romantic_1":
			dialog.text = "Сальвотора Олеварес? Да, сын мой, часто, это достойная прихожанка... А в чем дело?";
			link.l1 = "Да просто так спросил, падре. До свидания...";
			link.l1.go = "exit";
			DeleteAttribute(Pchar, "RomanticQuest.PriestAsk");
		break;

        case "Romantic_2":
			dialog.text = GetFullName(pchar) + ", берешь ли ты в жены Изабеллу де Вальдес и клянешься жить с ней в мире и согласии, пока смерть не разлучит вас?";
			link.l1 = "Да, падре.";
			link.l1.go = "Romantic_3";
		break;
        case "Romantic_3":	
			locCameraToPos(3.64, 3.25, -7.84, false);
			LAi_SetActorType(npchar);
			LAi_ActorTurnToLocator(npchar, "goto", "goto5");
			dialog.text = "А ты, Изабелла де Вальдес, берешь ли ты в мужья " + GetMainCharacterNameGen() + " и клянешься жить с ним в мире и согласии, пока смерть не разлучит вас?";
			link.l1 = "Да, падре.";
			link.l1.go = "Romantic_4";
		break;
        case "Romantic_4":	
			locCameraToPos(-0.75, 3.85, -10.60, false);
			LAi_SetActorType(npchar);
			LAi_ActorTurnToLocator(npchar, "goto", "goto5");
			dialog.text = "Если кто-то из присутсвующих знает причину, по которой этот мужчина и эта женщина не могут быть вместе - пусть скажет это прямо сейчас или молчит вечно.";
			link.l1 = "...";
			link.l1.go = "Romantic_5";
		break;
        case "Romantic_5":		
			dialog.text = "Отныне вы муж и жена!";
			link.l1 = "...";
			link.l1.go = "exit";
			NextDiag.TempNode = "First time";
			AddDialogExitQuest("Romantic_Padre_3");			
		break;

	}
	UnloadSegment(NPChar.FileDialog2);  // если где-то выход внутри switch  по return не забыть сделать анлод
}
