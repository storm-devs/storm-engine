#ifndef _PARTICLE_DATA_POSITION_H_
#define _PARTICLE_DATA_POSITION_H_

#include "../../../common_h/exs.h"
#include "../../../common_h/templates.h"
#include "..\..\icommon\memfile.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "math3d.h"

class DataPosition
{
    string Name;

    CVECTOR Value;

  public:
    //конструктор/деструктор
    DataPosition();
    virtual ~DataPosition();

    //Получить значение (Текущее время, Коэфицент рандома[0..1])
    const CVECTOR &GetValue();

    //Установить значение
    void SetValue(const CVECTOR &val);

    void Load(MemFile *File);
    void Write(MemFile *File);

    void SetName(const char *szName);
    const char *GetName();
};

#endif