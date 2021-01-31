#ifndef _PARTICLE_DATA_FLOAT_H_
#define _PARTICLE_DATA_FLOAT_H_

#include "../../ICommon/MemFile.h"
#include <string>

class DataFloat
{
    std::string Name;

    float Value;

  public:
    //конструктор/деструктор
    DataFloat();
    virtual ~DataFloat();

    //Получить значение
    float GetValue() const;

    //Установить значение
    void SetValue(float val);

    void Load(MemFile *File);
    void Write(MemFile *File) const;

    void SetName(const char *szName);
    const char *GetName() const;
};

#endif
