#pragma once

#include "../../icommon/memfile.h"

#include <string>

class DataString
{
    std::string Name;

    std::string Value;

  public:
    // constructor / destructor
    DataString();
    virtual ~DataString();

    // Get value
    const char *GetValue() const;

    // Set value
    void SetValue(const char *val);

    void Load(MemFile *File);
    void Write(MemFile *File) const;

    void SetName(const char *szName);
    const char *GetName() const;
};
