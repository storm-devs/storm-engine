#include "DataFloat.h"
#include "..\..\icommon\memfile.h"
#include "..\..\..\common_h\vmodule_api.h"



//конструктор/деструктор
DataFloat::DataFloat ()
{
	Value = 0.0f;
}

DataFloat::~DataFloat ()
{
}

	//Получить значение
float DataFloat::GetValue ()
{
	return Value;
}

	//Установить значение
void DataFloat::SetValue (float val)
{
	Value = val;
}

void DataFloat::Load (MemFile* File)
{
	float fValue = 0.0f;
	File->ReadType(fValue);
	SetValue (fValue);

	static char AttribueName[128];
	DWORD NameLength = 0;
	File->ReadType(NameLength);
	Assert (NameLength < 128);
	File->Read(AttribueName, NameLength);

	SetName (AttribueName);

}

void DataFloat::SetName (const char* szName)
{
	//api->Trace("DataFloat::SetName - '%s'", szName);
	Name = szName;
}

const char* DataFloat::GetName ()
{
	return Name.GetBuffer();
}

void DataFloat::Write (MemFile* File)
{
	float fValue = GetValue();
	File->WriteType(fValue);

	//save name
	DWORD NameLength = Name.Len();
	DWORD NameLengthPlusZero = NameLength+1;
	File->WriteType(NameLengthPlusZero);
	Assert (NameLength < 128);
	File->Write(Name.GetBuffer(), NameLength);
	File->WriteZeroByte();
}

