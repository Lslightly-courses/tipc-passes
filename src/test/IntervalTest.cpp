#include <catch2/catch_test_macros.hpp>

#include "Interval.h"

using namespace interval;

TEST_CASE("Interval Test", "[intervaltest]") {
    std::set<int> s{2,42,minf,pinf};
    auto exp = make(42, pinf);
    auto i = make(44, 44);
    auto res = widen(i, s);
    REQUIRE(res == exp);
}


