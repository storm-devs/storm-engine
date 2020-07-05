// boal 25/04/04 общий диалог горожан
#include "DIALOGS\russian\Rumours\Common_rumours.c"  //homo 25/06/06
void ProcessDialogEvent()
{
	ref NPChar, sld;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

    // вызов диалога по городам -->
    NPChar.FileDialog2 = "DIALOGS\" + LanguageGetLanguage() + "\Citizen\" + NPChar.City + "_Citizen.c";
    if (LoadSegment(NPChar.FileDialog2))
	{
        ProcessCommonDialog(NPChar, Link, NextDiag);
		UnloadSegment(NPChar.FileDialog2);
	}
    // вызов диалога по городам <--
    ProcessCommonDialogRumors(NPChar, Link, NextDiag);//homo 16/06/06
    int iTest, iTemp;
    iTest = FindColony(NPChar.City); // город
    ref rColony;
	if (iTest != -1)
	{
		rColony = GetColonyByIndex(iTest);
	}
	
	switch(Dialog.CurrentNode)
	{
		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
		
		case "fight":
            LAi_group_Attack(NPChar, Pchar);
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
		
		case "First time":
            /*if (GetNationRelation2MainCharacter(sti(NPChar.nation)) == RELATION_ENEMY && sti(NPChar.nation) != PIRATE)
			{
				dialog.text = RandPhraseSimple("Шпион? Стража!!!!!", "Караул! В городе враг!");
				link.l1 = "Заткнись! Я уже ухожу.";
				link.l1.go = "fight";
				break;
			}*/
			
			if (sti(NPChar.nation) == PIRATE)
			{
				if (sti(rColony.HeroOwn) == true) // наш горожанин
				{
				    dialog.text = RandPhraseSimple("Ходит слух, что вы и есть владелец этого города, сэр.", "Какая приятная встреча, сам глава города обратился ко мне с вопросом.");
					link.l1 = RandPhraseSimple("Я передумал. Удачи!", "Просто осматриваю город. До свидания.");
					link.l1.go = "exit";
					link.l2 = RandPhraseSimple("У меня есть вопрос к тебе.", "Мне нужна информация.");
					link.l2.go = "quests";//(перессылка в файл города)
					if (CheckAttribute(pchar, "GenQuest.Loan.FindCitizen") && !CheckAttribute(pchar, "GenQuest.Loan.FindCitizenDone") && !CheckAttribute(pchar, "GenQuest.Loan.FindCitizenFalied"))
		            {
		                if (Characters[sti(pchar.GenQuest.Loan.FindCitizenIdx)].id == Npchar.id)
		                {
							link.l3 = RandPhraseSimple("Хочу обсудить финансовые дела.", "Касательно финансов...");
							link.l3.go = "LoanForAll";//(перессылка в кредитный генератор)
						}
					}
					break;
				}
				else
				{ // пираты, не наши
				    dialog.text = RandPhraseSimple("Чего тебе? Проходи мимо.", "Хватит загораживать дорогу, отвали.");
					link.l1 = RandPhraseSimple("Уже ушел.", "Не нужно мне грубить. До свидания.");
					link.l1.go = "exit";
					link.l2 = RandPhraseSimple("Всего один маленький вопросик.", "Минутку внимания, всего один небольшой вопрос.");
					link.l2.go = "quests";//(перессылка в файл города)
					if (CheckAttribute(pchar, "GenQuest.Loan.FindCitizen") && !CheckAttribute(pchar, "GenQuest.Loan.FindCitizenDone") && !CheckAttribute(pchar, "GenQuest.Loan.FindCitizenFalied"))
		            {
		                if (Characters[sti(pchar.GenQuest.Loan.FindCitizenIdx)].id == Npchar.id)
		                {
							link.l3 = RandPhraseSimple("Хочу обсудить финансовые дела.", "Касательно финансов...");
							link.l3.go = "LoanForAll";//(перессылка в кредитный генератор)
						}
					}
					break;
				}
			}
            if(NPChar.quest.meeting == "0")
			{
				NPChar.quest.meeting = "1";

                // homo 05/08/06
				dialog.Text =NPCharRepPhrase(npchar,
                                LinkRandPhrase("Меня зовут ", "Мое имя ", "Можешь называть меня ")
                                +GetFullName(npchar)
                                +LinkRandPhrase(". Чего тебе нужно?", ". Я тебя раньше здесь не "
                                +NPCharSexPhrase(NPChar, "видел", "видела")
                                +", кто ты такой?", ". Кто ты и что тебе от меня надо?"),

        		                LinkRandPhrase("Приветствую вас, " + GetAddress_Form(NPChar) + ". Мое имя " + GetFullName(npchar) + ". А как зовут вас?",
                                 "Здравствуйте, " + GetAddress_Form(NPChar) + "! Я " + GetFullName(npchar) + ". Могу я узнать ваше имя?",
                                 "Да, " + GetAddress_Form(NPChar) + ". Что вы хотели? Кстати, меня зовут " + GetFullName(npchar) + ". А вас?"));

				Link.l1 = pcharrepphrase(
                                LinkRandPhrase("Разрази меня гром! ", "Тысяча чертей! ", "Чтоб тебе пусто было! ") + "Да я капитан "+ GetFullName(Pchar)+ LinkRandPhrase(", ты разве не слыхал"+NPCharSexPhrase(NPChar,"","а")+" обо мне, "+NPCharSexPhrase(NPChar,"каналья?","?"), " и самый известный пират в этих водах! ", " и провалиться мне на этом месте, если это не так!"),
                                LinkRandPhrase("Я " + GetFullName(Pchar) + ", капитан.", "Меня зовут " + GetFullName(Pchar) + ".", "Можешь называть меня капитан " + GetFullName(Pchar)));

                //LinkRandPhrase("Я " + GetFullName(Pchar) + ", капитан.", "Меня зовут " + GetFullName(Pchar) + ".", "Можешь называть меня капитан " + GetFullName(Pchar));
				Link.l1.go = "Meeting";
			}
			else
			{
				link.l1 = PCharRepPhrase("Хотел кое-что узнать об этом городишке!", "Хотел узнать кое-что о городе.");
				link.l1.go = "new question";
 				link.l2 = RandPhraseSimple("У меня есть вопрос к тебе.", "Мне нужна информация.");
				link.l2.go = "quests";//(перессылка в файл города)
				if (CheckAttribute(pchar, "GenQuest.Loan.FindCitizen") && !CheckAttribute(pchar, "GenQuest.Loan.FindCitizenDone") && !CheckAttribute(pchar, "GenQuest.Loan.FindCitizenFalied"))
	            {
	                if (Characters[sti(pchar.GenQuest.Loan.FindCitizenIdx)].id == Npchar.id)
	                {
						link.l3 = RandPhraseSimple("Хочу обсудить финансовые дела.", "Касательно финансов...");
						link.l3.go = "LoanForAll";//(перессылка в кредитный генератор)
					}
				}
				// --> квестовый генератор мэра, поиск шпиёна на улице. eddy
				if (CheckAttribute(pchar, "GenQuest.SeekSpy.City") && pchar.location == (npchar.city + "_town"))
	            {
					link.l4 = NPCharSexPhrase(npchar, LinkRandPhrase("Стойте! Немедленно скажите мне, кто вы такой?! Я ищу вражеского лазутчика по поручению губернатора " + XI_ConvertString("Colony"+characters[GetCharacterIndex(pchar.GenQuest.SeekSpy.MayorId)].city+"Gen") + "!", 
						"Немедленно стой! Я действую в интересах города " + XI_ConvertString("Colony"+characters[GetCharacterIndex(pchar.GenQuest.SeekSpy.MayorId)].city+"Gen") + " по поручению губернатора. Ищу в городе вражеского агента.", 
						"Стоять, приятель! Местный губернатор " + characters[GetCharacterIndex(pchar.GenQuest.SeekSpy.MayorId)].lastname + " очень хочет разыскать шпиона в городе."), 
						LinkRandPhrase("Женщина, стойте! Немедленно скажите мне, кто вы такая?! Я ищу вражеского лазутчика по поручению губернатора " + XI_ConvertString("Colony"+characters[GetCharacterIndex(pchar.GenQuest.SeekSpy.MayorId)].city+"Gen") + "!", 
						"Женщина, я должен вас задержать! Я действую в интересах города " + XI_ConvertString("Colony"+characters[GetCharacterIndex(pchar.GenQuest.SeekSpy.MayorId)].city+"Gen") + " по поручению губернатора. Ищу в городе вражеского агента.", 
						"Постой, красавица! Местный губернатор " + characters[GetCharacterIndex(pchar.GenQuest.SeekSpy.MayorId)].lastname + " очень хочет разыскать шпиона в городе..."));
					link.l4.go = "SitySpy";
				}
				// <-- квестовый генератор мэра, поиск шпиёна на улице
                dialog.Text = NPCharRepPhrase(npchar,
		                PCharRepPhrase(LinkRandPhrase("А, это ты "+GetFullName(Pchar)
                        +" - старый пройдоха", "Рад тебя видеть старина "
                        + Pchar.name, "О, да это же сам капитан "
                         +GetFullName(Pchar))
                                        +LinkRandPhrase(". Думал, тебя уже нет на этом свете!", ". И, наверное, как всегда пьян.", ". И, как вижу, еще на свободе!")
                                        +LinkRandPhrase(" Ну, говори, чего тебе нужно?", " Чего тебе на этот раз?", " Опять тревожишь меня по пустякам?"),
                                        LinkRandPhrase(TimeGreeting() + ", капитан ", "Здравствуйте, "+GetAddress_Form(NPChar)+" ", "Приветствую вас, капитан ")+GetFullName(Pchar)
                                        +LinkRandPhrase(". Что может быть нужно такому почтенному человеку, как вы, от меня?", ". Зачем вы пришли?", ". Что на этот раз вы хотите узнать?")

                                        ),
                                        
                        PCharRepPhrase(LinkRandPhrase("Здравствуйте, капитан ", "Приветствую, ах это вы "+GetAddress_Form(NPChar)+" ", "А, капитан ")
                                        +GetFullName(Pchar)
                                        +LinkRandPhrase(", я думал"+NPCharSexPhrase(NPChar, " ", "а ")+"мы больше с вами не встретимся,",
                                                         ", не скажу, что рад"+NPCharSexPhrase(NPChar, " ", "а ")+"вас видеть, но", ", вижу, вы еще живы, прискорбно, а")
                                        +LinkRandPhrase(" что вам нужно?", " зачем вы пришли?", " чем могу быть "+NPCharSexPhrase(NPChar, "полезен?", "полезна?")),
                                        LinkRandPhrase(TimeGreeting() + ", капитан " + GetFullName(Pchar) + ". Чем могу помочь?",
                                        "О, да это же сам капитан " + GetFullName(Pchar) + "! Что вы хотите узнать на этот раз?",
                                        "Здравствуйте, " + GetAddress_Form(NPChar) + " " + Pchar.lastname + ". Вы что-то хотели?"))
                                        );                         
	   			link.l5 = PCharRepPhrase(RandPhraseSimple("Тебе послышалось, я пойду.", "Нет, ничего - просто шел в таверну."),
                                        RandPhraseSimple("Ладно, ничего. Удачи!", "Просто осматриваю город. До свидания."));
				link.l5.go = "exit";
			}
			NextDiag.TempNode = "First time";
		break;
		
		case "Meeting":
		    dialog.Text  = NPCharRepPhrase(npchar, LinkRandPhrase("А ты не врешь, капитан " + GetFullName(Pchar)+"?",
                                                                    "И что с того " + PChar.name+"?",
                                                                    "Я запомню тебя, " + GetFullName(Pchar)+".")
                                                                     + " А теперь говори, чего тебе надо?",
                                                    LinkRandPhrase("Приятно познакомиться, капитан " + PChar.name,
                                                                    "Рад"+NPCharSexPhrase(NPChar, " ", "а ")+" нашему знакомству, " + GetAddress_Form(NPChar) + " " + PChar.lastname,
                                                                     "Очень приятно, капитан " + PChar.name) + ". Но вряд ли вы просто хотели узнать мое имя, не так ли?");

            link.l1 = PCharRepPhrase("Хотел кое-что узнать об этом городишке!", "Хотел узнать кое-что о городе.");
			link.l1.go = "new question";
            Link.l2 = RandPhraseSimple("У меня есть вопрос к тебе.", "Мне нужна информация.");
			Link.l2.go = "quests";
			if (CheckAttribute(pchar, "GenQuest.Loan.FindCitizen") && !CheckAttribute(pchar, "GenQuest.Loan.FindCitizenDone") && !CheckAttribute(pchar, "GenQuest.Loan.FindCitizenFalied"))
            {
                if (Characters[sti(pchar.GenQuest.Loan.FindCitizenIdx)].id == Npchar.id)
                {
					link.l3 = RandPhraseSimple("Хочу обсудить финансовые дела.", "Касательно финансов...");
					link.l3.go = "LoanForAll";//(перессылка в кредитный генератор)
				}
			}
			// --> квестовый генератор мэра, поиск шпиёна на улице. eddy
			if (CheckAttribute(pchar, "GenQuest.SeekSpy.City") && pchar.location == (npchar.city + "_town"))
	        {
				link.l4 = NPCharSexPhrase(npchar, LinkRandPhrase("Стойте! Немедленно скажите мне, кто вы такой?! Я ищу вражеского лазутчика по поручению губернатора " + XI_ConvertString("Colony"+characters[GetCharacterIndex(pchar.GenQuest.SeekSpy.MayorId)].city+"Gen") + "!", 
					"Немедленно стой! Я действую в интересах города " + XI_ConvertString("Colony"+characters[GetCharacterIndex(pchar.GenQuest.SeekSpy.MayorId)].city+"Gen") + " по поручению губернатора. Ищу в городе вражеского агента.", 
					"Стоять, приятель! Местный губернатор " + characters[GetCharacterIndex(pchar.GenQuest.SeekSpy.MayorId)].lastname + " очень хочет разыскать шпиона в городе."), 
					LinkRandPhrase("Женщина, стойте! Немедленно скажите мне, кто вы такая?! Я ищу вражеского лазутчика по поручению губернатора " + XI_ConvertString("Colony"+characters[GetCharacterIndex(pchar.GenQuest.SeekSpy.MayorId)].city+"Gen") + "!", 
					"Женщина, я должен вас задержать! Я действую в интересах города " + XI_ConvertString("Colony"+characters[GetCharacterIndex(pchar.GenQuest.SeekSpy.MayorId)].city+"Gen") + " по поручению губернатора. Ищу в городе вражеского агента.", 
					"Постой, красавица! Местный губернатор " + characters[GetCharacterIndex(pchar.GenQuest.SeekSpy.MayorId)].lastname + " очень хочет разыскать шпиона в городе..."));
				link.l4.go = "SitySpy";
			}
			// <-- квестовый генератор мэра, поиск шпиёна на улице
			Link.l5 = PCharRepPhrase(RandPhraseSimple("Тебе послышалось, я пойду.", "Нет, ничего - просто шел в таверну."),
                                        RandPhraseSimple("Ладно, ничего. Удачи!", "Просто осматриваю город. До свидания."));
			Link.l5.go = "exit";
		break;
		
		case "new question":
            dialog.text = NPCharRepPhrase(npchar,
		                PCharRepPhrase(LinkRandPhrase("О, да я рад"+NPCharSexPhrase(NPChar, " ", "а ")+" тебе, как выпивке, спрашивай, чего хочешь.", "Любишь поболтать, капитан? Что ж, я тоже... Особенно за кружечкой рома.", "Да, " + PChar.name + "?"),
                                        LinkRandPhrase("Ну, чего тебе еще?", "Вижу, капитан "+PChar.name+", ты охотник поболтать? ", "Замучил вопросами, капитан, лучше бы угостил ромом.")),
		                PCharRepPhrase(LinkRandPhrase("Что вы хотели узнать "+ GetAddress_Form(NPChar) + "?", "Я вас слушаю капитан.", "Мне нужно идти, поэтому спрашивайте скорее, капитан."),
                                        LinkRandPhrase("Всегда рад"+NPCharSexPhrase(NPChar, " ", "а ")+" приятному собеседнику, " + GetAddress_Form(NPChar) + " " + PChar.lastname + ". Говорите.",
                                                        "Да, " + GetAddress_Form(NPChar) + "?",
                                                        "Любите поболтать, капитан? Что ж, я тоже..."))
                            );

            // homo 25/06/06
			link.l1 = LinkRandPhrase ("Какие байки в ходу в здешней таверне?",
                                    "Что новенького в этих краях?",
                                    "Как течет жизнь на суше?");
			link.l1.go = "rumours_citizen";
			link.l2 = LinkRandPhrase("Не объясните мне дорогу?", "Что-то я никак не найду дороги кое-куда...", "Подскажите дорогу...");
			link.l2.go = "town";  //(перессылка в файл города)
			if(Rand(1)==0)
			{
				link.l3 = LinkRandPhrase("Хочу узнать, что тут говорят об одном человеке.", "Не знаете, что говорят тут об одной персоне?", "Хотелось бы узнать кое-что о местных жителях.");
				link.l3.go = "info"; //(перессылка в файл города)
			}
			else
			{
				link.l3 = "Расскажите мне о вашей колонии.";
				link.l3.go = "colony";
			}
            link.l4 = RandPhraseSimple("Есть важное дело!", "У меня к вам дело.");
			link.l4.go = "quests";  //(перессылка в файл города)
			link.l5 = PCharRepPhrase(RandPhraseSimple("Тебе послышалось, я пойду.", "Нет, ничего - просто шел в таверну."),
                                        RandPhraseSimple("Ладно, ничего. Удачи!", "Просто осматриваю город. До свидания."));
			link.l5.go = "exit";
		break;
		
		case "colony":
			dialog.text = LinkRandPhrase("Честно говоря, я не слишком-то разбираюсь в этом. Но кое-что могу рассказать.",
                           "Что же вас интересует в нашей колонии?", "Всегда пожалуйста. О чем вы хотите услышать?");
            switch(Rand(1))
			{
				case 0:
					link.l1 = "Что это за город?";
					link.l1.go = "colony_town";
				break;

				case 1:
					link.l1 = "Что вы знаете о форте, защищающем город?";
					link.l1.go = "fort";
				break;
            }
			link.l2 = "Поговорим о чем-нибудь другом.";
			link.l2.go = "new question";
		break;
	  // to_do
		case "colony_town":
            dialog.text = "О, да вы издеваетесь? Или шутите? Посмотрите в правый угол экрана.";
			link.l1 = "Точно, туплю.";
		    link.l1.go = "exit";
		break;
		
		case "fort":
            dialog.text = "Ничего про это не знаю.";
			link.l1 = "Жаль.";
		    link.l1.go = "exit";
		break;
		//============================ Генератор мэра. Квест по розыску шпиона на улице ============================
		case "SitySpy":
			dialog.text = NPCStringReactionRepeat(NPCharSexPhrase(npchar, "Ну, а я здесь при чем? Я местный житель!", "Ну, а я то здесь при чем? Я живу в этом городе, и знать ничего такого не желаю!"), 
				NPCharSexPhrase(npchar, "Что опять вам от меня нужно? Говорил вам уже, что я не шпион!", "Я же говорила вам уже, что я местная!"), 
				NPCharSexPhrase(npchar, "Ну, сколько можно? Отстаньте от меня, я местный житель!", "Ну что вы опять ко мне пристаете со своими дурацкими расспросами?"), 
				NPCharSexPhrase(npchar, "Что, опять?! Ну это ни в какие ворота не лезет!!!", "Послушайте, отстаньте от меня наконец, прошу вас!!"), "block", 0, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat("Кто может подтвердить твои слова?", "Ой, извините, я вас уже опрашивал...",
                      "Простите, столько народа...", "Хорошо, хорошо...", npchar, Dialog.CurrentNode);
		    link.l1.go = DialogGoNodeRepeat("SitySpy_1", "exit", "exit", "exit", npchar, Dialog.CurrentNode);
			// ==> выбираем подтверждателя
			int num = FindNearCharacters(npchar, 20.0, -1.0, -1.0, 1.0, false, true);
			if(num > 0)
			{
				for(int i = 0; i < num; i++)
				{
					iTemp = chrFindNearCharacters[i].index;
					if (CheckAttribute(&characters[iTemp], "CityType"))
					{
						if(characters[iTemp].CityType == "soldier" || characters[iTemp].CityType == "citizen")
						{
							break;
						}
					}
				}
				pchar.GenQuest.SeekSpy.Idx = chrFindNearCharacters[i].index; //индекс выбранного
				pchar.GenQuest.SeekSpy.Type = characters[sti(pchar.GenQuest.SeekSpy.Idx)].chr_ai.type; //тип кто помог
				pchar.GenQuest.SeekSpy.BaseIdx = npchar.index; //индекс
			}
		break;
		case "SitySpy_1":
			if (sti(pchar.GenQuest.SeekSpy.Idx) == -1 || sti(pchar.GenQuest.SeekSpy.Idx) == 1) //на всякий случай
			{
				dialog.text = "Да кто угодно! Послушайте, что вы меня достаете?! Если у вас какие-то проблемы, то пройдемте к губернатору, и все проясним!",
				link.l1 = "Гм... пожалуй, не будем его тревожить зря. Я вам верю...";
				link.l1.go = "exit";
				break;
			}
			dialog.text = "Кто угодно в этом городе! " + NPCharSexPhrase(npchar, GetFullName(&characters[sti(pchar.GenQuest.SeekSpy.Idx)]) + " может без проблем, благо " + NPCharSexPhrase(&characters[sti(pchar.GenQuest.SeekSpy.Idx)], "он", "она") + " в двух шагах.",
				"Ой, Господи, да вот, хотя бы " + NPCharSexPhrase(&characters[sti(pchar.GenQuest.SeekSpy.Idx)], GetAddress_Form(NPChar) + " " + characters[sti(pchar.GenQuest.SeekSpy.Idx)].lastname, GetFullName(&characters[sti(pchar.GenQuest.SeekSpy.Idx)])) + " может.");				
			link.l1 = "Что ж, давайте послушаем...";
		    link.l1.go = "SitySpy_2";
			sld = &characters[sti(pchar.GenQuest.SeekSpy.Idx)];
			sld.dialog.currentnode = "SeekSpy_Checking";
			pchar.GenQuest.SeekSpy.time = 4.0; //время на подбежать
			if (sld.chr_ai.type == "merchant") pchar.GenQuest.SeekSpy.time = 0.0; //мерчант базарит сразу
            LAi_SetActorTypeNoGroup(sld);
		break;
		case "SitySpy_2":
			sld = &characters[sti(pchar.GenQuest.SeekSpy.Idx)];
			LAi_SetActorTypeNoGroup(npchar);
			LAi_ActorTurnToCharacter(npchar, sld);
			LAi_ActorDialog(sld, pchar, "", stf(pchar.GenQuest.SeekSpy.time), 0); 
			DialogExit();
		break;
		//=================== ноды квеста мэра. поиск шпиона на улице, опрос свидетеля ==================
		case "SeekSpy_Checking":
			dialog.text = "Да, я подтверждаю, " + NPCharSexPhrase(&characters[sti(pchar.GenQuest.SeekSpy.BaseIdx)], "он действительно местный житель.", "она действительно местная жительница.");
			link.l1 = RandPhraseSimple("Я понял. Спасибо за помощь.", "Все ясно. Спасибо за помощь.");
			link.l1.go = "SeekSpy_Checking_1";
		break;
		case "SeekSpy_Checking_1":
			switch (pchar.GenQuest.SeekSpy.Type)
			{
				case "guardian": LAi_SetGuardianTypeNoGroup(npchar); break;
				case "patrol":   LAi_SetPatrolTypeNoGroup(npchar);   break;
				case "citizen":  LAi_SetCitizenTypeNoGroup(npchar);  break;
				case "merchant": LAi_SetMerchantTypeNoGroup(npchar); break;
			}
			LAi_SetCitizenTypeNoGroup(&characters[sti(pchar.GenQuest.SeekSpy.BaseIdx)]);
			NextDiag.CurrentNode = "First Time";
			DialogExit();
		break;
	}
}
