#pragma once

#include <cassert>

#include <array>
#include <concepts>
#include <utility>

namespace storm
{

namespace detail
{

template <typename T>
concept power_of_two = std::unsigned_integral<T> && requires(T v)
{
    (v != 0) && ((v & (v - 1)) == 0);
};

} // namespace detail


template <typename T, detail::power_of_two auto SIZE> class ringbuffer_stack final
{
  public:
    using buffer_type = std::array<T, SIZE>;
    using index_type = decltype(SIZE);

    using value_type = typename buffer_type::value_type;
    using reference = typename buffer_type::reference;
    using const_reference = typename buffer_type::const_reference;
    using size_type = typename buffer_type::size_type;

    [[nodiscard]] reference back() noexcept
    {
        assert(!empty());
        return buf_[circular_dec(end_)];
    }

    [[nodiscard]] const_reference back() const noexcept
    {
        assert(!empty());
        return buf_[circular_dec(end_)];
    }

    [[nodiscard]] constexpr bool empty() const noexcept
    {
        return size() == 0;
    }

    [[nodiscard]] constexpr size_t size() const noexcept
    {
        return size_;
    }

    constexpr void push(T &&value) noexcept
    {
        buf_[end_] = std::move(value);
        end_ = circular_inc(end_);
        if (end_ == begin_ + 1 && size() > 0)
        {
            begin_ = circular_inc(begin_);
        }

        if (size_ < SIZE)
        {
            ++size_;
        }
    }

    constexpr void push(const T &value) noexcept
    {
        buf_[end_] = value;
        end_ = circular_inc(end_);
        if (end_ == begin_ + 1 && size() > 0)
        {
            begin_ = circular_inc(begin_);
        }

        if (size_ < SIZE)
        {
            ++size_;
        }
    }

    constexpr void pop() noexcept
    {
        assert(!empty());
        end_ = circular_dec(end_);

        --size_;
    }

  private:
    buffer_type buf_{};
    index_type size_{};
    index_type begin_{};
    index_type end_{};

    [[nodiscard]] static constexpr index_type circular_inc(index_type idx) noexcept
    {
        if (idx == SIZE - 1)
        {
            idx = 0U;
        }
        else
        {
            ++idx;
        }

        return idx;
    }

    [[nodiscard]] static constexpr index_type circular_dec(index_type idx) noexcept
    {
        if (idx == 0U)
        {
            idx = SIZE - 1;
        }
        else
        {
            --idx;
        }

        return idx;
    }
};

template <typename T, detail::power_of_two auto SIZE> class ringbuffer_stack_push_guard final
{
public:
    explicit ringbuffer_stack_push_guard(ringbuffer_stack<T, SIZE> &stack) noexcept : stack_(stack)
    {
    }

    ~ringbuffer_stack_push_guard() noexcept
    {
        while (push_count_--)
        {
            stack_.pop();
        }
    }

    void push(T&& value) noexcept
    {
        stack_.push(std::forward<T>(value));
        ++push_count_;
    }

private:
    ringbuffer_stack<T, SIZE> &stack_;
    typename ringbuffer_stack<T, SIZE>::index_type push_count_{};
};


template <typename T, detail::power_of_two auto SIZE>
ringbuffer_stack_push_guard(ringbuffer_stack<T, SIZE>) -> ringbuffer_stack_push_guard<T, SIZE>;

} // namespace storm