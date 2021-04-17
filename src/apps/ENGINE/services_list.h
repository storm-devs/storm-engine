#pragma once

#include "service.h"
#include <deque>
#include <functional>
#include <tuple>

class SERVICES_LIST
{
    std::deque<std::tuple<SERVICE *, uint32_t>> list;

  public:
    void Release();

    SERVICE *Add(uint32_t class_code, SERVICE *pointer);

    void ForEachService(std::function<void(SERVICE *, uint32_t)> f);
};
