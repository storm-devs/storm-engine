#pragma once

#include "../../i_common/mem_file.h"
#include <string>

class DataFloat
{
    std::string Name;

    float Value;

  public:
    // constructor / destructor
    DataFloat();
    virtual ~DataFloat();

    // Get value
    float GetValue() const;

    // Set value
    void SetValue(float val);

    void Load(MemFile *File);
    void Write(MemFile *File) const;

    void SetName(const char *szName);
    const char *GetName() const;
};
