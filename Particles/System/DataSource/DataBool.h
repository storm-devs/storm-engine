#ifndef _PARTICLE_DATA_BOOLEAN_H_
#define _PARTICLE_DATA_BOOLEAN_H_

#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "../../../common_h/exs.h"
#include "../../../common_h/templates.h"
#include "..\..\icommon\memfile.h"



class DataBool
{
  string Name;
  

	bool Value;

public:

//конструктор/деструктор
	DataBool ();
	virtual ~DataBool ();

//Получить значение
	bool GetValue ();

//Установить значение
	void SetValue (bool val);


//Сохранить/загрузить...
	void Load (MemFile* File);
	void Write (MemFile* File);


	void SetName (const char* szName);
	const char* GetName ();


};


#endif