
void ProcessDialogEvent()
{
	ref NPChar;
	aref Link, NextDiag;

	DeleteAttribute(&Dialog,"Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

	int iTemp, i;

	string NodeName = Dialog.CurrentNode;
	
	switch(Dialog.CurrentNode)
	{
		case "First time":
			dialog.text = "Что ты хотел? Не задерживай меня по пустякам, я занят.";
			link.l1 = "Извини, так просто заскочил к тебе.";
			link.l1.go = "exit";
			NextDiag.TempNode = "First time";
			if (npchar.quest.meeting == "0")
			{
				npchar.quest.meeting = "1";
				pchar.questTemp.LSC = "AdmiralFoundHero";
				if (CheckAttribute(loadedLocation, "storm"))
				{
					dialog.text = "А-а-а, вот и ты. Это хорошо, что ты сам пришел ко мне. Не люблю, когда от меня бегают, да еще в такой шторм...";
					link.l1 = "Ни от кого бегать не намерен. Меня зовут " + GetFullName(pchar) + ".";
					link.l1.go = "FT_1";
					PChar.quest.LSC_admiralOwnFind.over = "yes";
				}
				else
				{
					dialog.text = "А-а-а, вот и ты. Я ждал, когда ты явишься ко мне.";
					link.l1 = "Приветствую. Меня зовут " + GetFullName(pchar) + ".";
					link.l1.go = "FT_1";
				}
			}
			//квестовые ноды
			if (pchar.questTemp.LSC == "toAdmNarvalRing" && CheckCharacterItem(pchar, "DOjeronRing"))
			{
				link.l4 = "Адмирал, у меня есть для тебя важная информация. Гм, как мне кажется...";
				link.l4.go = "NarvalRing_1";
			}
			if (pchar.questTemp.LSC == "NarvalDestroyed")
			{
				link.l4 = "Я все сделал, клан 'Нарвал' уничтожен. Вырезал там всех под корень, никто не ушел!";
				link.l4.go = "DestrNarval_1";
			}
		break;

		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;

		case "FT_1":
			dialog.text = "Очень хорошо, приятель, просто замечательно... А меня зовут " + GetFullName(npchar) + ", я адмирал этого Города Потерянных Кораблей!"; 
			link.l1 = "Ха, а ведь слышал о тебе! Ты тот самый пиратский капитан, что вышел за призом на 'Ужасе Мейна' и пропал. Так вот куда ты задевался!";
			link.l1.go = "FT_1_1";
		break;
		case "FT_1_1":
			dialog.text = "Да, обо мне еще помнят в Карибском море... Видишь, как получилось. Два дня мы гнались за испанским галеоном, а в ночь на третьи сутки оказались здесь... Дьявол, вся моя команда пошла код дну, выбраться смогли только я, Уоркман и Форе.";
			link.l1 = "Сожалею...";
			link.l1.go = "FT_1_2";
		break;
		case "FT_1_2":
			dialog.text = "По правде сказать, дермо была команда тогда. Так что, можно и не горевать особо, тем более, что дело прошлое... Ну, сейчас мы не об этом!";
			link.l1 = "Говори, что нужно. Я внимательно слушаю, " + npchar.name + ".";
			link.l1.go = "FT_2";
		break;
		case "FT_2":
			dialog.text = "Значит так! С того момента, как ты влез с моря на палубу нашего острова, ты стал гражданином Города, и теперь обязан подчинятся его Закону. Это тебе понятно?";
			link.l1 = "Вполне. И что за Закон, как с ним можно ознакомиться?";
			link.l1.go = "FT_3";
		break;
		case "FT_3":
			dialog.text = "С Законом знакомлю тебя я в данный конкретный момент. На скрижалях он не выбит, так как с камнем у нас напряженка, ха-ха... А Закон по сути своей заключается в следующих пунктах\n"+
				"Первое: все граждане Города Потерянных Кораблей имеют право на жизнь. Это право обеспечиват гарнизон милиции, который находится в распоряжении адмирала, то есть в моем распоряжении... Кстати, а ты не хочешь записаться в милицию?";
			link.l1 = "Там посмотрим. Сначала нужно обжиться, как-то устроится здесь, а потом уже решать такие вопросы.";
			link.l1.go = "FT_4";
		break;
		case "FT_4":
			dialog.text = "Ты по виду парень неслабый. Кем был в Большом мире?";
			link.l1 = "Да так, знаешь ли, странствовал... Был свободным капитаном, таким же, как и ты, в общем.";
			link.l1.go = "FT_5";
		break;
		case "FT_5":
			dialog.text = "Понятно... Но сейчас, собственно, мне до этого дела нет. Будь ты до попадания сюда испанцем или французом, ангелом или чертом лысым - здесь все равны. Теперь ты стал гражданином Города, со своими правами, важнейшим из которых является право на жизнь! Хех, как завернул!..";
			link.l1 = "Да, неплохо. Всеобщая справедливость. Кстати, в Карибском море ходят слухи о каком-то Острове Справедливости. Полагаю, что это и есть ваш Город.";
			link.l1.go = "FT_6";
		break;
		case "FT_6":
			dialog.text = "Нет, это не может быть. Путь отсюда заказан... Ну, продолжим. Итак, с правами все, теперь о твоих обязанностях. Второе: ты не можешь покушаться на жизнь граждан Города. Если ты убьешь кого-то, то тебя убью я. Это ясно?";
			link.l1 = "Хм, ясно...";
			link.l1.go = "FT_7";
		break;
		case "FT_7":
			dialog.text = "Небольшой нюанс. У нас тут в Городе есть два сообщества, так называемые кланы 'Нарвал' и 'Каспер'. В общем, там собрались люди, которым жизнь добропорядочного человека - что кость в горле. Мы им выделили два корабля под постоянное место дислокации - это барк 'Сан Габриэль' и галеон 'Веласко'. Корабли эти стоят на отшибе, поэтому кланы жизни Города не мешают\n"+
				"Так вот, заходить внурь этих кораблей гражданам не стоит - опасно для жизни. Внутренние помещения данных судов переданы в собственность кланов, и на этой территории у них действуют свои законы. Поэтому не лезь туда или пеняй на себя. Понятно?";
			link.l1 = "Понятно.";
			link.l1.go = "FT_7_1";
		break;
		case "FT_7_1":
			dialog.text = "Пойдем дальше. Третье: запрещается строить корабли, плоты и прочие плавсредства. Выбраться отсюда все равно невозможно, а разрушать Город нельзя.";
			link.l1 = "И это понятно.";
			link.l1.go = "FT_7_2";
		break;
		case "FT_7_2":
			dialog.text = "Четвертое: за воровство в Городе полагается смертная казнь. Если тебя застукают копающемся в чужом сундуке - ты автоматически лишаешься гражданства и становишься вне Закона. А это значит, что права на жизнь у тебя больше нет...";
			link.l1 = "Звучит заманчиво... Ну что же, это ясно. Что дальше?";
			link.l1.go = "FT_8";
		break;
		case "FT_8":
			dialog.text = "Ну и пятое: все материальные средства, которые попали в Город, принадележат Городу.";
			link.l1 = "Хм, а вот по этому пункту подробней, пожалуйста.";
			link.l1.go = "FT_9";
		break;
		case "FT_9":
			dialog.text = "Хех, ну что ж тут непонятного? Вот, к примеру, ты, как физическое лицо, попал в Город. И заметь, сразу стал его гражданином!\nА вот твое имущество, как материальные средства, становятся собственностью Города. Так что все твое теперь наше общее!";
			link.l1 = "Погодите!! Я с таким раскладом не согласен!";
			link.l1.go = "FT_10";
		break;
		case "FT_10":
			if (pchar.questTemp.LSC == "AdmiralFoundHero")
			{
				dialog.text = "Вот чудак-человек! Твоего согласия не требуется, мы просто забираем, и все... Но у меня есть и хорошая новость. Тебе, как полноправному гражданину Города, полагается 1/50 часть!";
				link.l1 = "От моего же имущества!!";
				link.l1.go = "FT_11";
				pchar.money = makeint(sti(pchar.money) / 50);
			}
			else
			{
				dialog.text = "А твоего согласия никто не спрашивает. Вообще-то Закон обязывает включать бывшего хозяина добра в число дольщиков при разделе. Но так как ты нарушил Закон, а именно не явился ко мне вовремя по прибытии в Город, данный пункт Закона в отношении тебя утрачивает свою силу. То есть тебе не достается ничего.";
				link.l1 = "Мне все это очень сильно не нравится!";
				link.l1.go = "FT_12";
				pchar.money = 0;
			}
		break;
		case "FT_11":
			dialog.text = "Именно! Ну вот, ты начинаешь понимать... Вообще-то, тебе все одно это барахло тут ни к чему, а нам всем приятно будет...";
			link.l1 = "Мне все это очень сильно не нравится!";
			link.l1.go = "FT_12";
		break;
		case "FT_12":
			dialog.text = "Не ты первый, приятель, кому это не нравится. Но только, уж поверь мне, все это в твоих интересах. Чем быстрей ты избавишься всего того, что тебя связывает с большим миром, тем быстрей ты вольешься в нашу размеренную жизнь.\nА так можешь сойти с ума, такое бывает. И, скажу я тебе, с такими мы не церемонимся - груз на шею, и на дно!";
			link.l1 = "Да уж, справедливость так и прет...";
			link.l1.go = "FT_13";
		break;
		case "FT_13":
			dialog.text = "За хамство я лишаю тебя оружия, да и вообще карманы твои почищу! И имей ввиду, что в следующий раз, если ты мне не понравишься, я тебя просто прикончу.";
			link.l1 = "М-да... Ну что же, придется смириться. Похоже, у меня нет другого выхода.";
			link.l1.go = "FT_14";
		break;
		case "FT_14":
			dialog.text = "Это верно. И не советую тебе его искать, это запрещено Законом!";
			link.l1 = "Я помню...";
			link.l1.go = "FT_15";
		break;
		case "FT_15":
			dialog.text = "Ну вот и отлично... А теперь ты свободен и теперь можешь перемещаться по Городу как угодно. Делай, что хочешь, наслаждайся жизнью в Городе.";
			link.l1 = "Так и поступлю, Чад.";
			link.l1.go = "FT_16";
		break;
		case "FT_16":
			chrDisableReloadToLocation = false;
			//bDisableFastReload = false; //откроем фастрелоады
			//откроем кланы
			ref rLoc = &locations[FindLocation("LostShipsCity_town")];
			DeleteAttribute(rLoc, "reload.l61.disable");
			DeleteAttribute(rLoc, "reload.l70.disable");
			DeleteAttribute(rLoc, "reload.l72.disable");
			pchar.questTemp.LSC = "AfterAdmiral";
			RemoveCharacterEquip(pchar, BLADE_ITEM_TYPE);
			RemoveCharacterEquip(pchar, GUN_ITEM_TYPE);
			RemoveCharacterEquip(pchar, SPYGLASS_ITEM_TYPE);
			RemoveCharacterEquip(pchar, PATENT_ITEM_TYPE);
			RemoveCharacterEquip(pchar, CIRASS_ITEM_TYPE);
			RemoveCharacterEquip(pchar, MAPS_ITEM_TYPE);	
			//сносим все предметы, кроме квестовых -->
            aref arItems;
			string sName;
    		makearef(arItems, pchar.items);
    		int	Qty = GetAttributesNum(arItems);
    		for (int a=0; a<Qty; a++)
    		{
                sName = GetAttributeName(GetAttributeN(arItems, a));
				rLoc = ItemsFromID(sName);
				if (rLoc.ItemType == "QUESTITEMS")
				{
					pchar.questTemp.items = sName;
				}
    		}
			DeleteAttribute(pchar, "items");
			pchar.items = "";
			makearef(arItems, pchar.questTemp.items);
    		Qty = GetAttributesNum(arItems);
    		for (a=0; a<Qty; a++)
    		{
                sName = GetAttributeName(GetAttributeN(arItems, a));
				GiveItem2Character(pchar, sName);
    		}
			DeleteAttribute(pchar, "questTemp.items");
			//<-- сносим все предметы, кроме квестовых	
			AddQuestRecord("ISS_MainLine", "3");
			if (npchar.chr_ai.type == LAI_TYPE_ACTOR) LAi_SetLSCoutTypeNoGroup(npchar);
			NextDiag.TempNode = "First time";
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;

		//базар с ГГ, если сам не пришел к адмиралу
		case "FoundHero":
			npchar.quest.meeting = "1";
			NextDiag.TempNode = "First time";
			pchar.questTemp.LSC = "AdmiralNotFoundHero";
			dialog.text = "Ага, вот значит и ты, выживший в последнем шторме. Скажи-ка мне, приятель, какого это черта я должен искать тебя по всему Городу? Ты что, не знаешь наших порядков?";
			link.l1 = "Приветствую. Каких порядков?";
			link.l1.go = "FoundHero_1";
		break;
		case "FoundHero_1":
			dialog.text = "Хочешь сказать, что ты не знал о том, что ко мне нужно явится сразу же, как только оказался в Городе?";
			link.l1 = "Не знал...";
			link.l1.go = "FoundHero_2";
		break;
		case "FoundHero_2":
			dialog.text = "Незнание Закона не освобождает от ответственности! А теперь говори, кто ты.";
			link.l1 = "Я прошу прощения за опоздание, я еще не изучил Законов Города. Надеюсь, что дальше будет лучше... Меня зовут " + GetFullName(pchar) + ".";
			link.l1.go = "FT_1";
		break;

		//ГГ принес кольцо нарвалов
		case "NarvalRing_1":
			dialog.text = "Говори.";
			link.l1 = "Я тут недавно забрел в каюту корвета 'Протектор', и совершенно случайно нашел там интересный предмет...";
			link.l1.go = "NarvalRing_2";
		break;
		case "NarvalRing_2":
			dialog.text = "Что за предмет? Говори быстрей, не тяни!";
			link.l1 = "Это кольцо. Вот оно, посмотри.";
			link.l1.go = "NarvalRing_3";
		break;
		case "NarvalRing_3":
			dialog.text = "Хо-хо, да это же кольцо 'нарвала'! Так, где ты его нашел, говоришь?";
			link.l1 = "В каюте 'Протектора'.";
			link.l1.go = "NarvalRing_4";
			TakeItemFromCharacter(pchar, "DOjeronRing");
		break;
		case "NarvalRing_3":
			dialog.text = "А тебе известно, что там было совершено убийство миллиционера и плотника Андрэ Лабора?";
			link.l1 = "Слышал...";
			link.l1.go = "NarvalRing_4";
		break;
		case "NarvalRing_4":
			dialog.text = "М-да, твоя находка ясно показывает, кто это сделал... А ты знаком с 'нарвалами'?";
			link.l1 = "Нет, Чад. Они же к себе никого не пускают. Чуть что - сразу нападают без разговоров.";
			link.l1.go = "NarvalRing_5";
		break;
		case "NarvalRing_5":
			dialog.text = "Да уж, это правда. На своей территории они могут делать, что хотят. Там у них свои законы. А ты знаешь, как получилось, что образовались кланы?";
			link.l1 = "Расскажи, пожалуйста. С удовольствием послушаю.";
			link.l1.go = "NarvalRing_6";
		break;
		case "NarvalRing_6":
			dialog.text = "Хм, хорошо... Дело в том, что наряду с обычными людим, в Город попадает всякое отребье. Жить с остальными они не могут в мире, поэтому и было принято решение отдать таким людям два корабля в собственность. К тому времени образовалось две группировки головорезов, они и образовали кланы 'Нарвал' и 'Каспер'.";
			link.l1 = "Понятно... А что теперь делать? Ведь получается, что 'нарвалы' причастны к убийству официального представителя власти!";
			link.l1.go = "NarvalRing_7";
		break;
		case "NarvalRing_7":
			dialog.text = "Пора избавиться от клана 'Нарвал', утомили они своей отмороженностью.";
			link.l1 = "Правильное решение, адмирал!";
			link.l1.go = "NarvalRing_8";
		break;
		case "NarvalRing_8":
			dialog.text = "Без сомнения! И я поручаю это тебе.";
			link.l1 = "Хм, что поручаешь?";
			link.l1.go = "NarvalRing_9";
		break;
		case "NarvalRing_9":
			dialog.text = "Уничтожение 'нарвалов', приятель. Кому же, как не тебе, выполнить это дело? Ведь это благодаря тебе мы узнали правду!";
			link.l1 = "Черт! Я как-то на такой поворот сюжета не рассчитывал...";
			link.l1.go = "NarvalRing_10";
		break;
		case "NarvalRing_10":
			dialog.text = "Никак сдрейфил? Ты же пиратский кэп, что что с тобой?!";
			link.l1 = "Их много, просто. А я хоть и был пираским капитаном, но не самоубийца.";
			link.l1.go = "NarvalRing_11";
		break;
		case "NarvalRing_11":
			dialog.text = "А-а, ну это мы решим. Я даю тебе троих моих людей в помощь.";
			link.l1 = "Ну, это меняет дело. Я согласен.";
			link.l1.go = "NarvalRing_12";
		break;
		case "NarvalRing_12":
			dialog.text = "Отлично! Так отправляйся на 'Веласко' прямо сейчас и поработай там как следует...";
			link.l1 = "Хорошо, Чад, все сделаю в лучшем виде.";
			link.l1.go = "NarvalRing_13";
		break;
		case "NarvalRing_13":
			AddQuestRecord("ISS_MainLine", "12");
			pchar.questTemp.LSC = "toDestoyAllNarval";
			LocatorReloadEnterDisable("LostShipsCity_town", "reload54", true); //закроем касперов, на всякий случай
			int iQty = GetOfficersQuantity(pchar);
			int iNum = 1;
			int idxMent;
			while(iQty < 4)
			{
				idxMent = GetCharacterIndex("Ment_" + iNum);
				if (idxMent != -1)
				{
					//ChangeCharacterAddressGroup(sld, pchar.location, "reload", "reload5");
					SetOfficersIndex(pchar, -1, idxMent);
				}
				iNum++;
				iQty = GetOfficersQuantity(pchar);
				if (iNum > 6) break;
			}
			pchar.questTemp.LSC.qtyOfficers = iQty; //запомним кол-во офицеров, для понта
			LAi_group_SetCheck("EnemyFight", "NarvalDestroyed");
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;
		//клан Нарвал уничтожен
		case "DestrNarval_1":
			dialog.text = "Ну что же, это хорошо. А как там мои люди?";
			iTemp = GetPassengersQuantity(pchar);
			if (iTemp == 0)
			{
				link.l1 = "Все погибли, к сожалению. Мне очень жаль...";
				link.l1.go = "DestrNarval_Bad";
			}
			else
			{
				if (iTemp == sti(pchar.questTemp.LSC.qtyOfficers))
				{
					link.l1 = "Да все нормально. Все живы и здоровы, возвращаю их тебе в целости и сохранности.";
					link.l1.go = "DestrNarval_Ok";
				}
				else
				{
					link.l1 = "Потери имеются, но погибли не все. Оставшихся в живых возвращаю.";
					link.l1.go = "DestrNarval_BadOk";
				}
			}
		break;
		case "DestrNarval_Bad":
			dialog.text = "М-да, печально... Однако, задача выполнена, 'нарвалы' уничтожены, а это главное. Так что все в порядке.";
			link.l1 = "Ну да, все путем, как говорится...";
			link.l1.go = "DestrNarval_Bad1";
		break;
		case "DestrNarval_Bad1":
			dialog.text = "Ну что же, отлично. Я тебя больше не задерживаю, ты можешь идти.";
			link.l1 = "Хм... Ну, спасибо, адмирал...";
			link.l1.go = "exit";
			pchar.questTemp.LSC = "toTavernNarvalDestroyed";
			AddQuestRecord("ISS_MainLine", "16");
			for (i=0; i<=PASSENGERS_MAX; i++)
			{
				iTemp = GetPassenger(pchar, i);
				if(iTemp != -1)
				{
					sld = &characters[iTemp];
					RemovePassenger(pchar, sld);
					LAi_SetWarriorType(sld);
					LAi_group_MoveCharacter(sld, LAI_GROUP_PLAYER_OWN);
					SaveCurrentNpcQuestDateParam(sld, "location");
					ChangeCharacterAddressGroup(sld, "LostShipsCity_town", "foto",  sld.location.baseLocator);
				}
			}
		break;
		case "DestrNarval_Ok":
			dialog.text = "Отлично! За хорошо сделанную работу, я, пожалуй, подкину тебе деньжат. Сто тысяч золотых! Забирай, они твои.";
			link.l1 = "Хм, мне здесь пока деньги не нужны, но все равно спасибо тебе.";
			link.l1.go = "DestrNarval_Ok1";
			AddQuestRecord("ISS_MainLine", "14");
			AddMoneyToCharacter(pchar, 100000);
		break;
		case "DestrNarval_Ok1":
			dialog.text = "Не за что! Ну, я тебя больше не задерживаю, так что можешь смело заниматься своим делами.";
			link.l1 = "Хм... Хорошо.";
			link.l1.go = "exit";
			pchar.questTemp.LSC = "toTavernNarvalDestroyed";
			for (i=0; i<=PASSENGERS_MAX; i++)
			{
				iTemp = GetPassenger(pchar, i);
				if(iTemp != -1)
				{
					sld = &characters[iTemp];
					RemovePassenger(pchar, sld);
					LAi_SetWarriorType(sld);
					LAi_group_MoveCharacter(sld, LAI_GROUP_PLAYER_OWN);
					SaveCurrentNpcQuestDateParam(sld, "location");
					ChangeCharacterAddressGroup(sld, "LostShipsCity_town", "foto",  sld.location.baseLocator);
				}
			}
		break;
		case "DestrNarval_BadOk":
			dialog.text = "М-да, неприятно несколько... Ну да ладно, все-таки это была резня, а не легкая прогулка по палубе. В общем, я рад, что все кончилось хорошо. Хочу подкинуть тебе деньжат за работу. Десять тысяч золотых!";
			link.l1 = "Ну, деньги здесь особо не нужны... Но все равно спасибо.";
			link.l1.go = "DestrNarval_Ok1";
			AddQuestRecord("ISS_MainLine", "15");
			AddMoneyToCharacter(pchar, 10000);
		break;
	}
}
