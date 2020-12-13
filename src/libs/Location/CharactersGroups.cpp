//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//	CharactersGroups
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#include "CharactersGroups.h"
#include "NPCharacter.h"

#define CGS_LOOK 15.0f
#define CGS_HEAR 2.5f
#define CGS_SAY 5.0f
#define CGS_START_ALARM 0.0f
#define CGS_ALARMDOWN 0.02f
#define CGS_ALARMMIN 0.3f
#define CGS_ALARMMAX 0.7f
#define CGS_ALARMVIS 0.3f
#define CGS_DEFMAXTIME 20.0f
#define CGS_VIEWANGLE 100.0f

//============================================================================================
//Конструирование, деструктурирование
//============================================================================================

CharactersGroups::CharactersGroups()
{
    numGroups = 0;
    maxGroups = 0;
    location = nullptr;
    waveTime = 10.0f;
    curExecuteChr = -1;
}

CharactersGroups::~CharactersGroups()
{
    for (long i = 0; i < numGroups; i++)
    {
        delete groups[i]->relations;
        delete groups[i];
    }
}

CharactersGroups::String::String()
{
    name = nullptr;
    len = 0;
    max = 0;
    hash = 0;
}

CharactersGroups::String::String(const char *str)
{
    name = nullptr;
    len = 0;
    max = 0;
    hash = 0;
    *this = str;
}

CharactersGroups::String::~String()
{
    delete name;
}

void CharactersGroups::String::operator=(const char *str)
{
    if (!str || !str[0])
    {
        len = 0;
        name = new char[1];
        name[0] = 0;
    }
    else
    {
        len = strlen(str);
        if (len + 1 > max)
        {
            delete name;
            max = (len + 16) & ~15;
            name = new char[max];
        }
        strcpy_s(name, len + 1, str);
        hash = LocatorArray::CalcHashString(str);
    }
}

bool CharactersGroups::String::Cmp(const char *str, long l, long h) const
{
    if (!name || !name[0])
    {
        if (!str || !str[0])
            return true;
        return false;
    }
    if (hash != h)
        return false;
    if (len != l)
        return false;
    return _stricmp(name, str) == 0;
}

long CharactersGroups::String::GetHash(const char *str)
{
    if (!str)
        return 0;
    return LocatorArray::CalcHashString(str);
}

long CharactersGroups::String::GetLen(const char *str)
{
    if (!str)
        return 0;
    return strlen(str);
}

//Инициализация
bool CharactersGroups::Init()
{
    //Указатель на локацию
    const auto loc = EntityManager::GetEntityId("location");
    location = static_cast<Location *>(EntityManager::GetEntityPointer(loc));
    if (!location)
        return false;
    RegistryGroup("");
    // api->LayerCreate("execute", true, false);
    EntityManager::SetLayerType(EXECUTE, EntityManager::Layer::Type::execute);
    EntityManager::AddToLayer(EXECUTE, GetId(), 10);
    return true;
}

//Исполнение
void CharactersGroups::Execute(uint32_t delta_time)
{
    //Время
    const auto dltTime = delta_time * 0.001f;
    //Исполняем группы
    const auto playerGroup = FindGroupIndex("Player");
    auto playerAlarm = 0.0f;
    auto playerActive = false;
    auto isDeactivate = false;
    for (long i = 0; i < numGroups; i++)
    {
        auto *const rel = groups[i]->relations;
        for (long j = 0; j <= i; j++)
        {
            rel[j].alarm -= dltTime * rel[j].alarmdown;
            if (rel[j].alarm < 0.0f)
                rel[j].alarm = 0.0f;
            const auto lastActive = rel[j].isActive;
            if (rel[j].alarm <= rel[j].alarmmin)
                rel[j].isActive = false;
            if (rel[j].alarm >= rel[j].alarmmax)
                rel[j].isActive = true;
            if (rel[j].isActive != lastActive)
            {
                if (!rel[j].isActive)
                {
                    rel[j].curState = rel[j].relState;
                    isDeactivate = true;
                }
                else
                {
                    rel[j].curState = rel[j].actState;
                }
            }
        }
        if (playerGroup >= 0 && playerGroup != i && groups[i]->numChr > 0)
        {
            auto &rl = FindRelation(playerGroup, i);
            if (rl.curState != rs_enemy)
                continue;
            if (playerAlarm < rl.alarm)
                playerAlarm = rl.alarm;
            long n;
            for (n = 0; n < groups[i]->numChr; n++)
            {
                auto *cg = static_cast<Character *>(EntityManager::GetEntityPointer(groups[i]->c[n]));
                if (cg && cg->IsSetBlade())
                    break;
            }
            if (n >= groups[i]->numChr)
                continue;
            if (rl.isActive)
                playerActive = true;
        }
    }
    //Удалим все неправильные цели
    if (isDeactivate)
        RemoveAllInvalidTargets();
    //Сообщим о текущем положении дел у игрока
    api->Event("CharacterGroup_UpdateAlarm", "fl", playerAlarm, playerActive);
    //Исполняем персонажей
    waveTime += dltTime;
    if (curExecuteChr >= 0)
    {
        if (curExecuteChr < location->supervisor.numCharacters)
        {
            auto *const c = location->supervisor.character[curExecuteChr].c;
            CharacterVisibleCheck(c);
            curExecuteChr++;
        }
        else
            curExecuteChr = -1;
    }
    else
    {
        if (waveTime >= 0.5f)
        {
            waveTime = 0.0f;
            curExecuteChr = 0;
        }
    }
}

//Проверка обнаружения персонажем остальных
void CharactersGroups::CharacterVisibleCheck(Character *chr)
{
    if (!chr)
        return;
    //Удалим отработавшие цели
    RemoveInvalidTargets(chr);
    //Группа персонажа
    const auto gi = GetCharacterGroup(chr);
    if (gi < 0)
        return;
    auto *const grp = groups[gi];
    //Видимая область
    long num;
    if (location->supervisor.FindCharacters(fnd, num, chr, grp->look, CGS_VIEWANGLE, 0.05f))
    {
        FindEnemyFromFindList(chr, grp, num, true);
    }
    //Слышимая область
    if (location->supervisor.FindCharacters(fnd, num, chr, grp->hear))
    {
        FindEnemyFromFindList(chr, grp, num, false);
    }
}

//Проверить найденных персонажей на врагов
void CharactersGroups::FindEnemyFromFindList(Character *chr, Group *grp, long num, bool visCheck)
{
    Character *targets[MAX_CHARACTERS];
    long numTrg = 0;
    //По всем найденным персонажем
    for (long i = 0; i < num; i++)
    {
        //Группа найденного персонажа
        const auto gi = GetCharacterGroup(fnd[i].c);
        if (gi < 0)
            continue;
        auto &r = FindRelation(grp->index, gi);
        if (r.curState != rs_enemy)
            continue;
        //Обнаружен враг, если невидим, то пропустим его
        if (visCheck && !chr->VisibleTest(fnd[i].c))
            continue;
        //Обнаружили врага, добавим
        if (AddEnemyTarget(chr, fnd[i].c))
        {
            if (numTrg < MAX_CHARACTERS)
                targets[numTrg++] = fnd[i].c;
        }
    }
    //Сообщим окружающим об обнаруженных целях
    if (numTrg > 0 && location->supervisor.FindCharacters(fnd, num, chr, grp->say))
    {
        for (long i = 0; i < num; i++)
        {
            auto *const c = fnd[i].c;
            //Если невидим, то пропустим его
            if (!chr->VisibleTest(c))
                continue;
            //Получим группу персонажа
            const auto cgrp = GetCharacterGroup(c);
            if (cgrp < 0)
                continue;
            //Группа найденного персонажа
            auto &r = FindRelation(grp->index, cgrp);
            if (r.curState != rs_friend)
                continue;
            for (long j = 0; j < numTrg; j++)
            {
                if (grp->index != cgrp)
                {
                    const auto tgrp = GetCharacterGroup(targets[j]);
                    if (tgrp >= 0)
                    {
                        auto &tr = FindRelation(cgrp, tgrp);
                        if (tr.curState == rs_friend)
                        {
                            //Обе группы дружественны к текущей, смотрим на приоритеты
                            if (groups[tgrp]->priority >= groups[grp->index]->priority)
                                continue;
                        }
                    }
                }
                AddEnemyTarget(c, targets[j]);
            }
        }
    }
}

//Добавить или обновить врага
bool CharactersGroups::AddEnemyTarget(Character *chr, Character *enemy, float maxtime)
{
    if (!chr || !enemy)
        return false;
    //Обновим аларм
    const auto isSelf = false;
    auto &r = FindRelation(GetCharacterGroup(chr), GetCharacterGroup(enemy));
    Assert(!isSelf);
    r.alarm = 1.0f;
    if (r.alarm >= r.alarmmax)
        r.isActive = true;
    if (r.isActive)
        r.curState = r.actState;
    if (r.actState != rs_enemy)
        return false;
    //Ищим среди добавленных
    for (long i = 0; i < chr->numTargets; i++)
    {
        if (enemy == EntityManager::GetEntityPointer(chr->grpTargets[i].chr))
        {
            chr->grpTargets[i].time = 0.0f;
            return true;
        }
    }
    if (chr->numTargets >= sizeof(chr->grpTargets) / sizeof(Character::GrpTarget))
        return false;
    //Добавим новую цель
    Assert(_stricmp(chr->group, enemy->group) != 0);
    auto &trg = chr->grpTargets[chr->numTargets++];
    trg.chr = enemy->GetId();
    trg.time = 0.0f;
    if (maxtime < 0.0f)
        trg.timemax = 3.0f + (rand() & 7);
    else
        trg.timemax = maxtime;
    return true;
}

//Удалить все неактивные или неправильные цели
void CharactersGroups::RemoveAllInvalidTargets()
{
    //Обновим списки целей
    for (long i = 0; i < location->supervisor.numCharacters; i++)
    {
        RemoveInvalidTargets(location->supervisor.character[i].c);
    }
}

//Удалить неактивные или неправильные цели
bool CharactersGroups::RemoveInvalidTargets(Character *chr, Character *check)
{
    if (!chr)
        return false;
    const auto gi = GetCharacterGroup(chr);
    if (gi < 0)
    {
        chr->numTargets = 0;
        return false;
    }
    auto isValidate = false;
    for (long i = 0; i < chr->numTargets;)
    {
        auto isDelete = true;
        auto &trg = chr->grpTargets[i];
        auto *c = static_cast<Character *>(EntityManager::GetEntityPointer(trg.chr));
        if (c && (trg.time < trg.timemax || trg.timemax < 0.0f))
        {
            if (!c->IsDead())
            {
                //Персонаж существует и о нём помнят
                const auto gc = GetCharacterGroup(c);
                auto &r = FindRelation(gi, gc);
                if (r.curState == rs_enemy)
                {
                    isDelete = false;
                    if (check == c)
                        isValidate = true;
                }
            }
        }
        if (isDelete)
        {
            trg = chr->grpTargets[--chr->numTargets];
        }
        else
            i++;
    }
    return isValidate;
}

//Сообщения
uint64_t CharactersGroups::ProcessMessage(MESSAGE &message)
{
    char cmd[64];
    message.String(sizeof(cmd), cmd);
    cmd[sizeof(cmd) - 1] = 0;
    if (!cmd[0])
        return 0;
    if (_stricmp(cmd, "VldTrg") == 0)
    {
        return MsgIsValidateTarget(message);
    }
    if (_stricmp(cmd, "GetTrg") == 0)
    {
        return MsgGetOptimalTarget(message);
    }
    if (_stricmp(cmd, "IsEnemy") == 0)
    {
        return MsgIsEnemy(message);
    }
    if (_stricmp(cmd, "MoveChr") == 0)
    {
        return MoveCharacterToGroup(message);
    }
    if (_stricmp(cmd, "Attack") == 0)
    {
        MsgAttack(message);
        return 1;
    }
    if (_stricmp(cmd, "AddTarget") == 0)
    {
        MsgAddTarget(message);
        return 1;
    }
    if (_stricmp(cmd, "UpdChrTrg") == 0)
    {
        MsgUpdChrTrg(message);
        return 1;
    }
    if (_stricmp(cmd, "RegistryGroup") == 0)
    {
        MsgRegistryGroup(message);
        return 1;
    }
    if (_stricmp(cmd, "ReleaseGroup") == 0)
    {
        MsgReleaseGroup(message);
        return 1;
    }
    if (_stricmp(cmd, "SetRelation") == 0)
    {
        MsgSetRelation(message);
        return 1;
    }
    if (_stricmp(cmd, "SetAlarmReaction") == 0)
    {
        MsgSetAlarmReaction(message);
        return 1;
    }
    if (_stricmp(cmd, "SetGroupLook") == 0)
    {
        return MsgSetGroupLook(message);
    }
    if (_stricmp(cmd, "SetGroupHear") == 0)
    {
        return MsgSetGroupHear(message);
    }
    if (_stricmp(cmd, "SetGroupSay") == 0)
    {
        return MsgSetGroupSay(message);
    }
    if (_stricmp(cmd, "SetGroupPriority") == 0)
    {
        return MsgSetGroupPriority(message);
    }
    if (_stricmp(cmd, "UnloadCharacter") == 0)
    {
        UnloadCharacter(message);
        return 1;
    }
    if (_stricmp(cmd, "ResetWaveTime") == 0)
    {
        waveTime = 1000.0f;
        return 1;
    }
    if (_stricmp(cmd, "SetAlarm") == 0)
    {
        return MsgSetAlarm(message);
    }
    if (_stricmp(cmd, "SetAlarmDown") == 0)
    {
        return MsgSetAlarmDown(message);
    }
    if (_stricmp(cmd, "ClearAllTargets") == 0)
    {
        ClearAllTargets();
        return 1;
    }
    if (_stricmp(cmd, "SaveData") == 0)
    {
        SaveData();
        return 1;
    }
    if (_stricmp(cmd, "LoadDataRelations") == 0)
    {
        LoadDataRelations();
        return 1;
    }
    if (_stricmp(cmd, "RestoreStates") == 0)
    {
        RestoreStates();
        return 1;
    }
    return 0;
}

//Изменение атрибута
uint32_t CharactersGroups::AttributeChanged(ATTRIBUTES *apnt)
{
    return 0;
}

//Проверить на действительность цель
bool CharactersGroups::MsgIsValidateTarget(MESSAGE &message)
{
    const auto chr = message.EntityID();
    const auto trg = message.EntityID();
    auto *c = static_cast<Character *>(EntityManager::GetEntityPointer(chr));
    if (!c)
        return false;
    auto *en = static_cast<Character *>(EntityManager::GetEntityPointer(trg));
    if (!en)
        return false;
    CVECTOR vP1, vP2;
    c->GetPosition(vP1);
    en->GetPosition(vP2);
    const auto fDistance = sqrtf(~(vP1 - vP2));
    AttributesPointer->SetAttributeUseFloat("distance", fDistance);
    return RemoveInvalidTargets(c, en);
}

//Найти оптимальную цель
bool CharactersGroups::MsgGetOptimalTarget(MESSAGE &message) const
{
    const auto chr = message.EntityID();
    auto *c = static_cast<Character *>(EntityManager::GetEntityPointer(chr));
    if (!c)
        return false;
    auto *vd = message.ScriptVariablePointer();
    if (!vd)
        return false;
    if (c->numTargets <= 0)
        return false;
    long s = 0;
    if (c->numTargets > 1)
    {
        CVECTOR pos, p;
        c->GetPosition(pos);
        const auto numChr = location->supervisor.numCharacters;
        auto *const cEx = location->supervisor.character;
        //Выберем оптимальную цель
        float value;
        s = -1;
        for (long i = 0; i < c->numTargets; i++)
        {
            //Указатель на персонажа
            auto *nc = static_cast<NPCharacter *>(EntityManager::GetEntityPointer(c->grpTargets[i].chr));
            if (!nc)
                continue;
            if (!nc->IsSetBlade())
                continue;
            //Соберём количество персонажей воюющих с этим хмырём
            long n = 0;
            for (long j = 0; j < numChr; j++)
            {
                if (cEx[j].c == nc || cEx[j].c == c)
                    continue;
                if (static_cast<NPCharacter *>(cEx[j].c)->GetAttackedCharacter() == nc)
                    n++;
            }
            //Найдём дистанцию
            nc->GetPosition(p);
            auto v = ~(pos - p);
            //Посчитаем эврестическое значение
            v *= 0.5f + n * n * 0.5f;
            //Учтём наличие у него оружия
            // if(!nc->IsSetBlade()) v *= v*100.0f;
            //Учитываем выбор цели
            if (s >= 0)
            {
                if (v < value)
                {
                    value = v;
                    s = i;
                }
            }
            else
            {
                value = v;
                s = i;
            }
        }
        if (s < 0)
            s = 0;
    }
    c = static_cast<Character *>(EntityManager::GetEntityPointer(c->grpTargets[s].chr));
    // if(!c->IsSetBlade()) return false;
    if (c->AttributesPointer)
    {
        vd->Set(static_cast<long>(c->AttributesPointer->GetAttributeAsDword("index", -1)));
    }
    else
    {
        vd->Set(static_cast<long>(-1));
    }
    return true;
}

//Враг ли данный персонаж
bool CharactersGroups::MsgIsEnemy(MESSAGE &message)
{
    const auto g1 = GetCharacterGroup(static_cast<Character *>(EntityManager::GetEntityPointer(message.EntityID())));
    const auto g2 = GetCharacterGroup(static_cast<Character *>(EntityManager::GetEntityPointer(message.EntityID())));
    if (g1 < 0 || g2 < 0)
        return false;
    auto isSelf = false;
    auto &r = FindRelation(g1, g2, &isSelf);
    if (isSelf)
        return false;
    return r.curState == rs_enemy;
}

//Реакция групп на атаку
void CharactersGroups::MsgAttack(MESSAGE &message)
{
    //Получим группы
    auto *const gAttack = GetGroup(message);
    auto *const gHit = GetGroup(message);
    Assert(gAttack);
    Assert(gHit);
    auto &r = FindRelation(gAttack->index, gHit->index);
    r.alarm = 1.0f;
    if (r.alarm >= r.alarmmax)
        r.isActive = true;
    if (r.isActive)
        r.curState = r.actState;
    //Установим враждебные отношения у всех дружественных к пострадавшей групп
    for (long i = 0; i < numGroups; i++)
    {
        if (gHit == groups[i])
            continue;
        if (gAttack == groups[i])
            continue;
        auto &r = FindRelation(gHit->index, i);
        if (r.curState != rs_friend)
            continue;
        //Посорим с нападающими
        auto &ra = FindRelation(gAttack->index, i);
        ra.alarm = 1.0f;
        if (ra.alarm >= ra.alarmmax)
            ra.isActive = true;
        if (ra.isActive)
            ra.curState = ra.actState;
    }
}

void CharactersGroups::MsgAddTarget(MESSAGE &message)
{
    //Получаем персонажей
    auto eid = message.EntityID();
    auto *chr = static_cast<Character *>(EntityManager::GetEntityPointer(eid));
    eid = message.EntityID();
    auto *enemy = static_cast<Character *>(EntityManager::GetEntityPointer(eid));
    if (!chr || !enemy)
        return;
    //Проверяем на враждебность
    const auto g1 = GetCharacterGroup(chr);
    const auto g2 = GetCharacterGroup(enemy);
    if (g1 < 0 || g2 < 0)
        return;
    auto isSelf = false;
    auto &r = FindRelation(g1, g2, &isSelf);
    if (isSelf)
        return;
    if (r.curState != rs_enemy)
        return;
    //Добавляем врага
    AddEnemyTarget(chr, enemy, message.Float());
    //Сообщаем окружающим о новой цели
    long num = 0;
    if (location->supervisor.FindCharacters(fnd, num, chr, groups[g1]->say))
    {
        for (long i = 0; i < num; i++)
        {
            auto *const c = fnd[i].c;
            //Если невидим, то пропустим его
            if (!chr->VisibleTest(c))
                continue;
            //Группа найденного персонажа
            auto &r = FindRelation(g1, GetCharacterGroup(c));
            if (r.curState != rs_friend)
                continue;
            AddEnemyTarget(c, enemy);
        }
    }
}

//Обновить цели у данного персонажа
void CharactersGroups::MsgUpdChrTrg(MESSAGE &message)
{
    const auto eid = message.EntityID();
    auto *chr = static_cast<Character *>(EntityManager::GetEntityPointer(eid));
    if (chr)
        CharacterVisibleCheck(chr);
}

//Зарегистрировать группу
void CharactersGroups::MsgRegistryGroup(MESSAGE &message)
{
    char grpName[128];
    message.String(sizeof(grpName), grpName);
    grpName[sizeof(grpName) - 1] = 0;
    RegistryGroup(grpName);
}

//Удалить группу
void CharactersGroups::MsgReleaseGroup(MESSAGE &message)
{
    char grpName[128];
    message.String(sizeof(grpName), grpName);
    grpName[sizeof(grpName) - 1] = 0;
    ReleaseGroup(grpName);
}

//Зарегистрировать группу
long CharactersGroups::RegistryGroup(const char *groupName)
{
    const auto idxgrp = FindGroupIndex(groupName);
    if (idxgrp >= 0)
        return idxgrp;
    //Регестрируем
    if (numGroups >= maxGroups)
    {
        maxGroups += 16;
        groups.resize(maxGroups);
    }
    auto *grp = new Group();
    grp->index = numGroups;
    grp->name = groupName;
    grp->look = CGS_LOOK;
    grp->hear = CGS_HEAR;
    grp->say = CGS_SAY;
    grp->priority = 10;
    groups[numGroups++] = grp;
    if (numGroups)
    {
        //Таблица отношений
        grp->relations = new Relation[numGroups];
        for (long i = 0; i < numGroups - 1; i++)
        {
            grp->relations[i].alarm = CGS_START_ALARM;
            grp->relations[i].alarmdown = CGS_ALARMDOWN;
            grp->relations[i].alarmmin = CGS_ALARMMIN;
            grp->relations[i].alarmmax = CGS_ALARMMAX;
            grp->relations[i].isActive = false;
            if (grp->relations[i].alarm >= grp->relations[i].alarmmax)
                grp->relations[i].isActive = true;
            grp->relations[i].curState = rs_neitral;
            grp->relations[i].actState = rs_enemy;
            grp->relations[i].relState = rs_neitral;
        }
        //Сами с собой всегда дружим
        grp->relations[numGroups - 1].alarm = 0.0f;
        grp->relations[numGroups - 1].alarmdown = 0.0f;
        grp->relations[numGroups - 1].alarmmin = 0.0f;
        grp->relations[numGroups - 1].alarmmax = 10.0f;
        grp->relations[numGroups - 1].isActive = false;
        grp->relations[numGroups - 1].curState = rs_friend;
        grp->relations[numGroups - 1].actState = rs_friend;
        grp->relations[numGroups - 1].relState = rs_friend;
    }
    else
        grp->relations = nullptr;
    grp->numChr = 0;
    return numGroups - 1;
}

void CharactersGroups::ReleaseGroup(const char *groupName)
{
    const auto idxgrp = FindGroupIndex(groupName);
    if (idxgrp < 0)
        return; // нет группы нечего удалять
    if (numGroups > 1)
    {
        groups[idxgrp] = groups[numGroups - 1];
        groups[numGroups - 1] = nullptr;
    }
    numGroups--;
}

//Установить для группы радиус видимости
bool CharactersGroups::MsgSetGroupLook(MESSAGE &message)
{
    //Текущая группа
    auto *const grp = GetGroup(message, false);
    if (!grp)
        return false;
    //Радиус
    grp->look = message.Float();
    if (grp->look < 0.0f)
        grp->look = 0.0f;
    return true;
}

//Установить для группы радиус слышимости
bool CharactersGroups::MsgSetGroupHear(MESSAGE &message)
{
    //Текущая группа
    auto *const grp = GetGroup(message, false);
    if (!grp)
        return false;
    //Радиус
    grp->hear = message.Float();
    if (grp->hear < 0.0f)
        grp->hear = 0.0f;
    return true;
}

//Установить для группы радиус сообщения
bool CharactersGroups::MsgSetGroupSay(MESSAGE &message)
{
    //Текущая группа
    auto *const grp = GetGroup(message, false);
    if (!grp)
        return false;
    //Радиус
    grp->say = message.Float();
    if (grp->say < 0.0f)
        grp->say = 0.0f;
    return true;
}

//Установить для группы приоритет
bool CharactersGroups::MsgSetGroupPriority(MESSAGE &message)
{
    //Текущая группа
    auto *const grp = GetGroup(message, false);
    if (!grp)
        return false;
    //Приоритет
    grp->priority = message.Long();
    return true;
}

//Установить скорость уровень тревоги
bool CharactersGroups::MsgSetAlarm(MESSAGE &message)
{
    //Текущая группа
    auto *const g1 = GetGroup(message, false);
    if (!g1)
        return false;
    auto *const g2 = GetGroup(message, false);
    if (!g2)
        return false;
    auto isSelf = false;
    auto &r = FindRelation(g1->index, g2->index, &isSelf);
    if (isSelf)
        return false;
    //Уровень тревоги
    r.alarm = message.Float();
    if (r.alarm < 0.0f)
        r.alarm = 0.0f;
    if (r.alarm > 1.0f)
        r.alarm = 1.0f;
    if (r.alarm <= r.alarmmin)
        r.isActive = false;
    if (r.alarm >= r.alarmmax)
        r.isActive = true;
    RemoveAllInvalidTargets();
    return true;
}

//Установить скорость убывания тревоги
bool CharactersGroups::MsgSetAlarmDown(MESSAGE &message)
{
    //Текущая группа
    auto *const g1 = GetGroup(message, false);
    if (!g1)
        return false;
    auto *const g2 = GetGroup(message, false);
    if (!g2)
        return false;
    auto isSelf = false;
    auto &r = FindRelation(g1->index, g2->index, &isSelf);
    if (isSelf)
        return false;
    //Скорость убывания
    r.alarmdown = message.Float();
    if (r.alarmdown < 0.0f)
        r.alarmdown = CGS_ALARMDOWN;
    return true;
}

//Добавить в группу персонажа
bool CharactersGroups::MoveCharacterToGroup(MESSAGE &message)
{
    const auto eid = message.EntityID();
    auto *chr = static_cast<Character *>(EntityManager::GetEntityPointer(eid));
    if (!chr)
        return false;
    //Создадим группу
    char grpName[128];
    message.String(sizeof(grpName), grpName);
    grpName[sizeof(grpName) - 1] = 0;
    auto *grp = FindGroup(grpName);
    if (!grp)
        RegistryGroup(grpName);
    grp = FindGroup(grpName);
    Assert(grp);
    //Удалим персонажа из предыдущей группы
    RemoveCharacterFromAllGroups(eid);
    //Проверим на свободное место в группе
    // boal fix for intel cpp if(grp->numChr >= sizeof(CharactersGroups::Group::c)/sizeof(Character *)) return false;
    if (grp->numChr >= MAX_CHARACTERS)
        return false; // fix
    //Разместим в новой
    grp->c[grp->numChr++] = eid;
    strcpy_s(chr->group, grpName);
    RemoveInvalidTargets(chr);
    return true;
}

//Установить отношения между группами
void CharactersGroups::MsgSetRelation(MESSAGE &message)
{
    auto isSelf = false;
    auto &r = FindRelation(message, &isSelf);
    if (isSelf)
        return;
    char buf[32];
    message.String(sizeof(buf), buf);
    buf[sizeof(buf) - 1] = 0;
    auto actState = rs_enemy;
    auto relState = rs_neitral;
    if (_stricmp(buf, "friend") == 0)
    {
        r.curState = rs_friend;
        actState = rs_enemy;
        relState = rs_neitral;
        r.alarm = 0.0f;
        r.alarmdown = CGS_ALARMDOWN;
        r.alarmmin = CGS_ALARMMIN;
        r.alarmmax = CGS_ALARMMAX;
        r.isActive = false;
    }
    else if (_stricmp(buf, "neitral") == 0)
    {
        r.curState = rs_neitral;
        actState = rs_enemy;
        relState = rs_neitral;
        r.alarm = 0.0f;
        r.alarmdown = CGS_ALARMDOWN;
        r.alarmmin = CGS_ALARMMIN;
        r.alarmmax = CGS_ALARMMAX;
    }
    else if (_stricmp(buf, "enemy") == 0)
    {
        r.curState = rs_enemy;
        actState = rs_enemy;
        relState = rs_enemy;
        r.alarm = 1.0f;
        r.isActive = true;
        r.alarmdown = 0.0f;
        r.alarmmin = 0.0f;
        r.alarmmax = 10.0f;
    }
    r.actState = actState;
    r.relState = relState;
    if (r.isActive)
        r.curState = r.actState;
}

//Установить реакцию на тревогу для пары групп
void CharactersGroups::MsgSetAlarmReaction(MESSAGE &message)
{
    auto isSelf = false;
    auto &r = FindRelation(message, &isSelf);
    if (isSelf)
        return;
    char act[32];
    message.String(sizeof(act), act);
    act[sizeof(act) - 1] = 0;
    char rel[32];
    message.String(sizeof(rel), rel);
    rel[sizeof(rel) - 1] = 0;
    auto actState = rs_enemy;
    if (_stricmp(act, "neitral") == 0)
    {
        actState = rs_neitral;
    }
    else if (_stricmp(act, "friend") == 0)
    {
        actState = rs_friend;
    }
    auto relState = rs_neitral;
    if (_stricmp(rel, "enemy") == 0)
    {
        relState = rs_enemy;
    }
    else if (_stricmp(rel, "friend") == 0)
    {
        relState = rs_friend;
    }
    r.actState = actState;
    r.relState = relState;
    if (r.isActive)
        r.curState = r.actState;
}

//Исключить персонажа из всех групп
void CharactersGroups::RemoveCharacterFromAllGroups(entid_t chr)
{
    auto *const ch = chr ? static_cast<Character *>(EntityManager::GetEntityPointer(chr)) : nullptr;
    //Удалим персонажа из предыдущей группы
    for (long i = 0; i < numGroups; i++)
    {
        auto *g = groups[i];
        auto *const cid = g->c;
        for (long j = 0; j < g->numChr;)
        {
            auto *c = static_cast<Character *>(EntityManager::GetEntityPointer(cid[j]));
            if (c == nullptr || c == ch)
            {
                cid[j] = cid[--g->numChr];
            }
            else
                j++;
        }
    }
}

//Выгрузка персонажа
void CharactersGroups::UnloadCharacter(MESSAGE &message)
{
    const auto eid = message.EntityID();
    RemoveCharacterFromAllGroups(eid);
}

//Получить группу из сообщения
CharactersGroups::Group *CharactersGroups::GetGroup(MESSAGE &message, bool isRegistry)
{
    char grpName[128];
    message.String(sizeof(grpName), grpName);
    grpName[sizeof(grpName) - 1] = 0;
    auto *grp = FindGroup(grpName);
    if (!grp && isRegistry)
    {
        RegistryGroup(grpName);
        grp = FindGroup(grpName);
        Assert(grp);
    }
    return grp;
}

//Найти группу по имени
CharactersGroups::Group *CharactersGroups::FindGroup(const char *name)
{
    const auto gi = FindGroupIndex(name);
    if (gi < 0)
        return nullptr;
    return groups[gi];
}

//Найти группу по имени
inline long CharactersGroups::FindGroupIndex(const char *name)
{
    if (!name)
        return -1;
    const auto l = String::GetLen(name);
    const auto h = String::GetHash(name);

    //Ищем среди зарегистрированных
    for (long i = 0; i < numGroups; i++)
    {
        if (groups[i]->name.Cmp(name, l, h))
            return i;
    }
    return -1;
}

//Найти отношение групп
CharactersGroups::Relation &CharactersGroups::FindRelation(MESSAGE &message, bool *selfgroup)
{
    char grpName1[128];
    message.String(sizeof(grpName1), grpName1);
    grpName1[sizeof(grpName1) - 1] = 0;
    char grpName2[128];
    message.String(sizeof(grpName2), grpName2);
    grpName2[sizeof(grpName2) - 1] = 0;
    return FindRelation(grpName1, grpName2, selfgroup);
}

//Найти отношение групп
inline CharactersGroups::Relation &CharactersGroups::FindRelation(const char *name1, const char *name2, bool *selfgroup)
{
    auto g1 = FindGroupIndex(name1);
    if (g1 < 0)
    {
        RegistryGroup(name1);
        g1 = FindGroupIndex(name1);
        Assert(g1 >= 0);
    }
    auto g2 = FindGroupIndex(name2);
    if (g2 < 0)
    {
        RegistryGroup(name2);
        g2 = FindGroupIndex(name2);
        Assert(g2 >= 0);
    }
    return FindRelation(g1, g2, selfgroup);
}

//Найти отношение групп
inline CharactersGroups::Relation &CharactersGroups::FindRelation(long g1, long g2, bool *selfgroup)
{
    Assert(g1 >= 0 && g1 < numGroups);
    Assert(g2 >= 0 && g2 < numGroups);
    if (selfgroup)
        *selfgroup = (g1 == g2);
    if (g1 <= g2)
    {
        return groups[g2]->relations[g1];
    }
    return groups[g1]->relations[g2];
}

//Получить индекс группы персонажа
inline long CharactersGroups::GetCharacterGroup(Character *c)
{
    if (!c)
        return -1;
    if (c->groupID >= 0 && c->groupID < numGroups)
    {
        if (_stricmp(c->group, groups[c->groupID]->name) == 0)
        {
            return c->groupID;
        }
    }
    c->groupID = FindGroupIndex(c->group);
    return c->groupID;
}

//Удалить все цели
void CharactersGroups::ClearAllTargets() const
{
    //Обновим списки целей
    for (long i = 0; i < location->supervisor.numCharacters; i++)
    {
        location->supervisor.character[i].c->numTargets = 0;
    }
}

//Сохранить даные в объект
void CharactersGroups::SaveData()
{
    //Корневой атрибут для сохранения данных
    if (!AttributesPointer)
    {
        api->Trace("CharactersGroups::SaveData -> no attributes");
        return;
    }
    auto *saveData = AttributesPointer->FindAClass(AttributesPointer, "savedata");
    if (saveData)
        AttributesPointer->DeleteAttributeClassX(saveData);
    saveData = AttributesPointer->CreateSubAClass(AttributesPointer, "savedata");
    //Сохраняем отношения групп
    for (long i = 0, cnt = 0; i < numGroups; i++)
    {
        for (long j = 0; j < i; j++)
        {
            //Раздел отношений
            char buf[16];
            sprintf_s(buf, "r%.4i", cnt++);
            auto *grp = saveData->CreateSubAClass(saveData, buf);
            //Сохраним параметры групп
            grp->SetAttribute("name1", groups[i]->name.name);
            grp->SetAttributeUseFloat("look1", groups[i]->look);
            grp->SetAttributeUseFloat("hear1", groups[i]->hear);
            grp->SetAttributeUseFloat("say1", groups[i]->say);
            grp->SetAttributeUseDword("prt1", groups[i]->priority);
            grp->SetAttribute("name2", groups[j]->name.name);
            grp->SetAttributeUseFloat("look2", groups[j]->look);
            grp->SetAttributeUseFloat("hear2", groups[j]->hear);
            grp->SetAttributeUseFloat("say2", groups[j]->say);
            grp->SetAttributeUseDword("prt2", groups[j]->priority);
            //Сохраним отношения
            auto &r = FindRelation(i, j);
            grp->SetAttributeUseFloat("alarm", r.alarm);
            grp->SetAttributeUseFloat("alarmdown", r.alarmdown);
            grp->SetAttributeUseFloat("alarmmin", r.alarmmin);
            grp->SetAttributeUseFloat("alarmmax", r.alarmmax);
            grp->SetAttributeUseDword("isactive", r.isActive);
            grp->SetAttributeUseDword("curState", r.curState);
            grp->SetAttributeUseDword("actState", r.actState);
            grp->SetAttributeUseDword("relState", r.relState);
        }
    }
    //Сохраняем параметры персонажей
    /*
    saveData = AttributesPointer->FindAClass(AttributesPointer, "characters");
    if(saveData) AttributesPointer->DeleteAttributeClassX(saveData);
    saveData = AttributesPointer->CreateSubAClass(AttributesPointer, "characters");
    */
}

//Прочитать даные из объект
void CharactersGroups::LoadDataRelations()
{
    //Корневой атрибут для сохранения данных
    if (!AttributesPointer)
    {
        api->Trace("CharactersGroups::LoadDataRelations -> no attributes");
        return;
    }
    auto *saveData = AttributesPointer->FindAClass(AttributesPointer, "savedata");
    if (!saveData)
        return;
    const long numG = saveData->GetAttributesNum();
    for (long i = 0; i < numG; i++)
    {
        auto *grp = saveData->GetAttributeClass(i);
        //Регестрируем первую группу
        const char *gname = grp->GetAttribute("name1");
        if (!gname)
            gname = "";
        const auto g1 = RegistryGroup(gname);
        groups[g1]->look = grp->GetAttributeAsFloat("look1", groups[g1]->look);
        groups[g1]->hear = grp->GetAttributeAsFloat("hear1", groups[g1]->hear);
        groups[g1]->say = grp->GetAttributeAsFloat("say1", groups[g1]->say);
        groups[g1]->priority = grp->GetAttributeAsDword("prt1", groups[g1]->priority);
        //Регестрируем вторую группу
        gname = grp->GetAttribute("name2");
        if (!gname)
            gname = "";
        const auto g2 = RegistryGroup(gname);
        groups[g2]->look = grp->GetAttributeAsFloat("look2", groups[g2]->look);
        groups[g2]->hear = grp->GetAttributeAsFloat("hear2", groups[g2]->hear);
        groups[g2]->say = grp->GetAttributeAsFloat("say2", groups[g2]->say);
        groups[g2]->priority = grp->GetAttributeAsDword("prt2", groups[g2]->priority);
        //Востановим отношения
        Assert(g1 != g2);
        auto &r = FindRelation(g1, g2);
        r.alarm = grp->GetAttributeAsFloat("alarm", r.alarm);
        if (r.alarm < 0.0f)
            r.alarm = 0.0f;
        r.alarmdown = grp->GetAttributeAsFloat("alarmdown", r.alarmdown);
        r.alarmmin = grp->GetAttributeAsFloat("alarmmin", r.alarmmin);
        r.alarmmax = grp->GetAttributeAsFloat("alarmmax", r.alarmmax);
        r.isActive = grp->GetAttributeAsDword("isactive", r.isActive) != 0;
        long curState = grp->GetAttributeAsDword("curState", r.curState);
        long actState = grp->GetAttributeAsDword("actState", r.actState);
        long relState = grp->GetAttributeAsDword("relState", r.relState);
        if (curState <= rs_beginvalue || curState >= rs_endvalue)
        {
            api->Trace("CharactersGroups::LoadDataRelations -> invalide curState value, set this neitral");
            curState = rs_neitral;
        }
        r.curState = static_cast<RelState>(curState);
        if (actState <= rs_beginvalue || actState >= rs_endvalue)
        {
            api->Trace("CharactersGroups::LoadDataRelations -> invalide actState value, set this enemy");
            actState = rs_enemy;
        }
        r.actState = static_cast<RelState>(actState);
        if (relState <= rs_beginvalue || relState >= rs_endvalue)
        {
            api->Trace("CharactersGroups::LoadDataRelations -> invalide relState value, set this neitral");
            relState = rs_neitral;
        }
        r.relState = static_cast<RelState>(relState);
    }
}

//Установить отношения для активных групп
void CharactersGroups::RestoreStates()
{
    for (long i = 0, cnt = 0; i < numGroups; i++)
    {
        for (long j = 0; j < i; j++)
        {
            auto &r = FindRelation(i, j);
            const auto oldState = r.isActive;
            if (r.alarmdown > 0)
                r.alarm = 0.0f;
            if (r.alarm <= r.alarmmin)
                r.isActive = false;
            if (r.alarm >= r.alarmmax)
                r.isActive = true;
            if (oldState != r.isActive)
            {
                if (r.isActive)
                    r.curState = r.actState;
                else
                    r.curState = r.relState;
            }
        }
    }
    RemoveAllInvalidTargets();
}

//Вывести информацию об отношениях
void CharactersGroups::DumpRelations()
{
    //Сохраняем отношения групп
    for (long i = 0; i < numGroups; i++)
    {
        for (long j = 0; j < i; j++)
        {
            api->Trace("\"%s\" <-> \"%s\"", groups[i]->name.name, groups[j]->name.name);
            //Сохраним отношения
            auto &r = FindRelation(i, j);
            api->Trace("alarm: %f", r.alarm);
            api->Trace("alarmdown: %f", r.alarmdown);
            api->Trace("alarmmin: %f", r.alarmmin);
            api->Trace("alarmmax: %f", r.alarmmax);
            api->Trace("isActive: %s", r.isActive ? "true" : "false");
            api->Trace("curState: \"%s\"", GetTextState(r.curState));
            api->Trace("actState: \"%s\"", GetTextState(r.actState));
            api->Trace("relState: \"%s\"", GetTextState(r.relState));
            api->Trace("");
        }
    }
    api->Trace("Groups info:");
    api->Trace("");
    for (long i = 0; i < numGroups; i++)
    {
        api->Trace("name: \"%s\"", groups[i]->name.name);
        api->Trace("    look: %f", groups[i]->look);
        api->Trace("    hear: %f", groups[i]->hear);
        api->Trace("    say: %f", groups[i]->say);
        api->Trace("");
    }
}

//Получить состояние в виде строки
const char *CharactersGroups::GetTextState(RelState state)
{
    switch (state)
    {
    case rs_friend:
        return "friend";
    case rs_neitral:
        return "neitral";
    case rs_enemy:
        return "enemy";
    }
    return "unknow value";
}
