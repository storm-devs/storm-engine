#pragma once

#include "service.h"

using SERVICE_NODE = struct
{
    uint32_t reference;
    uint32_t module_code;
    uint32_t class_code;
    SERVICE *pointer;
    void *linkL;
    void *linkR;
};

class SERVICES_LIST
{
    SERVICE_NODE *List;
    uint32_t Objects;
    uint32_t Search_module_code;

  public:
    SERVICES_LIST();
    ~SERVICES_LIST();

    void Release();
    SERVICE_NODE *FindNode(uint32_t module_code);
    SERVICE_NODE *FindNode(SERVICE *sp);
    SERVICE *Find(uint32_t module_code);
    SERVICE *Find(uint32_t module_code, uint32_t class_code);
    SERVICE *AddRef(uint32_t module_code);
    SERVICE *Add(uint32_t module_code, uint32_t class_code, SERVICE *pointer);
    uint32_t Release(uint32_t module_code, uint32_t class_code);
    SERVICE *GetService();
    SERVICE *GetServiceNext();

    SERVICE *GetService(uint32_t &class_code);
    SERVICE *GetServiceNext(uint32_t &class_code);

    uint32_t GetCount()
    {
        return Objects;
    };
    uint32_t GetRef(SERVICE *sp);
    void SetRef(SERVICE *sp, uint32_t ref);
};
