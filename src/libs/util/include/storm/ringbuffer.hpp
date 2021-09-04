#pragma once

#include <cassert>
#include <cstddef>

#include <list>
#include <type_traits>
#include <utility>

namespace storm
{

template <typename T> class ringbuffer
{
    using buffer_type = std::list<T>;

  public:
    // type traits
    using value_type = typename buffer_type::value_type;
    using reference = typename buffer_type::reference;
    using const_reference = typename buffer_type::const_reference;
    using size_type = typename buffer_type::size_type;

    // stack container traits
    [[nodiscard]] reference front() noexcept
    {
        return buf_.front();
    }

    [[nodiscard]] const_reference front() const noexcept
    {
        return buf_.front();
    }

    [[nodiscard]] reference back() noexcept
    {
        return buf_.back();
    }

    [[nodiscard]] const_reference back() const noexcept
    {
        return buf_.back();
    }

    [[nodiscard]] constexpr bool empty() const noexcept
    {
        return size() == 0U;
    }

    [[nodiscard]] constexpr size_t size() const noexcept
    {
        return buf_.size();
    }

    constexpr void push_back(T &&value) noexcept
    {
        if (size() == size_)
        {
            assert(size_ > 0U);
            buf_.pop_front();
        }

        buf_.push_back(std::forward<T>(value));
    }

    constexpr void pop_back() noexcept
    {
        assert(size() > 0U);
        buf_.pop_back();
    }

    // non-trait members
    void set_size(const size_t size) noexcept
    {
        size_ = size;
    }

  private:
    size_t size_{0U};
    buffer_type buf_;
};

} // namespace storm