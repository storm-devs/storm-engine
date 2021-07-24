#pragma once

#include "../../ICommon/MemFile.h"

#include "math3d.h"
#include <string>

class DataPosition
{
    std::string Name;

    Vector Value;

  public:
    // constructor / destructor
    DataPosition();
    virtual ~DataPosition();

    // Get value (Current time, Random factor [0..1])
    const Vector &GetValue() const;

    // Set value
    void SetValue(const Vector &val);

    void Load(MemFile *File);
    void Write(MemFile *File) const;

    void SetName(const char *szName);
    const char *GetName() const;
};
