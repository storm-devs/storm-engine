#include "storm/string_compare.hpp"

#define CATCH_CONFIG_MAIN

#ifdef _WIN32
#define CATCH_CONFIG_WINDOWS_CRTDBG
#endif

#include <catch2/catch.hpp>

TEST_CASE("Case-insensitive string comparison", "[utils]")
{
    using namespace storm;
    using namespace std::string_literals;

    const auto str_lowercase = "mystring"s;
    const auto str_uppercase = "MYSTRING"s;
    const auto str_mixedcase = "mYsTrInG"s;

    const auto str_other = "m_string"s;
    const auto str_long = "mystrings"s;

    SECTION("iEquals")
    {
        SECTION("Strings with same case should be considered equal")
        {
            CHECK(iEquals(str_lowercase, str_lowercase));
            CHECK(iEquals(str_uppercase, str_uppercase));
            CHECK(iEquals(str_mixedcase, str_mixedcase));
        }

        SECTION("Strings with different case should be considered equal")
        {
            CHECK(iEquals(str_lowercase, str_uppercase));
            CHECK(iEquals(str_lowercase, str_mixedcase));
            CHECK(iEquals(str_uppercase, str_mixedcase));
        }

        SECTION("Strings with different value should be considered not equal")
        {
            CHECK_FALSE(iEquals(str_lowercase, str_other));
        }

        SECTION("Strings with different length should be considered not equal")
        {
            CHECK_FALSE(iEquals(str_lowercase, str_long));
        }

        SECTION("Compare with raw char string literal")
        {
            CHECK(iEquals(str_lowercase, "myString"));

            const char string_array[] = "myString";
            CHECK(iEquals(string_array, str_lowercase));

        }

        SECTION("Compare only count number of character at most")
        {
            CHECK(iEquals(str_lowercase, str_long, 8));
            CHECK(iEquals(str_long, str_lowercase, 8));
            CHECK_FALSE(iEquals(".txt1", ".txt2", 5));
        }
    }

    SECTION("iLess")
    {
        SECTION("Identical string should not be less")
        {
            CHECK_FALSE(iLess(str_lowercase, str_lowercase));
            CHECK_FALSE(iLess(str_uppercase, str_uppercase));
            CHECK_FALSE(iLess(str_mixedcase, str_mixedcase));
        }

        SECTION("Lowercase should not be considered before uppercase")
        {
            CHECK_FALSE(iLess(str_lowercase, str_uppercase));
            CHECK_FALSE(iLess(str_uppercase, str_lowercase));
            CHECK_FALSE(iLess(str_lowercase, str_mixedcase));
            CHECK_FALSE(iLess(str_mixedcase, str_lowercase));
            CHECK_FALSE(iLess(str_mixedcase, str_uppercase));
            CHECK_FALSE(iLess(str_uppercase, str_mixedcase));
        }

        SECTION("Characters that appear earlier in the alphabet should be cosidered less than character after it")
        {
            CHECK(iLess("A", "b"));
            CHECK_FALSE(iLess("c", "B"));
        }

        SECTION("Shorter string should be considered before longer string, when they have the same starting sequence")
        {
            CHECK(iLess(str_lowercase, str_long));
            CHECK_FALSE(iLess(str_long, str_lowercase));
        }
    }
}
