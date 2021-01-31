#ifndef _PARTICLE_DATA_POSITION_H_
#define _PARTICLE_DATA_POSITION_H_

#include "../../ICommon/MemFile.h"

#include "math3d.h"
#include <string>

class DataPosition
{
    std::string Name;

    Vector Value;

  public:
    //конструктор/деструктор
    DataPosition();
    virtual ~DataPosition();

    //Получить значение (Текущее время, Коэфицент рандома[0..1])
    const Vector &GetValue() const;

    //Установить значение
    void SetValue(const Vector &val);

    void Load(MemFile *File);
    void Write(MemFile *File) const;

    void SetName(const char *szName);
    const char *GetName() const;
};

#endif
