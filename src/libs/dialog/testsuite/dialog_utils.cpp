#include "dialog_utils.hpp"

#define CATCH_CONFIG_MAIN

#ifdef _WIN32
#define CATCH_CONFIG_WINDOWS_CRTDBG
#endif

#include <catch2/catch.hpp>

using namespace storm::dialog;

TEST_CASE("SplitIntoPages", "[dialog]")
{
    SECTION("No breaks")
    {
        const auto page_breaks = SplitIntoPages(5, 5);

        CHECK(page_breaks.empty());
    }

    SECTION("No forced breaks")
    {
        const auto page_breaks = SplitIntoPages(10, 3);

        REQUIRE(page_breaks.size() == 3);
        CHECK(page_breaks[0] == 3);
        CHECK(page_breaks[1] == 6);
        CHECK(page_breaks[2] == 9);
    }

    SECTION("Forced breaks")
    {
        const auto page_breaks = SplitIntoPages(10, 3, {2, 6});

        REQUIRE(page_breaks.size() == 4);
        CHECK(page_breaks[0] == 2);
        CHECK(page_breaks[1] == 5);
        CHECK(page_breaks[2] == 6);
        CHECK(page_breaks[3] == 9);
    }
}