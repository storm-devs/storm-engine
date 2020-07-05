#include "DataColor.h"
#include "..\..\icommon\graphtime.h"
#include "..\..\icommon\memfile.h"
#include "..\..\..\common_h\vmodule_api.h"

//конструктор/деструктор
DataColor::DataColor () : ColorGraph(_FL_, 128), ZeroColor(0xFFFFFFFFL)
{
}

DataColor::~DataColor ()
{
}

//Получить значение (Текущее время, Общее время жизни, Коэфицент рандома[0..1])
Color DataColor::GetValue (float Time, float LifeTime, float K_rand)
{
	//Время у графика цвета всегда относительное...
	Time = (Time / LifeTime);

	DWORD Count = ColorGraph.Size();
	DWORD StartIndex = 0;
	for (DWORD n = StartIndex; n < (Count-1); n++)
	{
		float FromTime = ColorGraph[n].Time;
		float ToTime = ColorGraph[n+1].Time;

		//Если время в нужном диапазоне...
		if ((Time >= FromTime)	&& (Time <= ToTime))
		{
			float SegmentDeltaTime = ColorGraph[n+1].Time - ColorGraph[n].Time;
			float ValueDeltaTime = Time - ColorGraph[n].Time;
			float blend_k = 0.0f;
			if (SegmentDeltaTime > 0.001f)	blend_k = ValueDeltaTime / SegmentDeltaTime;

			Color ValueFirstMax = ColorGraph[n].MaxValue;
			Color ValueSecondMax = ColorGraph[n+1].MaxValue;
			Color ValueFirstMin = ColorGraph[n].MinValue;
			Color ValueSecondMin = ColorGraph[n+1].MinValue;

			Color MaxVal;
			MaxVal.Lerp(ValueFirstMax, ValueSecondMax, blend_k);
			Color MinVal;
			MinVal.Lerp(ValueFirstMin, ValueSecondMin, blend_k);

			Color Result;
			Result.Lerp(MinVal, MaxVal, K_rand);

			return Result;
		}
	}

	return ZeroColor;
}

//Устанавливает "значение по умолчанию"
//два индекса, Min=Max=Value
void DataColor::SetDefaultValue (const Color& Value)
{
	ColorGraph.DelAll();

	ColorVertex pMinVertex;
	pMinVertex.Time = MIN_GRAPH_TIME;
	pMinVertex.MinValue = Value;
	pMinVertex.MaxValue = Value;
	ColorGraph.Add(pMinVertex);

	ColorVertex pMaxVertex;
	pMinVertex.Time = 1.0f;
	pMinVertex.MinValue = Value;
	pMinVertex.MaxValue = Value;
	ColorGraph.Add(pMinVertex);

}

//Установить значения
void DataColor::SetValues (const ColorVertex* Values, DWORD Count)
{
	ColorGraph.DelAll();
	for (DWORD n = 0; n < Count; n++)
	{
		ColorGraph.Add(Values[n]);
	}
}

//Получить кол-во значений
DWORD DataColor::GetValuesCount ()
{
	return ColorGraph.Size();
}


//Получить мин. значение (по индексу)
const Color& DataColor::GetMinValue (DWORD Index)
{
	return ColorGraph[Index].MinValue;
}

//Получить макс. значение (по индексу)
const Color& DataColor::GetMaxValue (DWORD Index)
{
	return ColorGraph[Index].MaxValue;
}


void DataColor::Load (MemFile* File)
{
	DWORD dwColorCount = 0;
	File->ReadType(dwColorCount);

	for (DWORD n = 0; n < dwColorCount; n++)
	{
		float Time = 0.0f;
		File->ReadType(Time);

		Color clrMax;
		File->ReadType(clrMax);

		Color clrMin;
		File->ReadType(clrMin);

		ColorVertex pColor;
		pColor.Time = Time;
		pColor.MinValue = clrMin;
		pColor.MaxValue = clrMax;
		ColorGraph.Add(pColor);
	}

	static char AttribueName[128];
	DWORD NameLength = 0;
	File->ReadType(NameLength);
	Assert (NameLength < 128);
	File->Read(AttribueName, NameLength);

	SetName (AttribueName);

}

void DataColor::SetName (const char* szName)
{
	//api->Trace("DataColor::SetName - '%s'", szName);
	Name = szName;
}

const char* DataColor::GetName ()
{
	return Name.GetBuffer();
}

const ColorVertex& DataColor::GetByIndex (DWORD Index)
{
	return ColorGraph[Index];
}

void DataColor::Write (MemFile* File)
{
	DWORD dwColorCount = ColorGraph.Size();
	File->WriteType(dwColorCount);

	for (DWORD n = 0; n < dwColorCount; n++)
	{
		float Time = ColorGraph[n].Time;
		File->WriteType(Time);

		Color clrMax = ColorGraph[n].MaxValue;
		File->WriteType(clrMax);

		Color clrMin = ColorGraph[n].MinValue;
		File->WriteType(clrMin);
	}

	//save name
	DWORD NameLength = Name.Len();
	DWORD NameLengthPlusZero = NameLength+1;
	File->WriteType(NameLengthPlusZero);
	Assert (NameLength < 128);
	File->Write(Name.GetBuffer(), NameLength);
	File->WriteZeroByte();
}

