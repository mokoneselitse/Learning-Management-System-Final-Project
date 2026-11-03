// Unit tests for Assignment::effectiveDeadlineFor (FR06).
#include <catch2/catch_test_macros.hpp>

#include "Assignment.h"
#include "Extension.h"
#include "TestHelpers.h"

TEST_CASE("UT-22 Effective deadline uses a later extension, otherwise the original",
          "[unit][deadline][FR06]") {
    Timestamp d = testDeadline();
    Assignment a(1, 1, "Project", "", d, 50, 2);

    CHECK(a.effectiveDeadlineFor(nullptr) == d);  // no extension

    Extension later(1, 1, 5, d + hours(48), "sick", 2, d);
    CHECK(a.effectiveDeadlineFor(&later) == d + hours(48));

    Extension earlier(2, 1, 5, d - hours(1), "mistake", 2, d);
    CHECK(a.effectiveDeadlineFor(&earlier) == d);  // an earlier date never shortens it
}

TEST_CASE("UT-25 Effective deadline is the latest of original, class and individual (CR1)",
          "[unit][deadline][CR1][FR24]") {
    Timestamp d = testDeadline();
    Assignment a(1, 1, "Project", "", d, 50, 2);
    ClassExtension cls(1, d + hours(24), "Strike", 2, d);
    Extension mine(1, 1, 5, d + hours(48), "Sick", 2, d);
    Extension shortOne(2, 1, 6, d + hours(12), "Sick", 2, d);

    CHECK(a.effectiveDeadlineFor(&cls, nullptr) == d + hours(24));  // class only
    CHECK(a.effectiveDeadlineFor(&cls, &mine) == d + hours(48));    // individual is later
    CHECK(a.effectiveDeadlineFor(&cls, &shortOne) == d + hours(24));  // class is later
    CHECK(a.effectiveDeadlineFor(nullptr, nullptr) == d);
    CHECK(a.effectiveDeadlineFor(nullptr, &mine) == a.effectiveDeadlineFor(&mine));  // v1.0 call
}
