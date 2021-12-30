#pragma once

#include "../../i_common/mem_file.h"
#include <string>

class DataBool
{
    std::string Name;

    bool Value;

  public:
    // constructor / destructor
    DataBool();
    virtual ~DataBool();

    // Get value
    bool GetValue() const;

    // Set value
    void SetValue(bool val);

    // Save / Load ...
    void Load(MemFile *File);
    void Write(MemFile *File) const;

    void SetName(const char *szName);
    const char *GetName() const;
};
