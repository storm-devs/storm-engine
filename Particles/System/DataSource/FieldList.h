#ifndef _FIELD_LIST_H
#define _FIELD_LIST_H

#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "../../../common_h/math3d.h"
#include "../../../common_h/exs.h"
#include "../../../common_h/templates.h"
#include "../../../common_h/vapi.h"
#include "..\..\icommon\types.h"
#include "..\..\icommon\memfile.h"
#include "..\datadesc\data_desc.h"


class DataColor;
class DataBool;
class DataFloat;
class DataGraph;
class DataString;
class DataPosition;
class DataUV;


class FieldList
{
public:

  struct FieldDesc
  {
		//При конвертации используеться этот флажок
		bool MarkForDelete;

		//Хэш имени
		DWORD HashValue;

		//Тип
    FieldType Type;

		//Имя
    string Name;

		//Указатель
    void* pPointer;
  };
  
private:

  array<FieldDesc> Fields;

	void DeleteFieldData (const FieldList::FieldDesc& pData);


	//=================== создание пустых аттрибутов
	void CreateEmptyBoolField (const char* Name, bool def_value);
	void CreateEmptyFloatField (const char* Name, float def_value);
	void CreateEmptyGraphField (const char* Name, float def_value_min, float def_value_max);
	void CreateEmptyPositionField (const char* Name, const Vector& def_value);
	void CreateEmptyStringField (const char* Name, const char* def_value);
	void CreateEmptyUVField (const char* Name);
	void CreateEmptyColorField (const char* Name, DWORD def_value);


	//=================== Прогрузка аттрибутов из файла
	void CreateBoolField (MemFile* pMemFile);
	void CreateFloatField (MemFile* pMemFile);
	void CreateGraphField (MemFile* pMemFile);
	void CreatePositionField (MemFile* pMemFile);
	void CreateStringField (MemFile* pMemFile);
	void CreateUVField (MemFile* pMemFile);
	void CreateColorField (MemFile* pMemFile);


	FieldList::FieldDesc* FindField (const char* Name);

public:

  FieldList ();
  ~FieldList ();

	void DelAll ();

  
//Сохранить/загрузить...
	void Load (MemFile* File);
  void Write (MemFile* File);


	DataColor* FindColor (const char* AttrName);
	DataGraph* FindGraph (const char* AttrName);
	DataUV* FindUV (const char* AttrName);

	DataBool* FindBool (const char* AttrName);
	DataFloat* FindFloat (const char* AttrName);
	DataString* FindString (const char* AttrName);
	DataPosition* FindPosition (const char* AttrName);


	float GetFloat (const char* AttrName, float def_value = 0.0f);
	int GetFloatAsInt (const char* AttrName, int def_value = 0);
	bool GetBool (const char* AttrName, bool def_value = false);
	const char* GetString (const char* AttrName, const char* def_value = "");
	const Vector& GetPosition (const char* AttrName, const Vector& def_value = Vector(0.0f));

	float GetGraphVal (const char* AttrName, float Time, float LifeTime, float K_Rand, float def_value = 0.0f);
	float GetRandomGraphVal (const char* AttrName, float Time, float LifeTime, float def_value = 0.0f);


	DWORD GetFieldCount ();
	const FieldList::FieldDesc& GetFieldByIndex (DWORD Index);



//Конвертация ГАРАНТИРУЕТ, что в данном DataSource будут нужные поля
//и не будет лишнего мусора...
	void Convert (DataDescripion* pDataDescriptor);




  

};


#endif