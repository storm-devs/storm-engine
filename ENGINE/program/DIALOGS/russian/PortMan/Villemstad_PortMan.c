// диалог по городам
void ProcessCommonDialogEvent(ref NPChar, aref Link, aref NextDiag)
{
    switch (Dialog.CurrentNode)
	{
		case "quests":
			dialog.text = NPCStringReactionRepeat(RandPhraseSimple("Какие вопросы?", "Что вам угодно, " + GetAddress_Form(NPChar) + "?"), "Совсем недавно вы пытались задать мне вопрос " + GetAddress_Form(NPChar) + "...", "В течение этого дня вы уже третий раз говорите о каком-то вопросе...",
                          "Послушайте, если вы не по делам порта, то не стоит меня вопросами донимать.", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat(RandPhraseSimple("Я передумал...", "Сейчас мне не о чем говорить"), "Хм, что-то с памятью моей стало...",
                      "Да уж, действительно в третий раз...", "Извините, но портовые дела меня сейчас не интересуют.", npchar, Dialog.CurrentNode);
			link.l1.go = "exit";
            // ==> квест №5 голл.линейки, забрать три флейта.
            if (pchar.questTemp.State == "ThreeFleutes_toPortMan")
            {
    			link.l1 = "Я к вам по поручению Стэвезанта. Мне необходимо принять командование над тремя флейтами.";
    			link.l1.go = "Step_H5_1";
            }
		break;

//************************************* Диалоги по голл.линейке *******************************
		case "Step_H5_1":
    		if (GetCompanionQuantity(pchar) == 1)
            {
                dialog.text = "Пожалуйста, принимайте. Три флейта поступают в ваше распоряжение.";
    			link.l1 = "Великолепно.";
    			link.l1.go = "Step_H5_2";
            }
            else
            {
                dialog.text = "Да, я в курсе данного вопроса. Однако, чтобы принять командование над флейтами вам необходимо изабавиться от вашей эскадры. Прошу вас сделать это и затем вновь обратиться ко мне.";
    			link.l1 = "Хорошо, я так и сделаю.";
    			link.l1.go = "exit";
            }
		break;
		case "Step_H5_2":
            pchar.questTemp.State = "ThreeFleutes_toLaVega";
            ref sld;
            for (int i=1; i<=3; i++)
            {
                sld = GetCharacter(NPC_GenerateCharacter("Captain_"+i, "trader_"+(rand(15)+1), "man", "man", 15, HOLLAND, -1, true));
                FantomMakeCoolSailor(sld, SHIP_FLEUT, "", CANNON_TYPE_CANNON_LBS16, 30, 15, 15);
                FantomMakeCoolFighter(sld, 15, 10, 10, BLADE_SHORT, "pistol1", 0);
                sld.Ship.Cargo.Goods.Balls = 18;
                sld.Ship.Cargo.Goods.Grapes = 18;
                sld.Ship.Cargo.Goods.Knippels = 18;
                sld.Ship.Cargo.Goods.Bombs = 18;
                sld.Ship.Cargo.Goods.Weapon = 0;
                sld.Ship.Cargo.Goods.Powder = 50;
				sld.CompanionEnemyEnable = false; 
				sld.Abordage.Enable = false; // НЕЛЬЗЯ!
				SetShipRemovable(sld, false);
				AddCharacterGoods(sld, GOOD_CLOTHES, 5000);
                AddCharacterGoods(sld, GOOD_FRUITS, 200);
                AddCharacterGoods(sld, GOOD_RUM, 900);
                SetCompanionIndex(pchar, -1, sti(sld.index));
            	pchar.GenQuest.StoreGoods.StoreIdx = LaVega_STORE;
            	pchar.GenQuest.StoreGoods.HL5_ThreeFleutes = true;
                Pchar.quest.ThreeFleutes_TalkIntoLaVega.win_condition.l1 = "location";
                Pchar.quest.ThreeFleutes_TalkIntoLaVega.win_condition.l1.location = "LaVega_town";
                Pchar.quest.ThreeFleutes_TalkIntoLaVega.win_condition = "ThreeFleutes_TalkIntoLaVega";
            }
            DialogExit();
		break;
	}
	UnloadSegment(NPChar.FileDialog2);  // если где-то выход внутри switch  по return не забыть сделать анлод
}

