//============================================================================================
//	Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//	LocatorArray
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#ifndef _LocatorArray_H_
#define _LocatorArray_H_

#include "Matrix.h"
#include "vmodule_api.h"

class LocatorArray
{
    struct LocatorInfro
    {
        CMatrix mtx;
        long name;
        long hash;
        float radius;
    };

    //--------------------------------------------------------------------------------------------
    //Конструирование, деструктурирование
    //--------------------------------------------------------------------------------------------
  public:
    LocatorArray(const char *groupName);
    virtual ~LocatorArray();

    //--------------------------------------------------------------------------------------------
    //Работа с массивом
    //--------------------------------------------------------------------------------------------
  public:
    //Добавить локатор
    void AddLocator(CMatrix &mtx, const char *name = nullptr);
    //Изменить матрицу локатора
    void SetNewMatrix(long locIndex, CMatrix &mtx);
    //Найти ближайший локатор по шарику
    float FindNearesLocator(float x, float y, float z, long *locIndex = nullptr);
    //Найти ближайший локатор по цилиндру
    long FindNearesLocatorCl(float x, float y, float z, float height2, float &dist);
    //Найти локатор по имени
    long FindByName(const char *locName);
    //Получить имя локатора
    const char *LocatorName(long locIndex);
    //Получить координаты локатора
    bool GetLocatorPos(long locIndex, float &x, float &y, float &z);
    //Получить матрицу локатора
    bool GetLocatorPos(long locIndex, CMatrix &mtx);
    //Проверить индекс на правильность
    bool IsValidateIndex(long locIndex) const;
    //Количество локаторов
    long Num() const;
    //Имя локатора
    char *Name(long locIndex);
    //Сравнить имена груп
    bool CompareGroup(const char *groupName, long ghash) const;
    //Получить имя группы
    char *GetGroupName() const;

    //Установить локатору радиус
    void SetLocatorRadius(long locIndex, float radius);
    //Получить радиус локатора
    float GetLocatorRadius(long locIndex);

    //--------------------------------------------------------------------------------------------
    //Инкапсуляция
    //--------------------------------------------------------------------------------------------
  public:
    static long CalcHashString(const char *str);

  private:
    //Имя группы
    char *group;
    long hash;
    //Локаторы
    std::vector<LocatorInfro> locator;
    long numLocators;
    //Имена локаторов
    char *locatorNames;
    long bytesInLNArray;

  public:
    bool isVisible;
    float radius;
    float kViewRadius;
    uint32_t color;
    float viewDist;
};

//Получить имя локатора
inline const char *LocatorArray::LocatorName(long locIndex)
{
    if (locIndex < 0 || locIndex >= numLocators)
        return nullptr;
    if (locator[locIndex].name < 0)
        return nullptr;
    return locatorNames + locator[locIndex].name;
}

//Получить координаты локатора
inline bool LocatorArray::GetLocatorPos(long locIndex, float &x, float &y, float &z)
{
    if (locIndex < 0 || locIndex >= numLocators)
        return false;
    x = locator[locIndex].mtx.Pos().x;
    y = locator[locIndex].mtx.Pos().y;
    z = locator[locIndex].mtx.Pos().z;
    return true;
}

//Получить матрицу локатора
inline bool LocatorArray::GetLocatorPos(long locIndex, CMatrix &mtx)
{
    if (locIndex < 0 || locIndex >= numLocators)
        return false;
    mtx = locator[locIndex].mtx;
    return true;
}

//Проверить индекс на правильность
inline bool LocatorArray::IsValidateIndex(long locIndex) const
{
    return (locIndex < 0 || locIndex >= numLocators);
}

//Количество локаторов
inline long LocatorArray::Num() const
{
    return numLocators;
}

//Имя локатора
inline char *LocatorArray::Name(long locIndex)
{
    Assert(locIndex >= 0 && locIndex < numLocators);
    return locatorNames + locator[locIndex].name;
}

//Получить имя группы
inline char *LocatorArray::GetGroupName() const
{
    return group;
}

//Установить локатору радиус
inline void LocatorArray::SetLocatorRadius(long locIndex, float radius)
{
    Assert(locIndex >= 0 && locIndex < numLocators);
    locator[locIndex].radius = radius;
}

//Получить радиус локатора
inline float LocatorArray::GetLocatorRadius(long locIndex)
{
    Assert(locIndex >= 0 && locIndex < numLocators);
    if (locator[locIndex].radius < 0.0f)
        return radius;
    return locator[locIndex].radius;
}

#endif
