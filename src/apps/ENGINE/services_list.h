#pragma once

#include "service.h"

struct SERVICE_NODE
{
    uint32_t module_code;
    uint32_t class_code;
    SERVICE *pointer;
    SERVICE_NODE *linkL;
};

class SERVICES_LIST
{
    SERVICE_NODE *List;
    uint32_t Search_module_code;

    SERVICE_NODE *FindNode(uint32_t module_code);
    SERVICE_NODE *FindNode(SERVICE *sp);

  public:
    SERVICES_LIST();
    ~SERVICES_LIST();

    void Release();

    SERVICE *Add(uint32_t module_code, uint32_t class_code, SERVICE *pointer);

    SERVICE *GetService(uint32_t &class_code);
    SERVICE *GetServiceNext(uint32_t &class_code);
};
