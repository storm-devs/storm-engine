//============================================================================================
//    Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    LocatorArray
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#include "locator_array.h"

#include "storm/string_compare.hpp"

// ============================================================================================
// Construction, destruction
// ============================================================================================

LocatorArray::LocatorArray(const char *groupName)
{
    numLocators = 0;
    locatorNames = nullptr;
    bytesInLNArray = 0;
    if (groupName)
    {
        const int32_t l = strlen(groupName) + 1;
        group = new char[l];
        memcpy(group, groupName, l);
    }
    else
    {
        group = new char[1];
        group[0] = 0;
    }
    hash = CalcHashString(group);
    isVisible = false;
    radius = 0.0f;
    kViewRadius = -1.0f;
}

LocatorArray::~LocatorArray()
{
    delete group;
    free(locatorNames);
}

// ============================================================================================
// Working with an array
// ============================================================================================

// Add locator
void LocatorArray::AddLocator(CMatrix &mtx, const char *name)
{
    locator.resize(numLocators + 1);
    locator[numLocators].mtx = mtx;
    locator[numLocators].radius = -1.0f;
    int32_t slen = 0;
    if (name && name[0])
    {
        slen = strlen(name) + 1;
        locatorNames = static_cast<char *>(realloc(locatorNames, bytesInLNArray + slen));
        locator[numLocators].name = bytesInLNArray;
        memcpy(locatorNames + bytesInLNArray, name, slen);
        locator[numLocators].hash = CalcHashString(locatorNames + bytesInLNArray);
        bytesInLNArray += slen;
    }
    else
        locator[numLocators].name = -1;
    numLocators++;
}

// Change locator matrix
void LocatorArray::SetNewMatrix(int32_t locIndex, CMatrix &mtx)
{
    if (locIndex < 0 || locIndex >= numLocators)
        return;
    locator[locIndex].mtx = mtx;
}

// Find nearest locator
float LocatorArray::FindNearesLocator(float x, float y, float z, int32_t *locIndex)
{
    if (locIndex)
        *locIndex = -1;
    auto dist = 1000000000.0f;
    const CVECTOR v(x, y, z);
    for (int32_t i = 0; i < numLocators; i++)
    {
        const auto d = ~(locator[i].mtx.Pos() - v);
        if (dist > d)
        {
            if (locIndex)
                *locIndex = i;
            dist = d;
        }
    }
    return dist;
}

// Find the nearest locator by cylinder
int32_t LocatorArray::FindNearesLocatorCl(float x, float y, float z, float height2, float &dist)
{
    int32_t locIndex = -1;
    for (int32_t i = 0; i < numLocators; i++)
    {
        // if(fabsf(y - locator[i].mtx.Pos().y) > height2) continue;
        const auto r = GetLocatorRadius(i);

        if (fabsf(y - locator[i].mtx.Pos().y) > r)
            continue;

        if (r <= 0.0f)
            continue;
        const auto vx = locator[i].mtx.Pos().x - x;
        const auto vz = locator[i].mtx.Pos().z - z;
        const auto d = vx * vx + vz * vz;
        if (r * r <= d)
            continue;
        if (locIndex >= 0)
        {
            if (d < dist)
            {
                locIndex = i;
                dist = d;
            }
        }
        else
        {
            locIndex = i;
            dist = d;
        }
    }
    return locIndex;
}

// Find locator by name
int32_t LocatorArray::FindByName(const char *locName)
{
    if (!locName)
        return -1;
    const auto hash = CalcHashString(locName);
    for (int32_t i = 0; i < numLocators; i++)
    {
        if (locator[i].name >= 0)
        {
            if (locator[i].hash == hash)
                if (storm::iEquals(locatorNames + locator[i].name, locName))
                {
                    return i;
                }
        }
    }
    return -1;
}

int32_t LocatorArray::CalcHashString(const char *str)
{
    uint32_t hval = 0;
    while (*str != '\0')
    {
        auto c = *str++;
        if (c >= 'A' && c <= 'Z')
            c += 'a' - 'A';
        hval = (hval << 4) + static_cast<uint32_t>(c);
        const auto g = hval & (static_cast<uint32_t>(0xf) << (32 - 4));
        if (g != 0)
        {
            hval ^= g >> (32 - 8);
            hval ^= g;
        }
    }
    return static_cast<int32_t>(hval);
}

// Compare group names
bool LocatorArray::CompareGroup(const char *groupName, int32_t ghash) const
{
    if (hash != ghash)
        return false;
    if (!groupName)
        return group[0] == 0;
    return storm::iEquals(group, groupName);
}
