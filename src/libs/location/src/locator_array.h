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

#include "matrix.h"
#include "storm_assert.h"

class LocatorArray
{
    struct LocatorInfro
    {
        CMatrix mtx;
        int32_t name;
        int32_t hash;
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
    void SetNewMatrix(int32_t locIndex, CMatrix &mtx);
    // Find the nearest locator by ball
    float FindNearesLocator(float x, float y, float z, int32_t *locIndex = nullptr);
    // Find the nearest locator by cylinder
    int32_t FindNearesLocatorCl(float x, float y, float z, float height2, float &dist);
    // Find locator by name
    int32_t FindByName(const char *locName);
    // Get locator name
    const char *LocatorName(int32_t locIndex);
    // Get locator coordinates
    bool GetLocatorPos(int32_t locIndex, float &x, float &y, float &z);
    // Get locator matrix
    bool GetLocatorPos(int32_t locIndex, CMatrix &mtx);
    // Check the index for correctness
    bool IsValidateIndex(int32_t locIndex) const;
    // Number of locators
    int32_t Num() const;
    // Locator name
    char *Name(int32_t locIndex);
    // Compare group names
    bool CompareGroup(const char *groupName, int32_t ghash) const;
    // Get group name
    char *GetGroupName() const;

    // Set radius of the locator
    void SetLocatorRadius(int32_t locIndex, float radius);
    // Get the radius of the locator
    float GetLocatorRadius(int32_t locIndex);

    // --------------------------------------------------------------------------------------------
    // Encapsulation
    // --------------------------------------------------------------------------------------------
  public:
    static int32_t CalcHashString(const char *str);

  private:
    // Group name
    char *group;
    int32_t hash;
    // Locators
    std::vector<LocatorInfro> locator;
    int32_t numLocators;
    // Locator names
    char *locatorNames;
    int32_t bytesInLNArray;

  public:
    bool isVisible;
    float radius;
    float kViewRadius;
    uint32_t color;
    float viewDist;
};

// Get locator name
inline const char *LocatorArray::LocatorName(int32_t locIndex)
{
    if (locIndex < 0 || locIndex >= numLocators)
        return nullptr;
    if (locator[locIndex].name < 0)
        return nullptr;
    return locatorNames + locator[locIndex].name;
}

// Get locator coordinates
inline bool LocatorArray::GetLocatorPos(int32_t locIndex, float &x, float &y, float &z)
{
    if (locIndex < 0 || locIndex >= numLocators)
        return false;
    x = locator[locIndex].mtx.Pos().x;
    y = locator[locIndex].mtx.Pos().y;
    z = locator[locIndex].mtx.Pos().z;
    return true;
}

// Get locator matrix
inline bool LocatorArray::GetLocatorPos(int32_t locIndex, CMatrix &mtx)
{
    if (locIndex < 0 || locIndex >= numLocators)
        return false;
    mtx = locator[locIndex].mtx;
    return true;
}

// Check the index for correctness
inline bool LocatorArray::IsValidateIndex(int32_t locIndex) const
{
    return (locIndex < 0 || locIndex >= numLocators);
}

// Number of locators
inline int32_t LocatorArray::Num() const
{
    return numLocators;
}

// Locator name
inline char *LocatorArray::Name(int32_t locIndex)
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
inline void LocatorArray::SetLocatorRadius(int32_t locIndex, float radius)
{
    Assert(locIndex >= 0 && locIndex < numLocators);
    locator[locIndex].radius = radius;
}

// Get the radius of the locator
inline float LocatorArray::GetLocatorRadius(int32_t locIndex)
{
    Assert(locIndex >= 0 && locIndex < numLocators);
    if (locator[locIndex].radius < 0.0f)
        return radius;
    return locator[locIndex].radius;
}
