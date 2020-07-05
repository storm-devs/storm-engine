#include "scripts\utils.c"

// boal -->
#define CARDS_RULE  "Правила простые. Играем в 'двадцать одно'. В колоде 36 карт, от тузов до шестерок. Туз -11, король - 4, дама - 3, валет - 2, остальные как написано. Нужно набрать 21 очко. \n Раздаем по одной карте, далее набираем себе недостающие (клик по колоде). Ход передается кликом по портрету с профилем в правом верхнем углу окошка. Если перебор, то игра заканчивается. Каждая карта - это наша ставка. Начать игру можно, если у тебя есть деньги хотя бы для трех ставок. Повтор игры клик по колоде, выход - крестик или Esc."
#define DICE_RULE   "Играем в 'покер-джокер'. Пять кубиков у каждого, бросаем по очереди все пять. Можно перебросить любое число кубиков, сделав ставку за каждый, но только один раз. Нужно выкинуть максимальный результат. \n Результаты по возрастанию: одна пара (2), две пары (2+2), триада (3), фул (3+2), каре (4), стрит (кубики подряд) и покер (5). Если одинаково выпало, то победил тот, у кого старше комбинация. \n Управление: клик по стакану - бросок кубиков, клик по кубику - на переброс (деньги на ставку должны быть), клик по портрету - передать ход (когда переброс не нужен)."
#define MAX_TITLENEXTRATE   13 // счетчик для званий
#define MAX_TITLE           5
// boal <--

object Address_Form;

void Set_inDialog_Attributes()
{ 
	ref The_Character_is;

	The_Character_is = GetMainCharacter();
    if (!CheckAttribute(The_Character_is, "sex") || The_Character_is.sex == "man")
	{
		Address_Form.Spa = "сеньор";
		Address_Form.Fra = "месье";
		Address_Form.Eng = "сэр";
		Address_Form.Hol = "минхер";
		Address_Form.Pir = "сэр";
    }
    else
    {
		Address_Form.Spa = "сеньорита";
		Address_Form.Fra = "мадмуазэль";
		Address_Form.Eng = "мисс";
		Address_Form.Hol = "госпожа";
		Address_Form.Pir = "мисс";
	}
	Address_Form.Spa.Title1 = "Корсар";
	Address_Form.Fra.Title1 = "Приватир";
	Address_Form.Eng.Title1 = "Капер";
	Address_Form.Hol.Title1 = "Флибустьер";
	Address_Form.Pir.Title1 = "Пират";

    Address_Form.Spa.Title2 = "Коммандер";
	Address_Form.Fra.Title2 = "Коммандер";
	Address_Form.Eng.Title2 = "Коммандер";
	Address_Form.Hol.Title2 = "Коммандер";
	Address_Form.Pir.Title2 = "Коммандер";
	
	Address_Form.Spa.Title3 = "Капитан";
	Address_Form.Fra.Title3 = "Капитан";
	Address_Form.Eng.Title3 = "Капитан";
	Address_Form.Hol.Title3 = "Капитан";
	Address_Form.Pir.Title3 = "Капитан";

	Address_Form.Spa.Title4 = "Коммодор";
	Address_Form.Fra.Title4 = "Коммодор";
	Address_Form.Eng.Title4 = "Коммодор";
	Address_Form.Hol.Title4 = "Коммодор";
	Address_Form.Pir.Title4 = "Коммодор";

	Address_Form.Spa.Title5 = "Адмирал";
	Address_Form.Fra.Title5 = "Адмирал";
	Address_Form.Eng.Title5 = "Адмирал";
	Address_Form.Hol.Title5 = "Адмирал";
	Address_Form.Pir.Title5 = "Адмирал";

	Address_Form.Spa.woman = "сеньора";
	Address_Form.Fra.woman = "мадам";
	Address_Form.Eng.woman = "миссис";
	Address_Form.Hol.woman = "госпожа";
	Address_Form.Pir.woman = "мисис";

	Address_Form.Spa.man = "сеньор";
	Address_Form.Fra.man = "месье";
	Address_Form.Eng.man = "сэр";
	Address_Form.Hol.man = "минхер";
	Address_Form.Pir.man = "сэр";
	/*switch (The_Character_is.id)
	{	
		case "Blaze":
			
			Address_Form.Spa = GlobalStringConvert("Address_Form_Spa");
			Address_Form.Fra = GlobalStringConvert("Address_Form_Fra");
			Address_Form.Eng = GlobalStringConvert("Address_Form_Eng");
			Address_Form.Por = GlobalStringConvert("Address_Form_Por");
			Address_Form.Hol = GlobalStringConvert("Address_Form_Hol");
			return;
		break;

		case "Beatriss":

			Address_Form.Spa = "Senorita";
			Address_Form.Fra = "Mademoiselle";
			Address_Form.Eng = "Miss";
			Address_Form.Por = "Senhorita";
			Address_Form.Hol = "Juffrouw";
			return;
		break;
	}
	trace("ERROR: Player Character is not defined - can't select address form");
	*/
}

// boal -->

string RandSwear()
{
	switch (rand(8))
	{
		case 0:
			return "Карамба! ";
		break;

		case 1:
			return "Проклятье! ";
		break;

		case 2:
			return "Дьявол! ";
		break;

		case 3:
			return "Черт! ";
		break;

		case 4:
			return "Якорь мне в ..! ";
		break;

		case 5:
			return "Тысяча акул! ";
		break;

		case 6:
			return "Аргкх!!! ";
		break;

		case 7:
			return "Тысяча чертей! ";
		break;	 
		
		case 8:
			return "Алле Хагель! ";	// стандартное пиратское "твою мать"
		break;

	}
}

//navy --> вынес простые восклицания,
//т.к. "О, Боже!!! Я вырежу тебе сердце" звучит мягко говоря странно :)
string RandExclamation()
{
	switch(rand(2))
	{
		case 0:
			return "О, Боже!! ";
		break;

		case 1:
			return "О, Небеса! ";
		break;

		case 2:
			return "Святая Мадонна! ";
		break;
	}
}
//navy <--
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                Выбор фраз для диалога
////////////////////////////////////////////////////////////////////////////////////////////////////////
// boal 13.03.2004 -->
string RandPhraseSimple(string Var1, string Var2);
{
	int RandP;
	RandP = Rand(1);
	switch(RandP)
	{
		case 0:
			return /*"First phrase selected" + */ Var1;
		break;

		case 1:
			return /*"Second phrase selected" + */Var2;
		break;
	}
	return "ERROR";
}
// выбор фразы из трех
string LinkRandPhrase (string Var1, string Var2, string Var3)
{
	int RandP;
	RandP = Rand(2);
	switch(RandP)
	{
		case 0:

			return /*"First phrase selected" + */ Var1;

		break;

		case 1:

			return /*"Second phrase selected" + */Var2;

		break;

		case 2:

			return /*"Third phrase selected" + */Var3;

		break;
	}
	return "ERROR";
}

string NationKingsCrown(ref Character)
{
    switch(sti(Character.nation))
	{
		case ENGLAND:
            return "Английскому престолу";
		break;
		case FRANCE:
            return "Французской короне";
		break;
		case SPAIN:
            return "Испанской короне";
		break;
		case HOLLAND:
			return "Королевству Нидерланды";
		break;
		case PIRATE:
			return "Береговому братству";
		break;
	}
}
string NationKingsName(ref NPChar)
{
    switch(sti(NPChar.nation))
	{
		case ENGLAND:
            return "Его Величества Карла II Стюарта";
		break;
		case FRANCE:
            return "Его Величества Людовика XIV";
		break;
		case SPAIN:
            return "Его Католического Величества Карла II Габсбурга";
		break;
		case HOLLAND:
			return "Его Величества Вильгельма III Оранского";
		break;
		case PIRATE:
			return "берегового пиратского братства";
		break;
	}
}

string NationNameMan(int pNation)
{
    switch(pNation)
	{
		case ENGLAND:
            return "англичанин";
		break;
		case FRANCE:
            return "француз";
		break;
		case SPAIN:
            return "испанец";
		break;
		case HOLLAND:
			return "голландец";
		break;
		case PIRATE:
			return "пират";
		break;
	}
}
// boal 13.03.2004 <--
string NationNamePeople(int pNation)
{
    switch(pNation)
	{
		case ENGLAND:
            return "англичане";
		break;
		case FRANCE:
            return "французы";
		break;
		case SPAIN:
            return "испанцы";
		break;
		case HOLLAND:
			return "голландцы";
		break;
		case PIRATE:
			return "пираты";
		break;
	}
}

string NationNamePeopleAcc(int pNation)
{
    switch(pNation)
	{
		case ENGLAND:
            return "англичанами";
		break;
		case FRANCE:
            return "французами";
		break;
		case SPAIN:
            return "испанцами";
		break;
		case HOLLAND:
			return "голландцами";
		break;
		case PIRATE:
			return "паратами";
		break;
	}
}

string NationNameAblative(int iNation) //творительный падеж
{
    switch(iNation)
	{
		case ENGLAND:
            return "Англией";
		break;
		case FRANCE:
            return "Францией";
		break;
		case SPAIN:
            return "Испанией";
		break;
		case HOLLAND:
			return "Голландией";
		break;
		case PIRATE:
			return "пиратами";
		break;
	}
}

string NationNameNominative(int iNation) //именительный падеж
{
    switch(iNation)
	{
		case ENGLAND:
            return "Англия";
		break;
		case FRANCE:
            return "Франция";
		break;
		case SPAIN:
            return "Испания";
		break;
		case HOLLAND:
			return "Голландия";
		break;
		case PIRATE:
			return "Береговое братство";
		break;
	}
}

string NationNameGenitive(int iNation) // родительный падеж
{
    switch(iNation)
	{
		case ENGLAND:
            return "Англии";
		break;
		case FRANCE:
            return "Франции";
		break;
		case SPAIN:
            return "Испании";
		break;
		case HOLLAND:
			return "Голландии";
		break;
		case PIRATE:
			return "пиратов";
		break;
	}
}

///////Часто будет использоваться поэтому заношу сюда
string NationEuropaTown(int iNation)
{
    switch(iNation)
	{
		case ENGLAND:
            return "Лондон";
		break;
		case FRANCE:
            return "Ля Рошель";
		break;
		case SPAIN:
            return "Севилью";
		break;
        case PIRATE:
	    	return "Тортугу";
		break;
		case HOLLAND:
	    	return "Амстердам";
		break;
	}
}
string GetCityName(string city) // имена городов по аттрибуту char.city
{
    string ret;
    int nFile = LanguageOpenFile("LocLables.txt");
    
	ret = LanguageConvertString(nFile, city + " Town")
	LanguageCloseFile( nFile );
	
	return  ret;
}

string GetPortByCityName(string city) // имена портов по городов по аттрибуту char.city
{
    switch(city)
	{
		case "Bridgetown":
            return "Bridgetown_town";
		break;
		case "SanJuan":
            return "SanJuan_town";
		break;
		case "Santiago":
            return "Santiago_town";
		break;
		case "PuertoPrincipe":
            return "PuertoPrincipe_port";
		break;
		case "SentJons":
            return "SentJons_town";
		break;
		case "PortRoyal":
            return "PortRoyal_town";
		break;
		case "FortOrange":
            return "Shore35";
		break;
		case "Villemstad":
            return "Villemstad_town";
		break;
		case "BasTer":
            return "BasTer_town";
		break;
		case "LaVega":
            return "LaVega_port";
		break;
		case "Bridgetown":
            return "Bridgetown_town";
		break;
		case "SantoDomingo":
            return "SantoDomingo_town";
		break;
		case "PortPax":
            return "PortPax_town";
		break;
		case "Pirates":
            return "Pirates_town";
        break;
		case "FortFrance":
            return "FortFrance_town";
		break;
		case "LeFransua":
            return "LeFransua_port";
		break;
		case "Havana":
            return "Havana_town";
		break;
		case "Charles":
            return "Charles_town";
		break;
		case "PortoBello":
            return "PortoBello_town";
		break;
		case "PortSpein":
            return "PortSpein_town";
		break;
		case "Tortuga":
            return "Tortuga_town";
		break;
		case "Marigo":
            return "Marigo_town";
		break;
		case "Panama":
            return "Panama_town";
		break;
		case "Cartahena":
            return "Cartahena_town";
		break;
		case "Maracaibo":
            return "Maracaibo_town";
		break;
		case "Caracas":
            return "Caracas_town";
		break;
		case "Cumana":
            return "Cumana_town";
		break;
		case "SantaCatalina":
            return "SantaCatalina_town";
		break;
		case "Beliz":
            return "Beliz_town";
		break;
	}
	return "";
}


string GetIslandByCityName(string city) // имена острова по городу по аттрибуту char.city
{
    switch(city)
	{
		case "Bridgetown":
            return "Barbados";
		break;
		case "SanJuan":
            return "PuertoRico";
		break;
		case "Santiago":
            return "Cuba";
		break;
		case "PuertoPrincipe":
            return "Cuba";
		break;
		case "SentJons":
            return "Antigua";
		break;
		case "PortRoyal":
            return "Jamaica";
		break;
		case "FortOrange":
            return "Jamaica";
		break;
		case "Villemstad":
            return "Curacao";
		break;
		case "BasTer":
            return "Guadeloupe";
		break;
		case "LaVega":
            return "Hispaniola";
		break;
		case "SantoDomingo":
            return "Hispaniola";
		break;
		case "PortPax":
            return "Hispaniola";
		break;
		case "Pirates":
            return "Bermudes";
        break;
		case "FortFrance":
            return "Martinique";
		break;
		case "LeFransua":
            return "Martinique";
		break;
		case "Havana":
            return "Cuba";
		break;
		case "Charles":
            return "Nevis";
		break;
		case "PortoBello":
            return "Mein";
		break;
		case "PortSpein":
            return "Trinidad";
		break;
		case "Tortuga":
            return "Tortuga";
		break;
		case "Marigo":
            return "SentMartin";
		break;
		case "Panama":
            return "Mein";
		break;
		case "Cartahena":
            return "Mein";
		break;
		case "Maracaibo":
            return "Mein";
		break;
		case "Caracas":
            return "Mein";
		break;
		case "Cumana":
            return "Mein";
		break;
		case "SantaCatalina":
            return "Mein";
		break;
		case "Beliz":
            return "Mein";
		break;
		case "Tenotchitlan":
            return "Mein";
		break;
	}
	return "";
}

string GetArealByCityName(string city) // ареал по городу по аттрибуту char.city
{
    switch(city)
	{
		case "Bridgetown":
            return "Barbados";
		break;
		case "SanJuan":
            return "PuertoRico";
		break;
		case "Santiago":
            return "Cuba1";
		break;
		case "PuertoPrincipe":
            return "Cuba1";
		break;
		case "SentJons":
            return "Antigua";
		break;
		case "PortRoyal":
            return "Jamaica";
		break;
		case "FortOrange":
            return "Jamaica";
		break;
		case "Villemstad":
            return "Curacao";
		break;
		case "BasTer":
            return "Guadeloupe";
		break;
		case "LaVega":
            return "Hispaniola1";
		break;
		case "SantoDomingo":
            return "Hispaniola1";
		break;
		case "PortPax":
            return "Hispaniola2";
		break;
		case "Pirates":
            return "Bermudes";
        break;
		case "FortFrance":
            return "Martinique";
		break;
		case "LeFransua":
            return "Martinique";
		break;
		case "Havana":
            return "Cuba2";
		break;
		case "Charles":
            return "Nevis";
		break;
		case "PortoBello":
            return "PortoBello";
		break;
		case "PortSpein":
            return "Trinidad";
		break;
		case "Tortuga":
            return "Tortuga";
		break;
		case "Marigo":
            return "SentMartin";
		break;
		case "Panama":
            return "Panama";
		break;
		case "Cartahena":
            return "Cartahena";
		break;
		case "Maracaibo":
            return "Maracaibo";
		break;
		case "Caracas":
            return "Caracas";
		break;
		case "Cumana":
            return "Cumana";
		break;
		case "SantaCatalina":
            return "SantaCatalina";
		break;
		case "Beliz":
            return "Beliz";
		break;
		case "Tenotchitlan":
            return "Tenotchitlan";
		break;
	}
	return "";
}

string GiveArealByLocation(ref location)
{
	string sAreal;
	if (CheckAttribute(location, "MustSetReloadBack")) return "none";
	if (CheckAttribute(location, "fastreload")) 
	{
		sAreal = GetArealByCityName(location.fastreload); 
		return sAreal;
	}
	if (CheckAttribute(location, "parent_colony")) 
	{
		sAreal = GetArealByCityName(location.parent_colony); 
		return sAreal;
	}
	if (!CheckAttribute(location, "islandId")) return "none";
	if (location.islandId == "Cuba" || location.islandId == "Hispaniola" || location.islandId == "Mein") 
	{			
		if (!CheckAttribute(location, "islandIdAreal")) return "none";
		sAreal = location.islandIdAreal;
	}
	else sAreal = location.islandId;
	return sAreal;
}

// выбор куда идти  по острову даем город (главный город острова с портом)
string GetCityNameByIsland(string CurIslandId)
{
	string TargetLocation;

	TargetLocation = "None";

	switch (CurIslandId)
	{
    	case "Antigua":
			TargetLocation = "SentJons";
		break;
		case "Bermudes":
			TargetLocation = "Pirates";
		break;
		case "Barbados":
			TargetLocation = "Bridgetown";
		break;
		case "Beliz":
			TargetLocation = "Beliz";
		break;
		case "Cuba1":
			TargetLocation = "Santiago";
		break;
		case "Cuba2":
			TargetLocation = "Havana";
		break;
		case "Cumana":
			TargetLocation = "Cumana";
		break;
		case "Caracas":
			TargetLocation = "Caracas";
		break;
		case "Curacao":
			TargetLocation = "Villemstad";
		break;
		case "Cartahena":
			TargetLocation = "Cartahena";
		break;
		case "Guadeloupe":
			TargetLocation = "BasTer";
		break;
		case "Hispaniola1":
			TargetLocation = "SantoDomingo";
		break;
		case "Hispaniola2":
			TargetLocation = "PortPax";
		break;
		case "Jamaica":
			TargetLocation = "PortRoyal";
		break;
		case "Maracaibo":
			TargetLocation = "Maracaibo";
		break;
		case "Martinique":
			TargetLocation = "FortFrance";
		break;
		case "SentMartin":
			TargetLocation = "Marigo";
		break;
		case "Nevis":
			TargetLocation = "Charles";
		break;
		case "PuertoRico":
			TargetLocation = "SanJuan";
		break;
		case "PortoBello":
			TargetLocation = "PortoBello";
		break;
		case "Pearl":
			TargetLocation = "none";
		break;
		case "Panama":
			TargetLocation = "Panama";
		break;
		case "SantaCatalina":
			TargetLocation = "SantaCatalina";
		break;
		case "Terks":
			TargetLocation = "none";
		break;
		case "Dominica":
			TargetLocation = "none";
		break;
		case "Caiman":
			TargetLocation = "none";
		break;
		case "Tortuga":
			TargetLocation = "Tortuga";
		break;
		case "Trinidad":
			TargetLocation = "PortSpein";
		break;
		case "Tenotchitlan":
			TargetLocation = "none";
		break;
	}
    return TargetLocation;
}

string TimeGreeting()
{
    if (GetHour() >= 18 && GetHour() < 23)
    {
       return "Добрый вечер";
    }
    if (GetHour() >= 6 && GetHour() < 12)
    {
       return "Доброе утро";
    }
    if (GetHour() >= 12 && GetHour() < 18)
    {
       return "Добрый день";
    }
    if (GetHour() >= 23 || GetHour() < 6)
    {
       return "Доброй ночи";
    }
    return "Здрасте";
}

// выбор фразы от репутации
string PCharRepPhrase (string bad, string good)
{
	return NPCharRepPhrase(pchar, bad, good);
}
// boal для НПС
string NPCharRepPhrase(ref _pchar, string bad, string good)
{
	if(makeint(_pchar.reputation) < 41)
	{
		return bad;
	}
	else
	{
		return good;
	}
}

////////// поиск нужного перса для диалога
string GetCharIDByParam(string attrPresent1, string attr2, string val2)
{
    ref rCharacter; //ищем
	int n = GetCharIDXByParam(attrPresent1, attr2, val2);

    if (n > 0)
    {
        makeref(rCharacter,Characters[n]);
        return  rCharacter.id;
    }
    return  "";
}
int GetCharIDXByParam(string attrPresent1, string attr2, string val2)
{
    ref rCharacter; //ищем
	int n;

	for(n=0; n<MAX_CHARACTERS; n++)
	{
		makeref(rCharacter,Characters[n]);
		if (CheckAttribute(rCharacter, attrPresent1))
		{
            if (rCharacter.(attr2) == val2)
                return  n;
		}
    }
    return  -1;
}
int GetCharIDXForTownAttack(string attrPresent1)
{
    ref   rColony; //ищем
	int   n;
	bool  ok;

	for (n=0; n<MAX_COLONIES; n++)
	{
		makeref(rColony, colonies[n]);
		ok = false;
		if (CheckAttribute(rColony, "Default.BoardLocation2") && rColony.Default.BoardLocation2 == attrPresent1)
		{
		    ok = true;
		}
		if (rColony.Default.BoardLocation == attrPresent1 || ok)
        {
			if (rColony.HeroOwn == true)
			{
			    return  -1;
			}
			
			return GetFortCommanderIdx(rColony.id);
		}
    }
    return  -1;
}
// обращение НПС к ГГ, зависит от нации НПС
string GetAddress_Form(ref NPChar)
{
    string attr = NationShortName(sti(NPChar.nation));
    return address_form.(attr);
}
// обращение ГГ к НПС, зависит от нации НПС и его пола
string GetAddress_FormToNPC(ref NPChar)
{
    string attr  = NationShortName(sti(NPChar.nation));
    string attr2 = NPChar.sex;
    // проверка на скелетов
    if (attr2 != "woman")
    {
        attr2 = "man";
    }
    return address_form.(attr).(attr2);
}
string GetAddress_FormTitle(int nation, int num)
{
    string attr  =  NationShortName(nation);
    string attr2 =  "Title" + num;
    string ret   =  "нет звания";
    if (CheckAttribute(address_form, attr + "." + attr2))
    {
        ret = address_form.(attr).(attr2);
    }
    return ret;
}

bool isCityHasFort(string _city)
{
    int iTest = FindColony(_city); // город
    ref rColony;
	if (iTest != -1)
	{
		rColony = GetColonyByIndex(iTest);
		if (!CheckAttribute(rColony, "HasNoFort")) return true;
	}
	return false;
}

string GetCityFrom_Sea(string _city)
{
    int iTest = FindColony(_city); // город
    ref rColony;
	if (iTest != -1)
	{
		rColony = GetColonyByIndex(iTest);
		return rColony.from_sea;
	}
	return "";
}
// boal <--

// eddy. кто по профессии (только для типовых статиков, не фантомов). sPrefix - приставка для падежей: Gen, Dat..
string GetWorkTypeOfMan(ref NPChar, string sPrefix)
{
	string sCity, sTemp;
	sTemp = "unknown";
	if (CheckAttribute(NPChar, "City")) 
	{
		sCity = NPChar.City;
		sTemp = NPChar.id;
		sTemp = strcut(sTemp, strlen(sCity)+1, strlen(sTemp)-1);
	}
	return XI_ConvertString("Who" + sTemp + sPrefix);
}