# Work scratch 1



``` C++
void Execute(string file_name); // run file in different space (thread like)
void SendMessage(object to,string format_stringf,...);
void LayerCreate(string name,int ordered);
void LayerDelete(string name);
void  LayerFreeze(string name,int onoff);

```

``` C++
string NationShortName(int iNation) - краткое название нации (используется для всего!!! :) - в основном охотники, губеры и тд (т.е. return "eng"; "spa";  и тд)

int ChangeCharacterHunterScore(ref chref, string _huntName, int incr) // увеличение счетчика награды за голову 
(пример  ChangeCharacterHunterScore(GetMainCharacter(), NationShortName(sti(rMainGroupCharacter.nation)) + "hunter", 5);)

string GetAddress_Form(ref NPChar) - обращение, зависит от нации (сэр и тп)

string NationNameMan(int pNation) - англичанин и тп

string NationNamePeople(int pNation) - французы и тп

string NationNameAblative(int iNation) //творительный падеж - Англией

string NationNameNominative(int iNation) //именительный падеж - Голландия

string NationNameGenitive(int iNation) // родительный падеж - Франции

string GetCityName(string city) // имена городов по аттрибуту char.city - "Порт Рояль"

string GetPortByCityName(string city) // имена портов по городов по аттрибуту char.city - Conceicao_port

string GetIslandByCityName(string city) // имена острова по городу по аттрибуту char.city - Douwesen

string GetCityNameByIsland(string CurIslandId) - по острову даем город (главный город острова с портом)

string NationKingsName(ref NPChar) - имя монарха от нации - return "Его Величиства Якова II Стюарта";

string RandSwear() - восклицание ранд() - return "Карамба! ";

string SelectRumour() - слухи

string SelectCountryRumour(int nation) - слух, зависимый от нации (еще не готово)

string SelectSmugglingLocation() - по острову, в котором счас Гл герой, даем ранд() Бухту (контрабада так работает)

void SetCoastalGuardPursuit() - поставить патруль в море близ гл героя, герой на суше  ( контрабанда, но будут работать и просто так)

void StopCoastalGuardPursuit() - убрать патруль

string SelectSoldierModelByNation(ref rCharacter, string SoldierOrOfficer) - выбираем форму солдат от нации (солдаты ранд(), офицер один) SoldierOrOfficer == "officer" или "soldier"

int ChangeContrabandRelation(ref pchar, int _val) - изменение отношений контрабандистов (нужно по квестам использовать, как ветвление в диалоге)

ref GetFortCharRefByCity(string cityId) //ищем начальника форта города (я из него нацию беру)

SetFantomParam(ref pchar);  - раздача скилов, оружия и жизни (зависит от сложности)

string TimeGreeting() - "Здрасте" от времени суток

string RandPhraseSimple(string Var1, string Var2); - случайная фраза из двух

string LinkRandPhrase (string Var1, string Var2, string Var3) // выбор фразы из трех

string PCharRepPhrase (string Var1, string Var2) // выбор фразы от репутации

void FaceMaker(aref rCharacter) - дать Чару лицо и бошку, зависимо от его модели

void SetModelSailor(ref ChrefSailor) - морячок (модель + бошка)

void SetModelPirate(ref ChrefPirate) - пират (модель + бошка)

void SetCaptanModelByEncType(ref ChrefPirate, string sFantomType) выбор модели капитана зависимо от типа "trade" "war" "pirate"

int GetPrisonerQty() - сколько заключенных счас у гл героя

void SetShipMerchant(ref Char) - полностью экипировать и дать корабль торговца (квест сопровождения)

int RemoveCharacterGoodsSelf(ref _refCharacter,int _Goods,int _Quantity) - удалить товар только у данного НПС

int RemoveCharacterGoods(ref _refCharacter,int _Goods,int _Quantity) - обираем всех офицеров тоже

int GetSquadronGoods(ref _refCharacter,int _Goods) - колво груза на всех кораблях партии - ЯВНЫЙ ЧИТ - учитываются не компаньены (те торговец к примеру - его груз можно продать как контрабанду!!!!! Править не буду :))

int GetSquadronFreeSpace(ref _refCharacter,int _Goods) - свободное место (таже хрень - берем сопровождать торговца, продаем его товар, грузимся в магазине в него, довозим - денег куча - ЯВНЫЙ ЧИТ)

bool GetRemovable(ref _refCharacter)  - наш это перс или квестовый - это как раз то, что не достает методам выше :)
void SetCharacterRemovable(ref _refCharacter,bool removable) - ставим признак 

int GetCargoGoods(ref _refCharacter,int _Goods) - груз данного перса

int GetMaxItemsWeight(ref _chref) - мах вес, который может поднять перс

float GetItemsWeight(ref _chref) - текущий вес всех предметов

int GetSalaryForShip(ref chref) - предполагаемая зарплата за мес для Чара, включая корабль полностью и офицеров, для гл героя еще и пассажиров

int AddCharacterExpToSkill(ref _chref, string _skill, float _addValue) // Экспа в скил - автоматом растут ранги и перки _chref - character _skill - name ex -"Fencing"  _skill_exp = "Fencing_exp"   _addValue = 100

int GetSummonSkillFromName(ref _refCharacter, string skillName) - заначения скила skillName персанажа refCharacter с учетом вещей +1, офицеров, пенальти на корабль и перегруз

int GetCharacterSkill(ref _refCharacter, string skillName) - тоже самое, но без офицеров, только сам перс

float GetHullPercent(ref _refCharacter) - % целого корпуса (НР - это не проценты!!!! я уже несколько мест правил - баги)

float GetSailPercent(ref _refCharacter) - % целых парусов - а вот это == SP (оно само по себе %)



AddCharacterGoods - распределит меж всеми кораблями эскадры, что не влезет обрубит (метод только для ГГ, тк грузит не Removable)

AddCharacterGoodsSimple - дать конкретно в этот корабль (без учета эскадры и ремувабле)

SetCharacterGoods - применять НЕЛЬЗЯ
те затрет все, что было у ГГ до того - работает токо на один корабль


RemoveCharacterGoods - забрать товар у всей эскадры. (не ремувабле)

RemoveCharacterGoodsSelf - забрать у конкретного НПС (проверки ремува нет)

int GetCargoFreeSpace(ref _refCharacter) - скоко место свободно (это не в штуках, а в ц.)

int GetCharacterFreeSpace(ref _refCharacter,int _Goods)
- скоко штук товара влезет

int GetSquadronFreeSpace(ref _refCharacter,int _Goods)
это уже эскадра в шт. товара (с учетом ремувабле)

int GetSquadronGoods(ref _refCharacter,int _Goods)
- товар в шт. по эскаде (учет ремувабле)

AddDialogExitQuestFunction
- но выходу из диалога вызовет функцию, метод накопительный, те в диалоге можно кучу раз взывать, на выходе будет пачка функций последовательно


GetConvertStr(worldMap.island, "LocLables.txt") это аналог
    int    idLngFile = -1;
    string totalInfo;

    idLngFile = LanguageOpenFile("LocLables.txt");
    totalInfo = LanguageConvertString(idLngFile, _param);
    LanguageCloseFile(idLngFile);
    
//Запретить или разрешить генерации монстров
void LAi_LocationDisableMonstersGen(string locationId, bool isDisable);

//Запретить генерацию офицеров в локации, или разрешить после запрещения.
void LAi_LocationDisableOfficersGen(string locationId, bool isDisable);



void SetRandSelfSkill(ref _ch, int _min, int _max) // LeaderShip, FencLight, Fenc, FencHeavy, Pistol, Fortune, Sneak
void SetRandShipSkill(ref _ch, int _min, int _max) // Sailing, Commerce, Accuracy, Cannons, Repair, Grappling, Defence
- выставить персу _ch, рандомные cкиллы (минимальное значение - _min, максимальное - _max)

void RemoveAllCharacterItems(ref _ch, bool _removemoney)
- Отобрать все предметы (кроме квестовых) у выбранного персонажа. _removemoney - отбирать ини нет деньги

string strcur(string _sText, int FirstChar, int LastChar)
- вырезает часть строки _sText, начиная с буквы под номером FirstChar, заканчивая буквой под номером LastChar. Нумерация с НУЛЯ!

int strlen(string _sText) 
- метод возвращает количество символов в строке _sText, включая пробел.

void SetGameModeCondition(string _sQuest)
- Важный метод. Функция _sQuest будет проверяться в каждом фрейме (в паузе, при переходах между локациями тоже должна)
отключать методом DeleteGameModeCondition(string _sQuest)
Кочическво одновременно проверяйщихся функций не должно превышать MAX_GAMEMODE_CONDITIONS 10 (10 для того, чтоб тормозов не было)
```

"Весь мир театр, а люди в нем актеры"(С)

По ходу игры главный герой сталкивается  различными персонажами. Добавить в игру новых персонажей или изменить реакции существующих - наша задача.

Все НПС (актеры или статисты) хранятся  в Characters[]
Попадают они туда при инициализации игры различными путями:
(здесь и далее я буду приводить описание методов и переменных исходя из того, что Вы знаете где их искать, иначе используйте поиск по папке программ)

CreateCharacters() -  инициализация всего массива и главного героя
CreateOtherCharacters(); - все остальные служедные НПС (в ПКМ это лодка на берегу)

Они заполняются в файлах офицеры, другие, квестовые персонажи и во всех файлов островов (C:\gama\seadog2\PROGRAM\Characters\init)
При этом счетчик n увеличивается. Всего в игру заложена возможность инициализации 1000 персонажей (но это изменяемо)

``` C++
	makeref(ch,Characters[n]);		//Даниэль
	ch.name 	= "Даниель"; - имя в игре
	ch.lastname = "Грин";   - фамилия
	ch.id		= "Danielle"; - ИД перса для обращения к нему в коде
	ch.nation	= ENGLAND; - нация (ни на что не влияет)
	ch.model	= "Danielle"; - модель, можно менять динамически
	ch.activity_begin = "0";
	ch.activity_end = "24";
	ch.sex = "woman";  - пол :) (для модели нужно задать тип анимации model.animation = "woman";)
	ch.luck = 4;
	GiveItem2Character(ch, "blade6"); - заполняем карман
	TakeNItems(ch,"potion1", Rand(4)+1);
	ch.equip.blade = "blade6"; - что в руках
	GiveItem2Character(ch, GUN_COMMON);
	ch.equip.gun = GUN_COMMON; - что в руках, вообще есть спец метод екипировки, в инициализации рудимент
	ch.location	= ""; - где болтается (можно менять по сюжету)
	ch.location.group = ""; к какой группе принадлежит локатор
	ch.location.locator = ""; локатор, где конкретно болтается в локации, если пусто, то нигде нет, ждет своего часа
	ch.Dialog.Filename = "Danielle_dialog.c"; диалог, который сработает при обращении к персу
	ch.greeting = "Gr_danielle"; - альяс для звуков (список файлов, из которых перс вякает при диалоге)
	ch.rank 	= 4; - ранг
	ch.reputation = "50"; - репутация
	ch.experience = CalculateExperienceFromRank(4)+ (CalculateExperienceFromRank(4)/10 + rand(4000)); - текущий опыт (для адона рудимент)
	ch.skill.Leadership = "2"; - скилы
	ch.skill.Fencing = "4";
	ch.skill.Sailing = "2";
	ch.skill.Accuracy = "3";
	ch.skill.Cannons = "2";
	ch.skill.Grappling = "1";
	ch.skill.Repair = "1";
	ch.skill.Defence = "1";
	ch.skill.Commerce = "1";
	ch.skill.Sneak = "1"; - это удача, а не скрытность как написано :)
	ch.money = "0"; - деньги в кармане(т.е считай, что предмет)
	ch.perks.list.LongRangeShoot = true; - перки
	ch.Ship.Name = "Vertigo"; - имя корабля (может не быть корабля вообще. Для того чтою отобразить корабль в игре нужно спец функции вызвать)
	ch.Ship.Type = SHIP_SCHOONER2_ENGLAND;
	ch.Ship.Cannons.Type = CANNON_TYPE_CANNON_LBS16;
	ch.ship.cargo.goods.bombs = 1000;  - задавать так товары ИЗВРАТ (методы есть), но в инициализации бывает - рудимент.
	ch.FaceId = 30; - морда лица при диалоге (перекрывается дальше и тут не на что не влияет и не соответствует правде) - рудимент
``` 
типа
``` C++
switch (rCharacter.model)
		{
			case "black_corsair": 	rCharacter.FaceId = 0; break;
			case "bocman1":			rCharacter.FaceId = 1; break;
			case "bocman3":			rCharacter.FaceId = 2; break;
			case "bocman4":			rCharacter.FaceId = 3; break;
```
Зависит исключительно от модели
Внимание! Добавление новой модели требует добавление морды!!!
Динамическая смена модели морду не меняет

Итак, у нас в игре все актеры и статисты обозначены.. Это не значит, что герой их сразу увидит. Многие должны сами появляться, а не сидеть сиднями..
Да, кстати, чтоб НПС именно сидел (в таверне) используется специальный локатор, например
ch.location.group = "sit";
ch.location.locator = "sit1";

Система квестов в игре основана на событиях
```C++
Ch.quest.Story_LeavingOxbay.win_condition.l1 = "location"; - тип события (появится в локации)
Ch.quest.Story_LeavingOxbay.win_condition.l1.character = "Blaze"; (кто появляется)
Ch.quest.Story_LeavingOxbay.win_condition.l1.location = "Oxbay"; (где)
Ch.quest.Story_LeavingOxbay.win_condition = "Story_leavingOxbay"; (состояние квеста при совпадении)
Ch.quest.Story_LeavingOxbay.fail_condition = "Story_leavingOxbay"; (при не совпадении, но вопрос??? Не ясно когда оно сработает и что будет.. возможно не сработает вовсе)
```

Введем терминологию
Состояние квеста - СК

Итак, СК обрабатывается в диалогах и, главное, в quests_reaction.c  QuestComplete();
И другими файлами из папки квест.

Нужно учесть, что СК это строка и никаких ошибок при ошибке в ней не будет вовсе
(эта беда игры - не перечислимый тип и бич тестеров. Не удивительно, что игру тестили целый год :) Поэтому, нужно быть очень внимательным!!!!
