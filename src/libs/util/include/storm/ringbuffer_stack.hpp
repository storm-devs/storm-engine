#pragma once

#include <stack>
#include <type_traits>

#include "ringbuffer.hpp"

namespace storm
{

template <typename T>
class ringbuffer_stack : public std::stack<T, ringbuffer<T>> {
public:
    explicit ringbuffer_stack(const size_t size)
    {
        std::stack<T, ringbuffer<T>>::c.set_size(size);
    }
};

template <typename T>
class guarded_ringbuffer_stack : public ringbuffer_stack<T>
{
public:
    using ringbuffer_stack<T>::ringbuffer_stack;
    
    class guard final
    {
    public:
        explicit guard(ringbuffer_stack<T> &stack) noexcept : stack_(stack)
        {
        }

        guard(guard &&guard) = delete;
        guard& operator=(guard &&guard) = delete;

        ~guard() noexcept
        {
            stack_.pop();
        }

      private:
        ringbuffer_stack<T> &stack_;
    };

    [[nodiscard, maybe_unused]] guard push(T&&val) noexcept(noexcept(ringbuffer_stack<T>::push(val)))
    {
        ringbuffer_stack<T>::push(std::forward<T>(val));
        return guard(*this);
    }
};

} // namespace storm