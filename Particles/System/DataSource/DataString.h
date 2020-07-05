#ifndef _PARTICLE_DATA_STRING_H_
#define _PARTICLE_DATA_STRING_H_

#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "../../../common_h/exs.h"
#include "../../../common_h/templates.h"
#include "..\..\icommon\memfile.h"

#include "..\..\..\common_h\vapi.h"


class DataString
{
  string Name;

	string Value;

public:

//конструктор/деструктор
	DataString ();
	virtual ~DataString ();

//Получить значение
	const char* GetValue ();

//Установить значение
	void SetValue (const char* val);


	void Load (MemFile* File);
	void Write (MemFile* File);


	void SetName (const char* szName);
	const char* GetName ();

};


#endif