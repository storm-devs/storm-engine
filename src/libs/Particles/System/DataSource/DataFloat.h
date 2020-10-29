#ifndef _PARTICLE_DATA_FLOAT_H_
#define _PARTICLE_DATA_FLOAT_H_

#include "../../../common_h/exs.h"
#include "../../../common_h/templates.h"
#include "..\..\icommon\memfile.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

class DataFloat
{
    string Name;

    float Value;

  public:
    //конструктор/деструктор
    DataFloat();
    virtual ~DataFloat();

    //Получить значение
    float GetValue();

    //Установить значение
    void SetValue(float val);

    void Load(MemFile *File);
    void Write(MemFile *File);

    void SetName(const char *szName);
    const char *GetName();
};

#endif