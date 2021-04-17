#include "services_list.h"
#include <cstdint>
#include <stdexcept>

SERVICE *SERVICES_LIST::Add(uint32_t class_code, SERVICE *pointer)
{
    for (auto [pointer, handle_class_code] : this->list)
    {
        if (handle_class_code == class_code)
        {
            return pointer;
        }
    }

    this->list.emplace_front(pointer, class_code);

    return pointer;
}

void SERVICES_LIST::Release()
{
    this->list.clear();
}

void SERVICES_LIST::ForEachService(std::function<void(SERVICE *, uint32_t)> f)
{
    for (auto [pointer, class_code] : this->list)
    {
        f(pointer, class_code);
    }
}
