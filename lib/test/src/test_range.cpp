#include <array>
#include <doctest.h>
#include <py2cpp/py2cpp.hpp>

TEST_CASE("Test Range")
{
    const auto R = py::range(0U, 10U);

    CHECK(not R.empty());
    CHECK(R.contains(4U));
    CHECK(R[3] == 3U);

    auto count = 0;
    for ([[maybe_unused]] auto a : R)
    {
        ++count;
    }
    CHECK(count == R.size());
}

TEST_CASE("Test Range2")
{
    const auto R = py::range(-10, 10);

    CHECK(not R.empty());
    CHECK(R.contains(4));
    CHECK(R[3] == -7);

    auto count = 0;
    for ([[maybe_unused]] auto a : R)
    {
        ++count;
    }
    CHECK(count == R.size());
}

TEST_CASE("Test Range (char)")
{
    const auto R = py::range('A', 'W');

    CHECK(not R.empty());
    CHECK(R.contains('B'));
    CHECK(R[3] == 'D');

    auto count = 0;
    for ([[maybe_unused]] auto a : R)
    {
        ++count;
    }
    CHECK(count == R.size());
}

TEST_CASE("Test Range (pointer)")
{
    auto A = std::array {0.2, 0.4, 0.1, 0.9};
    auto R = py::range(&A, &A + 4);

    CHECK(not R.empty());
    CHECK(R.contains(&A + 2));
    CHECK(R[1] == &A + 1);

    auto count = 0;
    for ([[maybe_unused]] auto _ : R)
    {
        ++count;
    }
    CHECK(count == R.size());
}
