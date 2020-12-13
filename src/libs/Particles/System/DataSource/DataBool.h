#ifndef _PARTICLE_DATA_BOOLEAN_H_
#define _PARTICLE_DATA_BOOLEAN_H_

#include "../../ICommon/MemFile.h"
#include <string>

class DataBool
{
    std::string Name;

    bool Value;

  public:
    //конструктор/деструктор
    DataBool();
    virtual ~DataBool();

    //Получить значение
    bool GetValue() const;

    //Установить значение
    void SetValue(bool val);

    //Сохранить/загрузить...
    void Load(MemFile *File);
    void Write(MemFile *File) const;

    void SetName(const char *szName);
    const char *GetName() const;
};

#endif
