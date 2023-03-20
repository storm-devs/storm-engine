#pragma once

#include <algorithm>
#include <limits>
#include <ranges>
#include <type_traits>
#include <utility>
#include <vector>

namespace storm
{

template <typename Type> struct RandCstd
{
    Type operator()(Type max) noexcept
    {
        return max * static_cast<float>(std::rand()) / static_cast<float>((RAND_MAX + 1));
    }
};

template <typename Type> struct EpsilonLessThan
{
    bool operator()(Type lhs, Type rhs)
    {
        return rhs - lhs > std::numeric_limits<Type>::epsilon();
    }
};

/**
 * \brief Probability table quantizing generated random value according to its weight
 * \tparam Type - containing type
 * \tparam Resolution - resolution type
 * \tparam RandProvider - functor returning 0..1
 */
template <typename Type, typename Resolution = float, typename RandProvider = RandCstd<Resolution>>
    requires std::is_floating_point_v<Resolution>
class ProbabilityTable
{
    using element = std::pair<Resolution, Type>;

  public:
    ProbabilityTable() = default;

    template <class... Args> void emplace(Resolution weight, Args &&...args)
    {
        weight_sum_ += weight;
        table_.emplace_back(std::make_pair(weight_sum_, std::forward<Args>(args)...));
    }

    const Type &pickRandom() const
    {

        const Resolution random_value = provider_(weight_sum_);
        const auto it = std::ranges::upper_bound(table_, random_value, EpsilonLessThan<Resolution>{}, &element::first);
        return it->second;
    }

    bool empty() const
    {
        return table_.empty();
    }

  private:
    mutable RandProvider provider_;
    Resolution weight_sum_{0.0f};
    std::vector<element> table_;
};

} // namespace storm