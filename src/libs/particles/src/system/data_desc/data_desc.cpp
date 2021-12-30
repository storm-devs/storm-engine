#include "data_desc.h"
#include "storm_assert.h"

DataDescripion::DataDescripion()
{
    ItemsCount = 0;
}

DataDescripion::~DataDescripion()
{
}

void DataDescripion::AddField(FieldType Type, const char *Name)
{
    Assert(ItemsCount < MAX_DESC_COUNT);
    Fields[ItemsCount].Type = Type;
    Fields[ItemsCount].Name = Name;
    ItemsCount++;
}

const char *DataDescripion::GetFieldName(uint32_t Index) const
{
    return Fields[Index].Name;
}

FieldType DataDescripion::GetFieldType(uint32_t Index) const
{
    return Fields[Index].Type;
}

int DataDescripion::GetFieldCount() const
{
    return ItemsCount;
}

void DataDescripion::Clear()
{
    ItemsCount = 0;
}
