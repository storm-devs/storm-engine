#include "DataPosition.h"
#include "..\..\icommon\memfile.h"
#include "..\..\..\common_h\vmodule_api.h"


//конструктор/деструктор
DataPosition::DataPosition ()
{
	Value = Vector(0, 0, 0);
}

DataPosition::~DataPosition ()
{
}

//Получить значение (Текущее время, Коэфицент рандома[0..1])
const Vector& DataPosition::GetValue ()
{
	return Value;
}

//Установить значение
void DataPosition::SetValue (const Vector& val)
{
	Value = val;
}


void DataPosition::Load (MemFile* File)
{
	Vector vValue;
	File->ReadType(vValue.x);
	File->ReadType(vValue.y);
	File->ReadType(vValue.z);
	//api->Trace("Read position %3.2f, %3.2f, %3.2f", vValue.x, vValue.y, vValue.z);
	SetValue (vValue);

	static char AttribueName[128];
	DWORD NameLength = 0;
	File->ReadType(NameLength);
	Assert (NameLength < 128);
	File->Read(AttribueName, NameLength);

	SetName (AttribueName);
}

void DataPosition::SetName (const char* szName)
{
	//api->Trace("DataPosition::SetName - '%s'", szName);
	Name = szName;
}

const char* DataPosition::GetName ()
{
	return Name.GetBuffer();
}

void DataPosition::Write (MemFile* File)
{
	Vector vValue = GetValue();
	//api->Trace("Write position %3.2f, %3.2f, %3.2f", vValue.x, vValue.y, vValue.z);
	File->WriteType(vValue.x);
	File->WriteType(vValue.y);
	File->WriteType(vValue.z);

	//save name
	DWORD NameLength = Name.Len();
	DWORD NameLengthPlusZero = NameLength+1;
	File->WriteType(NameLengthPlusZero);
	Assert (NameLength < 128);
	File->Write(Name.GetBuffer(), NameLength);
	File->WriteZeroByte();
}
