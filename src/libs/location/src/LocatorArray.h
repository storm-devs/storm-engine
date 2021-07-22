//============================================================================================
//    Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    LocatorArray
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#pragma once

#include "Matrix.h"
#include "storm_assert.h"

class LocatorArray
{
    struct LocatorInfro
    {
        CMatrix mtx;
        long name;
        long hash;
        float radius;
    };

    // --------------------------------------------------------------------------------------------
    // Construction, destruction
    // --------------------------------------------------------------------------------------------
  public:
    LocatorArray(const char *groupName);
    virtual ~LocatorArray();

    // --------------------------------------------------------------------------------------------
    // Working with an array
    // --------------------------------------------------------------------------------------------
  public:
    // Add locator
    void AddLocator(CMatrix &mtx, const char *name = nullptr);
    // Change locator matrix
    void SetNewMatrix(long locIndex, CMatrix &mtx);
    // Find the nearest locator by ball
    float FindNearesLocator(float x, float y, float z, long *locIndex = nullptr);
    // Find the nearest locator by cylinder
    long FindNearesLocatorCl(float x, float y, float z, float height2, float &dist);
    // Find locator by name
    long FindByName(const char *locName);
    // Get locator name
    const char *LocatorName(long locIndex);
    // Get locator coordinates
    bool GetLocatorPos(long locIndex, float &x, float &y, float &z);
    // Get locator matrix
    bool GetLocatorPos(long locIndex, CMatrix &mtx);
    // Check the index for correctness
    bool IsValidateIndex(long locIndex) const;
    // Number of locators
    long Num() const;
    // Locator name
    char *Name(long locIndex);
    // Compare group names
    bool CompareGroup(const char *groupName, long ghash) const;
    // Get group name
    char *GetGroupName() const;

    // Set radius of the locator
    void SetLocatorRadius(long locIndex, float radius);
    // Get the radius of the locator
    float GetLocatorRadius(long locIndex);

    // --------------------------------------------------------------------------------------------
    // Encapsulation
    // --------------------------------------------------------------------------------------------
  public:
    static long CalcHashString(const char *str);

  private:
    // Group name
    char *group;
    long hash;
    // Locators
    std::vector<LocatorInfro> locator;
    long numLocators;
    // Locator names
    char *locatorNames;
    long bytesInLNArray;

  public:
    bool isVisible;
    float radius;
    float kViewRadius;
    uint32_t color;
    float viewDist;
};

// Get locator name
inline const char *LocatorArray::LocatorName(long locIndex)
{
    if (locIndex < 0 || locIndex >= numLocators)
        return nullptr;
    if (locator[locIndex].name < 0)
        return nullptr;
    return locatorNames + locator[locIndex].name;
}

// Get locator coordinates
inline bool LocatorArray::GetLocatorPos(long locIndex, float &x, float &y, float &z)
{
    if (locIndex < 0 || locIndex >= numLocators)
        return false;
    x = locator[locIndex].mtx.Pos().x;
    y = locator[locIndex].mtx.Pos().y;
    z = locator[locIndex].mtx.Pos().z;
    return true;
}

// Get locator matrix
inline bool LocatorArray::GetLocatorPos(long locIndex, CMatrix &mtx)
{
    if (locIndex < 0 || locIndex >= numLocators)
        return false;
    mtx = locator[locIndex].mtx;
    return true;
}

// Check the index for correctness
inline bool LocatorArray::IsValidateIndex(long locIndex) const
{
    return (locIndex < 0 || locIndex >= numLocators);
}

// Number of locators
inline long LocatorArray::Num() const
{
    return numLocators;
}

// Locator name
inline char *LocatorArray::Name(long locIndex)
{
    Assert(locIndex >= 0 && locIndex < numLocators);
    return locatorNames + locator[locIndex].name;
}

// Get group name
inline char *LocatorArray::GetGroupName() const
{
    return group;
}

// Set radius of the locator
inline void LocatorArray::SetLocatorRadius(long locIndex, float radius)
{
    Assert(locIndex >= 0 && locIndex < numLocators);
    locator[locIndex].radius = radius;
}

// Get the radius of the locator
inline float LocatorArray::GetLocatorRadius(long locIndex)
{
    Assert(locIndex >= 0 && locIndex < numLocators);
    if (locator[locIndex].radius < 0.0f)
        return radius;
    return locator[locIndex].radius;
}
