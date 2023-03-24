#include "probability_table.hpp"

#include <catch2/catch.hpp>

using namespace storm;

TEST_CASE("EpsilonLessThan", "[utils]")
{
    using Compare = EpsilonLessThan<float>;

    constexpr auto eps = std::numeric_limits<float>::epsilon();

    SECTION("Less")
    {
        CHECK(Compare{}(0.0f, 1.0f));
        CHECK(Compare{}(1.0f - 2 * eps, 1.0f));
        CHECK(Compare{}(-1.0f, 0.0f));
    }

    SECTION("EqualOrGreater")
    {
        CHECK_FALSE(Compare{}(1.0f, 1.0f));
        CHECK_FALSE(Compare{}(1.5f, 1.0f));
        CHECK_FALSE(Compare{}(1.0f + eps, 1.0f));
    }
}

template <typename T> class RandProviderMock
{
  public:
    T operator()(T max_excluded)
    {
        if (ret_max_)
        {
            return max_excluded - (max_excluded > 1.0f ? max_excluded : 1.0f) * std::numeric_limits<T>::epsilon();
        }
        return ret_;
    }

    static void ret(T val)
    {
        ret_ = val;
    }

    static void ret_max(bool val)
    {
        ret_max_ = val;
    }

  private:
    static inline bool ret_max_{false};
    static inline T ret_{};
};

TEST_CASE("ProbabilityTable", "[utils]")
{
    using ProviderMock = RandProviderMock<float>;

    ProbabilityTable<size_t, float, ProviderMock> table_;
    table_.emplace(1.0f, 0); // 1.0
    table_.emplace(1.0f, 1); // 2.0
    table_.emplace(0.8f, 2); // 2.8
    table_.emplace(0.7f, 3); // 3.5
    table_.emplace(0.5f, 4); // 4.0

    SECTION("0")
    {
        ProviderMock::ret_max(false);
        ProviderMock::ret(0.0f);
        CHECK(table_.pickRandom() == 0);
        ProviderMock::ret(0.5f);
        CHECK(table_.pickRandom() == 0);
        ProviderMock::ret(0.99f);
        CHECK(table_.pickRandom() == 0);
    }

    SECTION("1")
    {
        ProviderMock::ret_max(false);
        ProviderMock::ret(1.0f);
        CHECK(table_.pickRandom() == 1);
        ProviderMock::ret(1.5f);
        CHECK(table_.pickRandom() == 1);
        ProviderMock::ret(1.99f);
        CHECK(table_.pickRandom() == 1);
    }

    SECTION("2")
    {
        ProviderMock::ret_max(false);
        ProviderMock::ret(2.0f);
        CHECK(table_.pickRandom() == 2);
        ProviderMock::ret(2.5f);
        CHECK(table_.pickRandom() == 2);
        ProviderMock::ret(2.79f);
        CHECK(table_.pickRandom() == 2);
    }

    SECTION("3")
    {
        ProviderMock::ret_max(false);
        ProviderMock::ret(2.88f);
        CHECK(table_.pickRandom() == 3);
        ProviderMock::ret(3.0f);
        CHECK(table_.pickRandom() == 3);
        ProviderMock::ret(3.49f);
        CHECK(table_.pickRandom() == 3);
    }

    SECTION("4")
    {
        ProviderMock::ret_max(false);
        ProviderMock::ret(3.5f);
        CHECK(table_.pickRandom() == 4);
        ProviderMock::ret(3.9f);
        CHECK(table_.pickRandom() == 4);
        ProviderMock::ret_max(true);
        CHECK(table_.pickRandom() == 4);
    }

    SECTION("5")
    {
        auto extended = table_;
        extended.emplace(10000000.0f, 5);
        ProviderMock::ret_max(true);
        CHECK(extended.pickRandom() == 5);
    }
}
