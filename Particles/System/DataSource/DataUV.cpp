#include "DataUV.h"
#include "..\..\icommon\memfile.h"
#include "..\..\..\common_h\vmodule_api.h"


//конструктор/деструктор
DataUV::DataUV () : Frames(_FL_)
{
}

DataUV::~DataUV ()
{
}

	//Получить значение   [ x,y = UV1; z,w = UV2 ]
const Vector4 &DataUV::GetValue (DWORD FrameNum)
{
	DWORD TotalFrames = Frames.Size();
	FrameNum = FrameNum % TotalFrames;
	return Frames[FrameNum];
}

	//Установить значения
void DataUV::SetValues (const Vector4* _Frames, DWORD FramesCount)
{
	Frames.DelAll();

	for (DWORD n = 0; n < FramesCount; n++)
	{
		Frames.Add(_Frames[n]);
	}
}

	//Получить кол-во кадров
DWORD DataUV::GetFrameCount ()
{
	return Frames.Size();
}

void DataUV::Load (MemFile* File)
{
	DWORD ElementCount = 0;
	File->ReadType(ElementCount);
	for (DWORD n = 0; n < ElementCount; n++)
	{
		Vector4 rFrame;
		File->ReadType(rFrame.x);
		File->ReadType(rFrame.y);
		File->ReadType(rFrame.z);
		File->ReadType(rFrame.w);

		Vector4 newFrame;
		newFrame = rFrame;
		newFrame.z += newFrame.x;
		newFrame.w += newFrame.y;
		Frames.Add(newFrame);
	}

	static char AttribueName[128];
	DWORD NameLength = 0;
	File->ReadType(NameLength);
	Assert (NameLength < 128);
	File->Read(AttribueName, NameLength);

	SetName (AttribueName);
}

void DataUV::SetName (const char* szName)
{
	//api->Trace("DataUV::SetName - '%s'", szName);
	Name = szName;
}

const char* DataUV::GetName ()
{
	return Name.GetBuffer();
}

void DataUV::Write (MemFile* File)
{
	DWORD ElementCount = GetFrameCount();
	File->WriteType(ElementCount);
	for (DWORD n = 0; n < ElementCount; n++)
	{
		float w = Frames[n].z-Frames[n].x;
		float h = Frames[n].w-Frames[n].y;
		File->WriteType(Frames[n].x);
		File->WriteType(Frames[n].y);
		File->WriteType(w);
		File->WriteType(h);
	}

	//save name
	DWORD NameLength = Name.Len();
	DWORD NameLengthPlusZero = NameLength+1;
	File->WriteType(NameLengthPlusZero);
	Assert (NameLength < 128);
	File->Write(Name.GetBuffer(), NameLength);
	File->WriteZeroByte();
}
