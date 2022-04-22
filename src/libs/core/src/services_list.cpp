#include "services_list.h"
#include <cstdint>
#include <stdexcept>

SERVICES_LIST::SERVICES_LIST()
{
    List = nullptr;
    Search_module_code = 0;
    Objects = 0;
}

SERVICES_LIST::~SERVICES_LIST()
{
    Release();
}

SERVICE *SERVICES_LIST::Add(uint32_t module_code, uint32_t class_code, SERVICE *pointer)
{
    auto *const Pointer = AddRef(module_code);

    if (Pointer)
        return Pointer;

    auto *node_PTR = (SERVICE_NODE *)new char[sizeof(SERVICE_NODE)];
    memset(node_PTR, 0, sizeof(SERVICE_NODE));
    node_PTR->module_code = module_code;
    node_PTR->class_code = class_code;
    node_PTR->pointer = pointer;
    node_PTR->reference = 1;

    Objects++;

    if (List == nullptr)
    {
        // add first node ------------------------------------------
        List = node_PTR;
        return node_PTR->pointer;
    }

    node_PTR->linkL = List;
    List->linkR = node_PTR;
    List = node_PTR;
    return node_PTR->pointer;
}

SERVICE *SERVICES_LIST::AddRef(uint32_t module_code)
{
    auto *const node_PTR = FindNode(module_code);
    if (node_PTR == nullptr)
        return nullptr;
    node_PTR->reference++;
    return node_PTR->pointer;
}

SERVICE *SERVICES_LIST::Find(uint32_t module_code)
{
    auto *node_PTR = List;
    while (node_PTR)
    {
        if (node_PTR->module_code == module_code)
            return node_PTR->pointer;
        node_PTR = static_cast<SERVICE_NODE *>(node_PTR->linkL);
    }
    return nullptr;
}

SERVICE *SERVICES_LIST::Find(uint32_t module_code, uint32_t class_code)
{
    auto *node_PTR = List;
    while (node_PTR)
    {
        if (node_PTR->module_code == module_code)
        {
            if (node_PTR->class_code == class_code)
                return node_PTR->pointer;
        }
        node_PTR = static_cast<SERVICE_NODE *>(node_PTR->linkL);
    }
    return nullptr;
}

SERVICE_NODE *SERVICES_LIST::FindNode(uint32_t module_code)
{
    auto *node_PTR = List;
    while (node_PTR)
    {
        if (node_PTR->module_code == module_code)
            return node_PTR;
        node_PTR = static_cast<SERVICE_NODE *>(node_PTR->linkL);
    }
    return nullptr;
}

SERVICE_NODE *SERVICES_LIST::FindNode(SERVICE *sp)
{
    SERVICE_NODE *node_PTR = List;
    while (node_PTR)
    {
        if (node_PTR->pointer == sp)
            return node_PTR;
        node_PTR = static_cast<SERVICE_NODE *>(node_PTR->linkL);
    }
    return nullptr;
}

uint32_t SERVICES_LIST::Release(uint32_t module_code, uint32_t class_code)
{
    uint32_t ref;

    SERVICE_NODE *node_PTR = List;
    while (node_PTR)
    {
        if (node_PTR->module_code == module_code)
        {
            if (node_PTR->class_code == class_code)
            {
                if (node_PTR->reference > 0)
                    node_PTR->reference--;
                ref = node_PTR->reference;
                if (node_PTR->reference == 0)
                {
                    SERVICE *pointer = node_PTR->pointer;
                    auto *node_L = static_cast<SERVICE_NODE *>(node_PTR->linkL);
                    auto *node_R = static_cast<SERVICE_NODE *>(node_PTR->linkR);
                    if (node_L)
                        node_L->linkR = node_R;
                    if (node_R)
                        node_R->linkL = node_L;
                    if (node_PTR == List)
                        List = node_L;
                    delete node_PTR;
                    Objects--;
                    return 0;
                }
            }
            return ref;
        }
        node_PTR = static_cast<SERVICE_NODE *>(node_PTR->linkL);
    }
    return 0;
}

void SERVICES_LIST::Release()
{
    SERVICE_NODE *node_PTR = List;
    while (node_PTR)
    {
        List = static_cast<SERVICE_NODE *>(node_PTR->linkL);
        delete node_PTR;
        node_PTR = List;
    }

    List = nullptr;
}

SERVICE *SERVICES_LIST::GetService()
{
    if (List)
    {
        Search_module_code = List->module_code;
        return List->pointer;
    }
    return nullptr;
}

SERVICE *SERVICES_LIST::GetServiceNext()
{
    SERVICE_NODE *node_PTR = List;
    while (node_PTR)
    {
        if (node_PTR->module_code == Search_module_code)
        {
            auto *node_L = static_cast<SERVICE_NODE *>(node_PTR->linkL);
            if (node_L)
            {
                Search_module_code = node_L->module_code;
                return node_L->pointer;
            }
            return nullptr;
        }
        node_PTR = static_cast<SERVICE_NODE *>(node_PTR->linkL);
    }
    return nullptr;
}

SERVICE *SERVICES_LIST::GetService(uint32_t &class_code)
{
    if (List)
    {
        Search_module_code = List->module_code;
        class_code = List->class_code;
        return List->pointer;
    }
    return nullptr;
}

SERVICE *SERVICES_LIST::GetServiceNext(uint32_t &class_code)
{
    SERVICE_NODE *node_PTR = List;
    while (node_PTR)
    {
        if (node_PTR->module_code == Search_module_code)
        {
            auto *node_L = static_cast<SERVICE_NODE *>(node_PTR->linkL);
            if (node_L)
            {
                Search_module_code = node_L->module_code;
                class_code = node_L->class_code;
                return node_L->pointer;
            }
            return nullptr;
        }
        node_PTR = static_cast<SERVICE_NODE *>(node_PTR->linkL);
    }
    return nullptr;
}

uint32_t SERVICES_LIST::GetRef(SERVICE *sp)
{
    SERVICE_NODE *node_PTR = FindNode(sp);
    if (node_PTR == nullptr)
        throw std::runtime_error("no service");
    return node_PTR->reference;
}

void SERVICES_LIST::SetRef(SERVICE *sp, uint32_t ref)
{
    SERVICE_NODE *node_PTR = FindNode(sp);
    if (node_PTR == nullptr)
        throw std::runtime_error("no service");
    node_PTR->reference = ref;
}
