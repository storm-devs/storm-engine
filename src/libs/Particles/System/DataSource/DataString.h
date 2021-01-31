#ifndef _PARTICLE_DATA_STRING_H_
#define _PARTICLE_DATA_STRING_H_

#include "../../icommon/memfile.h"

#include <string>

class DataString
{
    std::string Name;

    std::string Value;

  public:
    //конструктор/деструктор
    DataString();
    virtual ~DataString();

    //Получить значение
    const char *GetValue() const;

    //Установить значение
    void SetValue(const char *val);

    void Load(MemFile *File);
    void Write(MemFile *File) const;

    void SetName(const char *szName);
    const char *GetName() const;
};

#endif
