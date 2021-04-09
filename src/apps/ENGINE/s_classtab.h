#pragma once

#include "data.h"
#include <vector>

#define CLASS_BUFFER_BLOCK_SIZE 1024
#define INVALID_CLASS_CODE 0xffffffff
#define INVALID_SEGMENT_ID 0xffffffff

#define CCF_SYSTEMTYPEVARIABLE 0x1
#define CCF_VARIABLE 0x2
#define CCF_FUNCTION 0x4
#define CCF_VARARRAY 0x8

struct CLASS_COMPONENT
{
    CLASS_COMPONENT()
    {
        name = nullptr;
        nFlags = 0;
        nID = 0xffffffff;
        nElements = 0;
    };
    uint32_t nID;
    uint32_t nFlags;
    char *name;
    uint32_t nElements;
};

struct CLASSINFO
{
    const char *name;
    uint32_t hash;
    uint32_t segment_id;
    uint32_t nComponentsNum;
    CLASS_COMPONENT *pComponent;
    bool bClassRegistredOnly;
};

class S_DEBUG;

class S_CLASSTAB
{
    friend S_DEBUG;
    uint32_t Buffer_size;
    uint32_t nClassesNum;
    std::vector<CLASSINFO> pTable;
    VIRTUAL_COMPILER *pVCompiler;

  public:
    S_CLASSTAB();
    ~S_CLASSTAB();
    uint32_t GetClassesNum()
    {
        return nClassesNum;
    };
    uint32_t AddClass(CLASSINFO &ci, bool bRegisterOnly = false);

    bool GetClass(CLASSINFO &ci, uint32_t class_code);  // return true if class registred and loaded
    bool GetClassX(CLASSINFO &ci, uint32_t class_code); // return true if class registred
    uint32_t MakeHashValue(const char *string);
    void Release();
    void InvalidateBySegmentID(uint32_t segment_id);
    uint32_t FindClass(const char *class_name);
    void SetVCompiler(VIRTUAL_COMPILER *pvc)
    {
        pVCompiler = pvc;
    }
    void InitSystemClasses();
    bool IsClassDataSet(uint32_t class_id);
};
