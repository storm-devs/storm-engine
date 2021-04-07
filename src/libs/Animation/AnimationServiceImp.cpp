// ============================================================================================
// Spirenkov Maxim aka Sp-Max Shaman, 2001
// --------------------------------------------------------------------------------------------
// Storm engine v2.00
// --------------------------------------------------------------------------------------------
// AnimationServiceImp
// --------------------------------------------------------------------------------------------
// Animation service for creating AnimationManager objects
// ============================================================================================

#include "AnimationServiceImp.h"

#include "core.h"

#include "AnimationImp.h"
#include "an_file.h"
#include "vfile_service.h"

//============================================================================================

// Unused animation unload time
#define ASRV_DOWNTIME 1
// Longest time span supplied by the AnimationManager
#define ASRV_MAXDLTTIME 50

// Paths
#define ASKW_PATH_ANI "resource\\animation\\"
#define ASKW_PATH_JFA "resource\\animation\\"

// Keywords
#define ASKW_JFA_FILE "animation" // Skeleton and Animation File
#define ASKW_STIME "start_time"   // Action start time
#define ASKW_ETIME "end_time"     // End time of action
#define ASKW_RATE "speed"         // Playback speed ratio
#define ASKW_TYPE "type"          // Animation type
#define ASKW_LOOP "loop"          // Repeat animation
#define ASKW_EVENT "event"        // Event
#define ASKW_DATA "data"          // User data
#define ASKW_BONE "bone"          // Bone indices used in animation

// Animation type:
#define ASKWAT_NORMAL "normal"       // From start to the end and stop
#define ASKWAT_REVERSE "reverse"     // From end to start and stop
#define ASKWAT_PINGPONG "pingpong"   // From start to end, back and stop
#define ASKWAT_RPINGPONG "rpingpong" // From end to start, back and stop
// Looped animation
#define ASKWAL_TRUE "true"   // Animation looping allowed
#define ASKWAL_FALSE "false" // Animation looping is forbidden
// Event type
#define ASKWAE_ALWAYS "always"   // Always generate
#define ASKWAE_NORMAL "normal"   // On playing the animation forward
#define ASKWAE_REVERSE "reverse" // On playing the animation backward

//============================================================================================

char AnimationServiceImp::key[1024];

// ============================================================================================
// Construction, destruction
// ============================================================================================

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

// Phase for running the animation
uint32_t AnimationServiceImp::RunSection()
{
    return SECTION_REALIZE;
};

// Execution functions
void AnimationServiceImp::RunStart()
{
    if (core.Controls->GetDebugAsyncKeyState(VK_F4))
        return;
    auto dltTime = core.GetDeltaTime();
    if (dltTime > 1000)
        dltTime = 1000;
    // Check all animations
    for (long i = 0; i < ainfo.size(); i++)
        if (ainfo[i])
        {
            ainfo[i]->AddDowntime(dltTime);
            if (ainfo[i]->GetDowntime() >= ASRV_DOWNTIME)
            {
                // Unloading unused animation
                // core.Trace("Download animation %s", ainfo[i]->GetName());
                delete ainfo[i];
                ainfo[i] = nullptr;
            }
        }
    // execute all animations
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

// Create animation for the model, delete using "delete"
Animation *AnimationServiceImp::CreateAnimation(const char *animationName)
{
    // looking for animation, load if not found
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
    // The animation is loaded, creating an animation manager
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

// Remove animation (called from destructor)
void AnimationServiceImp::DeleteAnimation(AnimationImp *ani)
{
    Assert(ani);
    Assert(ani->GetThisID() >= 0 || ani->GetThisID() < animations.size());
    Assert(animations[ani->GetThisID()] == ani);
    animations[ani->GetThisID()] = nullptr;
}

// Event
void AnimationServiceImp::Event(const char *eventName)
{
    // Sending a message to the system
    core.Trace("Called function <void AnimationServiceImp::Event(%s)>, please make it.", eventName);
}

// load animation
long AnimationServiceImp::LoadAnimation(const char *animationName)
{
    // Form the file name
    static char path[MAX_PATH];
    strcpy_s(path, ASKW_PATH_ANI);
    strcat_s(path, animationName);
    strcat_s(path, ".ani");
    // Open the ini file describing the animation
    auto ani = fio->OpenIniFile(path);
    if (!ani)
    {
        core.Trace("Cannot open animation file %s", path);
        return -1;
    }
    // Get the name of the jfa file with the skeleton
    strcpy_s(path, ASKW_PATH_JFA);
    const size_t l = strlen(path);
    if (!ani->ReadString(nullptr, ASKW_JFA_FILE, path + l, MAX_PATH - l - 1, nullptr))
    {
        core.Trace("Incorrect key \"%s\" in animation file %s.ani", ASKW_JFA_FILE, animationName);
        return -1;
    }
    // Animation descriptor
    auto *info = new AnimationInfo(animationName);
    // read the bones
    if (!LoadAN(path, info))
    {
        delete info;
        core.Trace("Animation file %s is damaged!", path);
        return -1;
    }
    // Global user data
    LoadUserData(ani.get(), nullptr, info->GetUserData(), animationName);
    // read actions
    for (auto isHaveSection = ani->GetSectionName(path, 63); isHaveSection;
         isHaveSection = ani->GetSectionNameNext(path, 63))
    {
        // Action handling
        if (path[0] == 0 || strlen(path) >= 64)
        {
            core.Trace("Incorrect name action [%s] of animation file %s.ani", path, animationName);
            continue;
        }
        // Reading the times
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
        // Add an action
        auto *aci = info->AddAction(path, stime, etime);
        if (aci == nullptr)
        {
            core.Trace("Warning! Action [%s] of animation file %s.ani is repeated, skip it", path, animationName);
            continue;
        }
        // Playback speed ratio
        const auto rate = ani->GetFloat(path, ASKW_RATE, 1.0f);
        aci->SetRate(rate);
        // Animation type
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
        // Looped animation
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
        // Events
        if (ani->ReadString(path, ASKW_EVENT, key, 256, ""))
        {
            do
            {
                key[256] = 0;
                memcpy(key + 257, key, 257);
                // The beginning of the name
                if (key[0] != '"')
                {
                    core.Trace("Incorrect %s <%s> in action [%s] of animation file %s.ani\nFirst symbol is not '\"'\n",
                               ASKW_EVENT, key + 257, path, animationName);
                    continue;
                }
                // End of name
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
                // determine the time
                // First digit
                for (; key[p] && (key[p] < '0' || key[p] > '9'); p++)
                    ;
                if (!key[p])
                {
                    core.Trace("Incorrect %s <%s> in action [%s] of animation file %s.ani\nNo found time\n", ASKW_EVENT,
                               key + 257, path, animationName);
                    continue;
                }
                auto *em = key + p;
                // Looking for the end of the number
                for (; key[p] >= '0' && key[p] <= '9'; p++)
                    ;
                float tm = 0;
                if (key[p] != '%')
                {
                    // The absolute time value
                    if (key[p])
                        key[p++] = 0;
                    tm = static_cast<float>(atof(em));
                }
                else
                {
                    // Relative time value
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
                // Reading the event type
                auto ev = eae_normal;
                if (key[p])
                {
                    // Looking for a start
                    for (p++; key[p]; p++)
                        if ((key[p] >= 'A' && key[p] <= 'Z') || (key[p] >= 'a' && key[p] <= 'z'))
                            break;
                    em = key + p;
                    // Looking for an ending
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
                // Add an event
                if (!aci->AddEvent(key + 1, tm, ev))
                {
                    core.Trace("Warning: Incorrect %s <%s> in action [%s] of animation file %s.ani,\nvery many events "
                               "-> ignory it\n",
                               ASKW_EVENT, key + 257, path, animationName);
                }
            } while (ani->ReadStringNext(path, ASKW_EVENT, key, 256));
        }
        // Bones

        // User data
        LoadUserData(ani.get(), path, aci->GetUserData(), animationName);
    }
    // Looking for a free pointer
    long i;
    for (i = 0; i < ainfo.size(); i++)
        if (!ainfo[i])
            break;
    // expand the array if not found
    if (i == ainfo.size())
        ainfo.emplace_back(nullptr);

    ainfo[i] = info;
    return i;
}

// Load user data from the current section
void AnimationServiceImp::LoadUserData(INIFILE *ani, const char *sectionName,
                                       std::unordered_map<std::string, std::string> &data, const char *animationName)
{
    if (ani->ReadString(sectionName, ASKW_DATA, key, 1023, ""))
    {
        do
        {
            key[1023] = 0;
            // Looking for data name
            // The beginning of the name
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
            // End of name
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
            // Checking for data availability
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
            // looking for a string with data
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
            // Looking for the end of the data string
            auto *const uds = key + ++p;
            for (; key[p] && key[p] != '"'; p++)
                ;
            key[p] = 0;
            // Add data
            // core.Trace("Add user data \"%s\", \"%s\" of \"%s\"", key + 1, uds, sectionName);
            data[key + 1] = uds;
        } while (ani->ReadStringNext((char *)sectionName, ASKW_DATA, key, 1023));
    }
}

// load AN
bool AnimationServiceImp::LoadAN(const char *fname, AnimationInfo *info)
{
    std::fstream fileS;
    try
    {
        fileS = fio->_CreateFile(fname, std::ios::binary | std::ios::in);
        if (!fileS.is_open())
        {
            core.Trace("Cannot open file: %s", fname);
            return false;
        }
        // Reading the file header
        ANFILE::HEADER header;
        if (!fio->_ReadFile(fileS, &header, sizeof(ANFILE::HEADER)) || header.nFrames <= 0 || header.nJoints <= 0 ||
            header.framesPerSec < 0.0f || header.framesPerSec > 1000.0f)
        {
            core.Trace("Incorrect file header in animation file: %s", fname);
            fio->_CloseFile(fileS);
            return false;
        }
        // Set animation time
        info->SetNumFrames(header.nFrames);
        // Set the animation speed
        info->SetFPS(header.framesPerSec);
        // Create the required number of bones
        info->CreateBones(header.nJoints);
        // Setting parents
        auto *const prntIndeces = new long[header.nJoints];
        if (!fio->_ReadFile(fileS, prntIndeces, header.nJoints * sizeof(long)))
        {
            core.Trace("Incorrect parent indeces block in animation file: %s", fname);
            delete[] prntIndeces;
            fio->_CloseFile(fileS);
            return false;
        }
        for (long i = 1; i < header.nJoints; i++)
        {
            Assert(prntIndeces[i] >= 0 || prntIndeces[i] < header.nJoints);
            Assert(prntIndeces[i] != i);
            info->GetBone(i).SetParent(&info->GetBone(prntIndeces[i]));
        }
        delete[] prntIndeces;
        // Starting positions of bones
        auto *vrt = new CVECTOR[header.nJoints];
        if (!fio->_ReadFile(fileS, vrt, header.nJoints * sizeof(CVECTOR)))
        {
            core.Trace("Incorrect start joints position block block in animation file: %s", fname);
            delete[] vrt;
            fio->_CloseFile(fileS);
            return false;
        }
        for (long i = 0; i < header.nJoints; i++)
        {
            info->GetBone(i).SetNumFrames(header.nFrames, vrt[i], i == 0);
        }
        delete[] vrt;

        // Root bone positions
        vrt = new CVECTOR[header.nFrames];
        if (!fio->_ReadFile(fileS, vrt, header.nFrames * sizeof(CVECTOR)))
        {
            core.Trace("Incorrect root joint position block block in animation file: %s", fname);
            delete[] vrt;
            fio->_CloseFile(fileS);
            return false;
        }
        info->GetBone(0).SetPositions(vrt, header.nFrames);
        delete[] vrt;

        // Angles
        auto *ang = new D3DXQUATERNION[header.nFrames];
        for (long i = 0; i < header.nJoints; i++)
        {
            if (!fio->_ReadFile(fileS, ang, header.nFrames * sizeof(*ang)))
            {
                core.Trace("Incorrect joint angle block (%i) block in animation file: %s", i, fname);
                fio->_CloseFile(fileS);
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

        // Close the file
        fio->_CloseFile(fileS);
        return true;
    }
    catch (...)
    {
        if (fileS.is_open())
        {
            fio->_CloseFile(fileS);
        }
        core.Trace("Error reading animation file: %s", fname);
        return false;
    }
}
