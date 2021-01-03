//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//	Storm engine v2.00
//--------------------------------------------------------------------------------------------
//	AnimationServiceImp
//--------------------------------------------------------------------------------------------
//	Сервис анимации, позволяющий создавать объекты AnimationManager
//============================================================================================

#include "AnimationServiceImp.h"

#include "core.h"

#include "AnimationImp.h"
#include "an_file.h"
#include "vfile_service.h"

//============================================================================================

//Время выгрузки неиспользуемой анимации
#define ASRV_DOWNTIME 1
//Наибольший временной отрезок, подаваемый в AnimationManager
#define ASRV_MAXDLTTIME 50

//Пути
#define ASKW_PATH_ANI "resource\\animation\\"
#define ASKW_PATH_JFA "resource\\animation\\"

//Ключевые слова
#define ASKW_JFA_FILE "animation" //Файл со скелетом и анимацией
#define ASKW_STIME "start_time"   //Начальное время действия
#define ASKW_ETIME "end_time"     //Конечное время действия
#define ASKW_RATE "speed"         //Коэфициент скорости воспроизведения
#define ASKW_TYPE "type"          //Тип анимации
#define ASKW_LOOP "loop"          //Повторять анимацию
#define ASKW_EVENT "event"        //Событие
#define ASKW_DATA "data"          //Пользовательские данные
#define ASKW_BONE "bone"          //Индексы костей используемые при анимации

//Тип анимации:
#define ASKWAT_NORMAL "normal"       //От старта до конца и остановиться
#define ASKWAT_REVERSE "reverse"     //От конца до старта и остановиться
#define ASKWAT_PINGPONG "pingpong"   //От старта до конца, обратно и остановиться
#define ASKWAT_RPINGPONG "rpingpong" //От конца до старта, обратно и остановиться
//Зацикленность анимации
#define ASKWAL_TRUE "true"   //Разрешенна зацикленность анимации
#define ASKWAL_FALSE "false" //Запрещенна зацикленность анимации
//Тип события
#define ASKWAE_ALWAYS "always"   //Всегда генерить
#define ASKWAE_NORMAL "normal"   //При прямом прокручивании анимации
#define ASKWAE_REVERSE "reverse" //При обратном прокручивании анимации

//============================================================================================

char AnimationServiceImp::key[1024];

//============================================================================================
//Конструирование, деструктурирование
//============================================================================================

AnimationServiceImp::AnimationServiceImp()
{
    AnimationImp::SetAnimationService(this);
}

AnimationServiceImp::~AnimationServiceImp()
{
    for (const auto &animation : animations)
    {
        if (animation)
        {
            core.Trace("No release Animation pnt:0x%x for %s.ani", animation, animation->GetAnimationInfo()->GetName());
            delete animation;
        }
    }

    for (const auto &info : ainfo)
        delete info;
}

//============================================================================================

//Место исполнения
uint32_t AnimationServiceImp::RunSection()
{
    return SECTION_REALIZE;
};

//Функции исполнения
void AnimationServiceImp::RunStart()
{
    if (core.Controls->GetDebugAsyncKeyState(VK_F4))
        return;
    auto dltTime = core.GetDeltaTime();
    if (dltTime > 1000)
        dltTime = 1000;
    //Просмотрим все анимации
    for (long i = 0; i < ainfo.size(); i++)
        if (ainfo[i])
        {
            ainfo[i]->AddDowntime(dltTime);
            if (ainfo[i]->GetDowntime() >= ASRV_DOWNTIME)
            {
                //Выгружаем никем не используемую анимацию
                // core.Trace("Download animation %s", ainfo[i]->GetName());
                delete ainfo[i];
                ainfo[i] = nullptr;
            }
        }
    //Исполним все анимации
    for (auto i = 0; i < animations.size(); i++)
        if (animations[i])
        {
            long dt;
            for (dt = dltTime; dt > ASRV_MAXDLTTIME; dt -= ASRV_MAXDLTTIME)
                animations[i]->Execute(ASRV_MAXDLTTIME);
            if (dt > 0)
                animations[i]->Execute(dt);
            // core.Trace("Animation: 0x%.8x Time: %f", animation[i], animation[i]->Player(0).GetPosition());
        }
}

void AnimationServiceImp::RunEnd()
{
}

//Создать анимацию для модели, удалять через delete
Animation *AnimationServiceImp::CreateAnimation(const char *animationName)
{
    //Ищем анимацию, если нет, то загружаем
    long i;
    for (i = 0; i < ainfo.size(); i++)
        if (ainfo[i])
        {
            if (ainfo[i][0] == animationName)
                break;
        }
    if (i == ainfo.size())
    {
        i = LoadAnimation(animationName);
        if (i < 0)
            return nullptr;
    }
    const auto aniIndex = i;
    //Анимация загружена, создаём менеджер анимации
    for (i = 0; i < animations.size(); i++)
        if (!animations[i])
            break;
    if (i == animations.size())
    {
        animations.emplace_back(nullptr);
    }
    animations[i] = new AnimationImp(i, ainfo[aniIndex]);
    return animations[i];
}

//Удалить анимацию (вызывается из деструктора)
void AnimationServiceImp::DeleteAnimation(AnimationImp *ani)
{
    Assert(ani);
    Assert(ani->GetThisID() >= 0 || ani->GetThisID() < animations.size());
    Assert(animations[ani->GetThisID()] == ani);
    animations[ani->GetThisID()] = nullptr;
}

//Событие
void AnimationServiceImp::Event(const char *eventName)
{
    //Отправка сообщения системе
    core.Trace("Called function <void AnimationServiceImp::Event(%s)>, please make it.", eventName);
}

//Загрузить анимацию
long AnimationServiceImp::LoadAnimation(const char *animationName)
{
    //Формируем имя файла
    static char path[MAX_PATH];
    strcpy_s(path, ASKW_PATH_ANI);
    strcat_s(path, animationName);
    strcat_s(path, ".ani");
    //Открываем ini файл, описывающий анимацию
    auto *ani = fio->OpenIniFile(path);
    if (!ani)
    {
        core.Trace("Cannot open animation file %s", path);
        return -1;
    }
    //Получаем имя jfa файла со скелетом
    strcpy_s(path, ASKW_PATH_JFA);
    const size_t l = strlen(path);
    if (!ani->ReadString(nullptr, ASKW_JFA_FILE, path + l, MAX_PATH - l - 1, nullptr))
    {
        core.Trace("Incorrect key \"%s\" in animation file %s.ani", ASKW_JFA_FILE, animationName);
        delete ani;
        return -1;
    }
    //Описатель онимации
    auto *info = new AnimationInfo(animationName);
    //Зачитаем кости
    if (!LoadAN(path, info))
    {
        delete ani;
        delete info;
        core.Trace("Animation file %s is damaged!", path);
        return -1;
    }
    //Глобальные пользовательские данные
    LoadUserData(ani, nullptr, info->GetUserData(), animationName);
    //Зачитаем действия
    for (auto isHaveSection = ani->GetSectionName(path, 63); isHaveSection;
         isHaveSection = ani->GetSectionNameNext(path, 63))
    {
        //Обработка действия
        if (path[0] == 0 || strlen(path) >= 64)
        {
            core.Trace("Incorrect name action [%s] of animation file %s.ani", path, animationName);
            continue;
        }
        //Зачитываем времена
        const auto stime = ani->GetLong(path, ASKW_STIME, -1);
        if (stime < 0)
        {
            core.Trace("Incorrect %s in action [%s] of animation file %s.ani", ASKW_STIME, path, animationName);
            continue;
        }
        const auto etime = ani->GetLong(path, ASKW_ETIME, -1);
        if (etime < 0)
        {
            core.Trace("Incorrect %s in action [%s] of animation file %s.ani", ASKW_ETIME, path, animationName);
            continue;
        }
        //Добавляем действие
        auto *aci = info->AddAction(path, stime, etime);
        if (aci == nullptr)
        {
            core.Trace("Warning! Action [%s] of animation file %s.ani is repeated, skip it", path, animationName);
            continue;
        }
        //Коэфициент скорости воспроизведения
        const auto rate = ani->GetFloat(path, ASKW_RATE, 1.0f);
        aci->SetRate(rate);
        //Тип анимации
        auto type = at_normal;
        if (ani->ReadString(path, ASKW_TYPE, key, 256, ASKWAT_NORMAL))
        {
            if (_stricmp(key, ASKWAT_NORMAL) == 0)
                type = at_normal;
            else if (_stricmp(key, ASKWAT_REVERSE) == 0)
                type = at_reverse;
            else if (_stricmp(key, ASKWAT_PINGPONG) == 0)
                type = at_pingpong;
            else if (_stricmp(key, ASKWAT_RPINGPONG) == 0)
                type = at_rpingpong;
            else
            {
                core.Trace("Incorrect %s in action [%s] of animation file %s.ani\nNo set %s, set type is %s\n",
                           ASKW_TYPE, path, animationName, key, ASKWAT_NORMAL);
            }
        }
        aci->SetAnimationType(type);
        //Зацикленность анимации
        auto isLoop = true;
        if (ani->ReadString(path, ASKW_LOOP, key, 256, "false"))
        {
            if (_stricmp(key, ASKWAL_TRUE) == 0)
                isLoop = true;
            else if (_stricmp(key, ASKWAL_FALSE) == 0)
                isLoop = false;
            else
            {
                core.Trace("Incorrect %s in action [%s] of animation file %s.ani\nThis parameter (%s) use is default "
                           "value %s\n",
                           ASKW_LOOP, path, animationName, key, ASKWAL_FALSE);
            }
        }
        aci->SetLoop(isLoop);
        //События
        if (ani->ReadString(path, ASKW_EVENT, key, 256, ""))
        {
            do
            {
                key[256] = 0;
                memcpy(key + 257, key, 257);
                //Начало имени
                if (key[0] != '"')
                {
                    core.Trace("Incorrect %s <%s> in action [%s] of animation file %s.ani\nFirst symbol is not '\"'\n",
                               ASKW_EVENT, key + 257, path, animationName);
                    continue;
                }
                //Конец имени
                long p;
                for (p = 1; key[p] && key[p] != '"'; p++)
                    ;
                if (!key[p])
                {
                    core.Trace(
                        "Incorrect %s <%s> in action [%s] of animation file %s.ani\nNot found closed symbol '\"'\n",
                        ASKW_EVENT, key + 257, path, animationName);
                    continue;
                }
                if (p == 1)
                {
                    core.Trace("Incorrect %s <%s> in action [%s] of animation file %s.ani\nName have zero lenght\n",
                               ASKW_EVENT, key + 257, path, animationName);
                    continue;
                }
                if (p > 65)
                {
                    core.Trace(
                        "Incorrect %s <%s> in action [%s] of animation file %s.ani\nName have big length (max 63)\n",
                        ASKW_EVENT, key + 257, path, animationName);
                    continue;
                }
                key[p++] = 0;
                //Определяем время
                //Первая цифра
                for (; key[p] && (key[p] < '0' || key[p] > '9'); p++)
                    ;
                if (!key[p])
                {
                    core.Trace("Incorrect %s <%s> in action [%s] of animation file %s.ani\nNo found time\n", ASKW_EVENT,
                               key + 257, path, animationName);
                    continue;
                }
                auto *em = key + p;
                //Ищем окончание числа
                for (; key[p] >= '0' && key[p] <= '9'; p++)
                    ;
                float tm = 0;
                if (key[p] != '%')
                {
                    //Абслютное значение времени
                    if (key[p])
                        key[p++] = 0;
                    tm = static_cast<float>(atof(em));
                }
                else
                {
                    //Относительное значение времени
                    key[p++] = 0;
                    tm = static_cast<float>(atof(em));
                    if (tm < 0)
                        tm = 0;
                    if (tm > 100)
                        tm = 100;
                    tm = stime + 0.01f * tm * (etime - stime);
                }
                if (tm < stime)
                    tm = static_cast<float>(stime);
                if (tm > etime)
                    tm = static_cast<float>(etime);
                //Зачитываем тип события
                auto ev = eae_normal;
                if (key[p])
                {
                    //Ищем начало
                    for (p++; key[p]; p++)
                        if ((key[p] >= 'A' && key[p] <= 'Z') || (key[p] >= 'a' && key[p] <= 'z'))
                            break;
                    em = key + p;
                    //Ищем окончание
                    for (p++; key[p]; p++)
                        if (!(key[p] >= 'A' && key[p] <= 'Z') && !(key[p] >= 'a' && key[p] <= 'z'))
                            break;
                    key[p] = 0;
                    if (em[0] == 0)
                    {
                    }
                    else if (_stricmp(em, ASKWAE_ALWAYS) == 0)
                    {
                        ev = eae_always;
                    }
                    else if (_stricmp(em, ASKWAE_NORMAL) == 0)
                    {
                        ev = eae_normal;
                    }
                    else if (_stricmp(em, ASKWAE_REVERSE) == 0)
                    {
                        ev = eae_reverse;
                    }
                    else
                    {
                        core.Trace("Warning: Incorrect %s <%s> in action [%s] of animation file %s.ani,\nunknow event "
                                   "type <%s> -> set is default value\n",
                                   ASKW_EVENT, key + 257, path, animationName, em);
                    }
                }
                // core.Trace("Add event %s, time = %f to action %s", key + 1, (tm - stime)/float(etime - stime), path);
                //Добавляем событие
                if (!aci->AddEvent(key + 1, tm, ev))
                {
                    core.Trace("Warning: Incorrect %s <%s> in action [%s] of animation file %s.ani,\nvery many events "
                               "-> ignory it\n",
                               ASKW_EVENT, key + 257, path, animationName);
                }
            } while (ani->ReadStringNext(path, ASKW_EVENT, key, 256));
        }
        //Кости

        //Пользовательские данные
        LoadUserData(ani, path, aci->GetUserData(), animationName);
    }
    //Закроем ini файл
    delete ani;
    //Ищем свободный указатель
    long i;
    for (i = 0; i < ainfo.size(); i++)
        if (!ainfo[i])
            break;
    //Если нет такого, расширем массив
    if (i == ainfo.size())
        ainfo.emplace_back(nullptr);

    ainfo[i] = info;
    return i;
}

//Загрузить из текущей секции пользовательские данные
void AnimationServiceImp::LoadUserData(INIFILE *ani, const char *sectionName,
                                       std::unordered_map<std::string, std::string> &data, const char *animationName)
{
    if (ani->ReadString((char *)sectionName, ASKW_DATA, key, 1023, ""))
    {
        do
        {
            key[1023] = 0;
            //Ищем имя данных
            //Начало имени
            if (key[0] != '"')
            {
                if (sectionName)
                    core.Trace("Incorrect %s in action [%s] of animation file %s.ani\nFirst symbol is not '\"'",
                               ASKW_DATA, sectionName, animationName);
                else
                    core.Trace("Incorrect %s in global data of animation file %s.ani\nFirst symbol is not '\"'",
                               ASKW_DATA, animationName);
                continue;
            }
            //Конец имени
            long p;
            for (p = 1; key[p] && key[p] != '"'; p++)
                ;
            if (!key[p])
            {
                if (sectionName)
                    core.Trace("Incorrect %s in action [%s] of animation file %s.ani\nNot found closed symbol '\"' for "
                               "data name",
                               ASKW_DATA, sectionName, animationName);
                else
                    core.Trace("Incorrect %s in global data of animation file %s.ani\nNot found closed symbol '\"' for "
                               "data name",
                               ASKW_DATA, animationName);
                continue;
            }
            if (p == 1)
            {
                if (sectionName)
                    core.Trace("Incorrect %s in action [%s] of animation file %s.ani\nName have zero lenght", ASKW_DATA,
                               sectionName, animationName);
                else
                    core.Trace("Incorrect %s in global data of animation file %s.ani\nName have zero lenght", ASKW_DATA,
                               animationName);
                continue;
            }
            key[p++] = 0;
            //Проверяем наличие данных
            if (data.count(key + 1))
            {
                if (sectionName)
                    core.Trace("Incorrect %s in action [%s] of animation file %s.ani\nUser data repeated", ASKW_DATA,
                               sectionName, animationName);
                else
                    core.Trace("Incorrect %s in global data of animation file %s.ani\nUser data repeated", ASKW_DATA,
                               animationName);

                continue;
            }
            //Ищем строку с данными
            for (; key[p] && key[p] != '"'; p++)
                ;
            if (!key[p])
            {
                if (sectionName)
                    core.Trace("Incorrect %s in action [%s] of animation file %s.ani\nNo data string", ASKW_DATA,
                               sectionName, animationName);
                else
                    core.Trace("Incorrect %s in global data of animation file %s.ani\nNo data string", ASKW_DATA,
                               animationName);
                continue;
            }
            //Ищем окончание строки данных
            auto *const uds = key + ++p;
            for (; key[p] && key[p] != '"'; p++)
                ;
            key[p] = 0;
            //Добавляем данные
            // core.Trace("Add user data \"%s\", \"%s\" of \"%s\"", key + 1, uds, sectionName);
            data[key + 1] = uds;
        } while (ani->ReadStringNext((char *)sectionName, ASKW_DATA, key, 1023));
    }
}

//Загрузить AN
bool AnimationServiceImp::LoadAN(const char *fname, AnimationInfo *info)
{
    auto *fl = INVALID_HANDLE_VALUE;
    try
    {
        fl = fio->_CreateFile(fname, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING);
        if (fl == INVALID_HANDLE_VALUE)
        {
            core.Trace("Cannot open file: %s", fname);
            return false;
        }
        //Читаем заголовок файла
        ANFILE::HEADER header;
        if (!fio->_ReadFile(fl, &header, sizeof(ANFILE::HEADER), nullptr) || header.nFrames <= 0 ||
            header.nJoints <= 0 || header.framesPerSec < 0.0f || header.framesPerSec > 1000.0f)
        {
            core.Trace("Incorrect file header in animation file: %s", fname);
            fio->_CloseHandle(fl);
            return false;
        }
        //Установим время анимации
        info->SetNumFrames(header.nFrames);
        //Установим скорость анимации
        info->SetFPS(header.framesPerSec);
        //Заводим нужное число костей
        info->CreateBones(header.nJoints);
        //Устанавливаем родителей
        auto *const prntIndeces = new long[header.nJoints];
        if (!fio->_ReadFile(fl, prntIndeces, header.nJoints * sizeof(long), nullptr))
        {
            core.Trace("Incorrect parent indeces block in animation file: %s", fname);
            delete[] prntIndeces;
            fio->_CloseHandle(fl);
            return false;
        }
        for (long i = 1; i < header.nJoints; i++)
        {
            Assert(prntIndeces[i] >= 0 || prntIndeces[i] < header.nJoints);
            Assert(prntIndeces[i] != i);
            info->GetBone(i).SetParent(&info->GetBone(prntIndeces[i]));
        }
        delete[] prntIndeces;
        //Стартовые позиции костей
        auto *vrt = new CVECTOR[header.nJoints];
        if (!fio->_ReadFile(fl, vrt, header.nJoints * sizeof(CVECTOR), nullptr))
        {
            core.Trace("Incorrect start joints position block block in animation file: %s", fname);
            delete[] vrt;
            fio->_CloseHandle(fl);
            return false;
        }
        for (long i = 0; i < header.nJoints; i++)
        {
            info->GetBone(i).SetNumFrames(header.nFrames, vrt[i], i == 0);
        }
        delete[] vrt;

        //Позиции рутовой кости
        vrt = new CVECTOR[header.nFrames];
        if (!fio->_ReadFile(fl, vrt, header.nFrames * sizeof(CVECTOR), nullptr))
        {
            core.Trace("Incorrect root joint position block block in animation file: %s", fname);
            delete[] vrt;
            fio->_CloseHandle(fl);
            return false;
        }
        info->GetBone(0).SetPositions(vrt, header.nFrames);
        delete[] vrt;

        //Углы
        auto *ang = new D3DXQUATERNION[header.nFrames];
        for (long i = 0; i < header.nJoints; i++)
        {
            if (!fio->_ReadFile(fl, ang, header.nFrames * sizeof(*ang), nullptr))
            {
                core.Trace("Incorrect joint angle block (%i) block in animation file: %s", i, fname);
                fio->_CloseHandle(fl);
                return false;
            }
            info->GetBone(i).SetAngles(ang, header.nFrames);
        }
        delete[] ang;

        //-----------------------------------------------
        for (long i = 0; i < header.nJoints; i++)
        {
            info->GetBone(i).BuildStartMatrix();
        }
        for (long i = 0; i < header.nJoints; i++)
        {
            info->GetBone(i).start.Transposition();
        }
        //-----------------------------------------------

        //Закроем файл
        fio->_CloseHandle(fl);
        return true;
    }
    catch (...)
    {
        if (fl != INVALID_HANDLE_VALUE)
            fio->_CloseHandle(fl);
        core.Trace("Error reading animation file: %s", fname);
        return false;
    }
}
