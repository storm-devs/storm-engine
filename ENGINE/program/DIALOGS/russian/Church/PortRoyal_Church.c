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
			if (pchar.questTemp.State == "MorrisWillams_ArrestedSeekInfo" || pchar.questTemp.State == "MorrisWillams_ArrestedSeekInfoFather")
            {
                link.l1 = "Святой отец, скажите мне, местный ростовщик ваш прихожанин?";
                link.l1.go = "Step_E6_1";
            }
		break;

        case "Step_E6_1":
			dialog.text = "Да, он посещает церковь, но очень редко.";
			link.l1 = "Что он за человек?";
			link.l1.go = "Step_E6_2";
		break;

        case "Step_E6_2":
			dialog.text = "Мне сложно говорить о людях плохо.\n"+
                          "Я бы посоветовал тебе обратиться к одной нашей прихожанке Джиллиан Эттербери. Мне известно, что у нее серьезные проблемы как раз с этим человеком. Возможно, вы сможете ей даже в чем-то помочь.\n"+
                          "Джиллиан хорошая прихожанка, как правило она бывает в церкви часов с одиннадцати утра ежедневно и молится в течение двух-трех часов.\n"+
                          "А вообще ее в городе можно встретить.";
			link.l1 = "Спасибо, святой отец.";
			link.l1.go = "exit";
			pchar.questTemp.State = "MorrisWillams_ArrestedSeekInfoWomen";
			AddQuestRecord("Eng_Line_6_MorrisWillams", "19");
            ref sld = GetCharacter(NPC_GenerateCharacter("Gillian Atterbury", "girl_8", "woman", "towngirl", 5, ENGLAND, 100, false));
        	sld.name 	= "Джиллиан";
        	sld.lastname = "Эттербери";
        	LAi_SetCitizenType(sld);
            sld.Dialog.Filename = "Quest\EngLineNpc_2.c";
            LAi_group_MoveCharacter(sld, "ENGLAND_CITIZENS");
           	ChangeCharacterAddressGroup(sld, "PortRoyal_town", "goto", "goto1");
			LAi_SetFanatic(sld, "PortRoyal_church", "sit", "sit1", 11.0, 14.0);
		break;
	}
	UnloadSegment(NPChar.FileDialog2);  // если где-то выход внутри switch  по return не забыть сделать анлод
}

