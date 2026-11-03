// Unit tests for the shared input checks in Validation.h.
#include <catch2/catch_test_macros.hpp>

#include "Errors.h"
#include "Validation.h"

TEST_CASE("UT-23 Marks in messages are shown without extra zeros (regression test for DEF-02)",
          "[unit][validation][regression]") {
    CHECK(formatNumber(50) == "50");
    CHECK(formatNumber(37.5) == "37.5");
    CHECK(formatNumber(0) == "0");
}

TEST_CASE("UT-24 Blank text is rejected, real text is accepted", "[unit][validation]") {
    CHECK(isBlank(""));
    CHECK(isBlank("   \t\n"));
    CHECK_FALSE(isBlank(" a "));
    CHECK_THROWS_AS(requireNotBlank("  ", "Reason"), ValidationError);
    CHECK_NOTHROW(requireNotBlank("Sick note", "Reason"));
}
