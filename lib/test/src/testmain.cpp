#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#include <array>
#include <gsl/span>
#include <vector>
#include <ckpttncpp/array_like.hpp>

TEST_CASE("Test span")
{
    auto k = std::array<int, 3> {3, 4, 5};
    auto s = gsl::span<int> {k};
    auto dumb = get_repeat_array(1, 100);
    CHECK(dumb[3] == 1);

    auto shift = shift_array(std::vector<int>{1, 4, 9, 16, 25});
    shift.set_start(3);
    CHECK(shift[6] == 16);
}
