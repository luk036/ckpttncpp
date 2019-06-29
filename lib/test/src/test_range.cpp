#include <catch.hpp>
#include <py2cpp/py2cpp.hpp>

TEST_CASE("Test Range", "[range]")
{
    auto R = py::range(0u, 10u);

    CHECK(!R.empty());
    CHECK(R.contains(4u));
    CHECK(R[3] == 3u);

    auto count = 0;
    for (auto a : R)
    {
        ++count;
    }
    CHECK(count == R.size());
}

TEST_CASE("Test Range2", "[range]")
{
    auto R = py::range(-10, 10);

    CHECK(!R.empty());
    CHECK(R.contains(4));
    CHECK(R[3] == -7);

    auto count = 0;
    for (auto a : R)
    {
        ++count;
    }
    CHECK(count == R.size());
}

TEST_CASE("Test Range (char)", "[range]")
{
    auto R = py::range('A', 'W');

    CHECK(!R.empty());
    CHECK(R.contains('B'));
    CHECK(R[3] == 'D');

    auto count = 0;
    for (auto a : R)
    {
        ++count;
    }
    CHECK(count == R.size());
}

TEST_CASE("Test Range (pointer)", "[range]")
{
    double A[] = {0.2, 0.4, 0.1, 0.9};
    auto   R   = py::range(&A, &A + 4);

    CHECK(!R.empty());
    CHECK(R.contains(&A + 2));
    CHECK(R[1] == &A + 1);

    auto count = 0;
    for (auto _ : R)
    {
        ++count;
    }
    CHECK(count == R.size());
}
