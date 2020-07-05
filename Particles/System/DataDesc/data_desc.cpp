#include "data_desc.h"


	 
DataDescripion::DataDescripion ()
{
	ItemsCount = 0;
}

DataDescripion::~DataDescripion ()
{
}
 
void DataDescripion::AddField (FieldType Type, const char* Name)
{
	Assert (ItemsCount < MAX_DESC_COUNT);
	Fields[ItemsCount].Type = Type;
	Fields[ItemsCount].Name = Name;
	ItemsCount++;
}
 
const char* DataDescripion::GetFieldName (DWORD Index)
{
	return Fields[Index].Name;
}

FieldType DataDescripion::GetFieldType (DWORD Index)
{
	return Fields[Index].Type;
}


int DataDescripion::GetFieldCount ()
{
  return ItemsCount;
}

void DataDescripion::Clear ()
{
	ItemsCount = 0;
}

