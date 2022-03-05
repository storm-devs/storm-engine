#include "data_float.h"
#include "v_module_api.h"

// constructor / destructor
DataFloat::DataFloat()
{
    Value = 0.0f;
}

DataFloat::~DataFloat()
{
}

// Get value
float DataFloat::GetValue() const
{
    return Value;
}

// Set value
void DataFloat::SetValue(float val)
{
    Value = val;
}

void DataFloat::Load(MemFile *File)
{
    auto fValue = 0.0f;
    File->ReadType(fValue);
    SetValue(fValue);

    static char AttribueName[128];
    uint32_t NameLength = 0;
    File->ReadType(NameLength);
    Assert(NameLength < 128);
    File->Read(AttribueName, NameLength);

    SetName(AttribueName);
}

void DataFloat::SetName(const char *szName)
{
    // core.Trace("DataFloat::SetName - '%s'", szName);
    Name = szName;
}

const char *DataFloat::GetName() const
{
    return Name.c_str();
}

void DataFloat::Write(MemFile *File) const
{
    auto fValue = GetValue();
    File->WriteType(fValue);

    // save name
    const uint32_t NameLength = Name.size();
    auto NameLengthPlusZero = NameLength + 1;
    File->WriteType(NameLengthPlusZero);
    Assert(NameLength < 128);
    File->Write(Name.c_str(), NameLength);
    File->WriteZeroByte();
}
