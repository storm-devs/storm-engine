//============================================================================================
//    Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    Supervisor
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#include "supervisor.h"
#include "character.h"
#include "entity.h"
#include "locator_array.h"
#include "core.h"

// ============================================================================================
// Construction, destruction
// ============================================================================================

Supervisor::Supervisor()
    : isDelete(false)
{
    time = 0.0f;
    waveTime = 0.0f;
    curUpdate = 0;
    player = nullptr;
}

Supervisor::~Supervisor()
{
    isDelete = true;
    for (size_t i = 0; i < character.size(); i++)
    {
        character[i].c->AlreadySTORM_DELETE();
        EntityManager::EraseEntity(character[i].c->GetId());
    }
}

// Add character to location
void Supervisor::AddCharacter(Character *ch)
{
    Assert(ch);
    character.emplace_back(ch, time);
    colchr.resize(character.size() * character.size());
}

// Remove character from location
void Supervisor::DelCharacter(Character *ch)
{
    for (size_t i = 0; i < character.size(); i++)
        if (character[i].c == ch)
        {
            character[i] = character.back();
            character.pop_back();
            colchr.resize(character.size() * character.size());
            return;
        }
}

void Supervisor::Update(float dltTime)
{
    // If there are no characters, do nothing
    if (character.empty())
        return;
    // Moving characters
    for (size_t i = 0; i < character.size(); i++)
    {
        character[i].c->Move(dltTime);
        character[i].c->colMove = 0.0f;
        character[i].c->isCollision = false;
    }
    // calculate the distances, and determine the interacting characters
    int32_t chr = 0;
    size_t i;
    for (i = 0; i < character.size() - 1; i++)
    {
        // skip the dead
        if (character[i].c->liveValue < 0 || character[i].c->deadName)
            continue;
        character[i].c->startColCharacter = chr;
        auto curPos(character[i].c->curPos);
        const auto radius = character[i].c->radius;
        for (auto j = i + 1; j < character.size(); j++)
        {
            // skip the dead
            auto *ci = character[i].c;
            auto *cj = character[j].c;
            if (cj->liveValue < 0 || cj->deadName)
                continue;
            // Distance between characters
            auto d = ~(curPos - cj->curPos);
            // Character interaction distance
            auto r = radius + cj->radius;
            const auto rr = r * 4.0f;
            if (d > rr * rr)
                continue;
            // Keeping character
            colchr[chr].c = cj;
            colchr[chr].d = sqrtf(d);
            colchr[chr].maxD = rr;
            chr++;
            // Checking the intersection of characters in height
            if (cj->curPos.y > ci->curPos.y + ci->height)
                continue;
            if (ci->curPos.y > cj->curPos.y + cj->height)
                continue;
            // Pushing the characters
            auto dx = curPos.x - cj->curPos.x;
            auto dz = curPos.z - cj->curPos.z;
            d = dx * dx + dz * dz;
            r *= 0.5f;
            if (d >= r * r)
                continue;
            if (d)
            {
                d = sqrtf(d);
                d = (r - d) / d;
                dx *= d;
                dz *= d;
                ci->isCollision = true;
                cj->isCollision = true;
                auto moveI = ci->IsMove();
                if ((~ci->impulse) > 0.0001f && !moveI)
                {
                    moveI = ((ci->impulse.x * dx + ci->impulse.z * dz) < 0.0f);
                }
                auto moveJ = cj->IsMove();
                if ((~cj->impulse) > 0.0001f && !moveJ)
                {
                    moveJ = ((cj->impulse.x * dx + cj->impulse.z * dz) > 0.0f);
                }
                if (ci->IsFight())
                {
                    if (moveI == moveJ)
                    {
                        ci->curPos.x += dx * 0.5f;
                        ci->curPos.z += dz * 0.5f;
                        cj->curPos.x -= dx * 0.5f;
                        cj->curPos.z -= dz * 0.5f;
                    }
                    else
                    {
                        if (moveI)
                        {
                            ci->curPos.x += dx * 0.999f;
                            ci->curPos.z += dz * 0.999f;
                            cj->curPos.x -= dx * 0.001f;
                            cj->curPos.z -= dz * 0.001f;
                        }
                        else
                        {
                            ci->curPos.x += dx * 0.001f;
                            ci->curPos.z += dz * 0.001f;
                            cj->curPos.x -= dx * 0.999f;
                            cj->curPos.z -= dz * 0.999f;
                        }
                    }
                }
                else if (moveI == moveJ)
                {
                    ci->curPos.x += dx * 0.5f;
                    ci->curPos.z += dz * 0.5f;
                    cj->curPos.x -= dx * 0.5f;
                    cj->curPos.z -= dz * 0.5f;
                }
                else
                {
                    if (moveI)
                    {
                        ci->curPos.x += dx * 0.9f;
                        ci->curPos.z += dz * 0.9f;
                        cj->curPos.x -= dx * 0.1f;
                        cj->curPos.z -= dz * 0.1f;
                    }
                    else
                    {
                        ci->curPos.x += dx * 0.1f;
                        ci->curPos.z += dz * 0.1f;
                        cj->curPos.x -= dx * 0.9f;
                        cj->curPos.z -= dz * 0.9f;
                    }
                }
            }
        }
        character[i].c->numColCharacter = chr - character[i].c->startColCharacter;
    }
    character[i].c->numColCharacter = 0;
    // Calculations
    for (i = 0; i < character.size(); i++)
        character[i].c->Calculate(dltTime);
    // Collision of characters and setting new coordinates
    for (i = 0; i < character.size(); i++)
        character[i].c->Update(dltTime);
}

void Supervisor::PreUpdate(float dltTime) const
{
    // Resetting the state of the characters
    for (size_t i = 0; i < character.size(); i++)
        character[i].c->Reset();
    core.Event("CharactersStateUpdate", "f", dltTime);
}

void Supervisor::PostUpdate(float dltTime)
{
    // Character state update
    time += dltTime;
    waveTime += dltTime;
    // Correct the time when the limit is exceeded
    if (time > 10000.0f)
    {
        for (size_t i = 0; i < character.size(); i++)
            character[i].lastTime -= time;
        time -= 10000.0f;
    }
    // If it's not time to update, skip the turn
    if (curUpdate >= character.size())
    {
        if (waveTime < 0.1f)
            return;
        waveTime = 0.0f;
        curUpdate = 0;
    }
    // Executing the current character
    if (!character.empty())
    {
        for (int32_t i = 0; i < 5; i++)
        {
            if (curUpdate >= character.size())
                break;
            const auto dlt = time - character[curUpdate].lastTime;
            character[curUpdate].lastTime = time;
            if (EntityManager::GetEntityPointer(character[curUpdate].c->GetId()))
            {
                core.Event("CharacterUpdate", "if", character[curUpdate].c->GetId(), dlt);
            }
            curUpdate++;
        }
    }
}

// Set loading positions
void Supervisor::SetSavePositions() const
{
    for (size_t i = 0; i < character.size(); i++)
    {
        if (!character[i].c)
            continue;
        character[i].c->SetSavePosition();
    }
}

// Delete positions for loading
void Supervisor::DelSavePositions(bool isTeleport) const
{
    for (size_t i = 0; i < character.size(); i++)
    {
        if (!character[i].c)
            continue;
        character[i].c->DelSavePosition(isTeleport);
    }
}

// Check for free position
bool Supervisor::CheckPosition(float x, float y, float z, Character *c) const
{
    for (size_t i = 0; i < character.size(); i++)
    {
        if (character[i].c == c)
            continue;
        const auto dx = x - character[i].c->curPos.x;
        const auto dy = y - character[i].c->curPos.y;
        const auto dz = z - character[i].c->curPos.z;
        if (fabsf(dy) > character[i].c->height * 0.8f)
            continue;
        if (dx * dx + dz * dz > character[i].c->radius * 0.8f)
            continue;
        return false;
    }
    return true;
}

// Find characters by radius
std::vector<Supervisor::FindCharacter> Supervisor::FindCharacters(Character *chr,
                                float radius, float angTest, float nearPlane, float ax, bool isSort,
                                bool lookCenter) const
{
    if (!chr || radius < 0.0f)
        return {};

    auto found_characters = std::vector<FindCharacter>();
    // Test radius
    radius *= radius;
    // Character position
    auto x = chr->curPos.x;
    auto y = chr->curPos.y;
    auto z = chr->curPos.z;
    // Parameters for testing in the sector at x_z
    CVECTOR N1, N2, N3;
    float d1, d2, d3;
    if (angTest > 0.0f)
    {
        CMatrix m(0.0f, chr->ay, 0.0f);
        auto ang = 0.5f * angTest * 3.141592654f / 180.0f;
        N1 = m * CVECTOR(cosf(ang), 0.0f, sinf(ang));
        d1 = N1 | chr->curPos;
        N2 = m * CVECTOR(-cosf(-ang), 0.0f, -sinf(-ang));
        d2 = N2 | chr->curPos;
        N3 = m.Vz();
        d3 = N3 | chr->curPos;
    }
    // Parameters for testing in the sector by y_dist
    if (ax > 0.0f)
    {
        ax = sinf(0.5f * ax * 3.141592654f / 180.0f);
    }
    else
        ax = 1.0f;
    if (ax < 0.0f)
        ax = 0.0f;
    if (ax > 1.0f)
        ax = 1.0f;
    ax *= ax;
    auto testY = y + chr->height * 0.5f;
    // Viewing the characters
    for (size_t i = 0; i < character.size(); i++)
    {
        // Exclude ourselves
        if (character[i].c == chr)
            continue;
        // Checking for the killed
        if (character[i].c->liveValue < 0 || character[i].c->deadName)
            continue;
        // By distance
        auto dx = character[i].c->curPos.x - x;
        auto dz = character[i].c->curPos.z - z;
        auto d = dx * dx + dz * dz;
        if (d > radius)
            continue;
        // Height
        auto dy = character[i].c->curPos.y + character[i].c->height - testY;
        if (dy < 0.0f && dy * dy > d * ax)
            continue;
        dy = testY - character[i].c->curPos.y;
        if (dy < 0.0f && dy * dy > d * ax)
            continue;
        // In the xz plane
        auto dist1 = 0.0f;
        auto dist2 = 0.0f;
        auto dist3 = 0.0f;
        if (angTest > 0.0f && d > 1.0f) // eddy. when approached from the back, place in the structure
        {
            // Check the location
            auto rad = !lookCenter ? -character[i].c->radius : 0.0f;
            dist1 = (N1 | character[i].c->curPos) - d1;
            if (dist1 < rad)
                continue;
            dist2 = (N2 | character[i].c->curPos) - d2;
            if (dist2 < rad)
                continue;
            dist3 = (N3 | character[i].c->curPos) - d3;
            if (dist3 < nearPlane)
                continue;
        }
        // Add
        found_characters.emplace_back(character[i].c, dx, dy, dz, d);
    }
    if (isSort)
    {
        auto comparator = [](const auto &lhs, const auto &rhs)
        {
            return lhs.d2 < rhs.d2;
        };

        std::ranges::sort(found_characters, comparator);
    }
    return found_characters;
}

// Find the best locator to continue walking the character
int32_t Supervisor::FindForvardLocator(LocatorArray *la, const CVECTOR &pos, const CVECTOR &norm, bool lookChr) const
{
    if (!la)
        return -1;
    const auto num = la->Num();
    CVECTOR lpos;
    float maxcs;
    int32_t l = -1;
    for (int32_t i = 0; i < num; i++)
    {
        if (!la->GetLocatorPos(i, lpos.x, lpos.y, lpos.z))
            continue;
        if (lookChr)
        {
            if (!CheckPosition(lpos.x, lpos.y, lpos.z, nullptr))
                continue;
        }
        lpos -= pos;
        lpos.y = 0.0f;
        auto cs = lpos.x * lpos.x + lpos.z * lpos.z;
        if (cs <= 0.0f)
            continue;
        lpos *= 1.0f / sqrtf(cs);
        cs = lpos | norm;
        if (l > 0)
        {
            if (cs > maxcs)
            {
                l = i;
                maxcs = cs;
            }
        }
        else
        {
            l = i;
            maxcs = cs;
        }
    }
    if (l >= 0 && la->GetLocatorPos(l, lpos.x, lpos.y, lpos.z))
    {
        if (!CheckPosition(lpos.x, lpos.y, lpos.z, nullptr))
        {
            return FindForvardLocator(la, pos, norm, true);
        }
    }
    return l;
}
