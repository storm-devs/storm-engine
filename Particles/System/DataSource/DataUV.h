#ifndef _PARTICLE_DATA_UV_H_
#define _PARTICLE_DATA_UV_H_


#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "../../../common_h/exs.h"
#include "../../../common_h/templates.h"
#include "..\..\icommon\memfile.h"

#include "..\..\..\common_h\d_types.h"
#include "..\..\..\common_h\math3d.h"



class DataUV
{
  string Name;

	array<Vector4> Frames;

public:

//конструктор/деструктор
	DataUV ();
	virtual ~DataUV ();

	//Получить значение   [ x,y = UV1; z,w = UV2 ]
	const Vector4 &GetValue (DWORD FrameNum);

	//Установить значения
	void SetValues (const Vector4* _Frames, DWORD FramesCount);

	//Получить кол-во кадров
	DWORD GetFrameCount ();


	void Load (MemFile* File);
	void Write (MemFile* File);

	void SetName (const char* szName);
	const char* GetName ();


};


#endif