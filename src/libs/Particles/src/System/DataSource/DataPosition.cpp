#include "DataPosition.h"
#include "vmodule_api.h"

// constructor / destructor
DataPosition::DataPosition()
{
    Value = Vector(0, 0, 0);
}

DataPosition::~DataPosition()
{
}

// Get value (Current time, Random factor [0..1])
const Vector &DataPosition::GetValue() const
{
    return Value;
}

// Set value
void DataPosition::SetValue(const Vector &val)
{
    Value = val;
}

void DataPosition::Load(MemFile *File)
{
    Vector vValue;
    File->ReadType(vValue.x);
    File->ReadType(vValue.y);
    File->ReadType(vValue.z);
    // core.Trace("Read position %3.2f, %3.2f, %3.2f", vValue.x, vValue.y, vValue.z);
    SetValue(vValue);

    static char AttribueName[128];
    uint32_t NameLength = 0;
    File->ReadType(NameLength);
    Assert(NameLength < 128);
    File->Read(AttribueName, NameLength);

    SetName(AttribueName);
}

void DataPosition::SetName(const char *szName)
{
    // core.Trace("DataPosition::SetName - '%s'", szName);
    Name = szName;
}

const char *DataPosition::GetName() const
{
    return Name.c_str();
}

void DataPosition::Write(MemFile *File) const
{
    auto vValue = GetValue();
    // core.Trace("Write position %3.2f, %3.2f, %3.2f", vValue.x, vValue.y, vValue.z);
    File->WriteType(vValue.x);
    File->WriteType(vValue.y);
    File->WriteType(vValue.z);

    // save name
    const uint32_t NameLength = Name.size();
    auto NameLengthPlusZero = NameLength + 1;
    File->WriteType(NameLengthPlusZero);
    Assert(NameLength < 128);
    File->Write(Name.c_str(), NameLength);
    File->WriteZeroByte();
}
