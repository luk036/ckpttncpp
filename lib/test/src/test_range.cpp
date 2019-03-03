#include <catch.hpp>
#include <py2cpp/py2cpp.hpp>

TEST_CASE("Test Range", "[range]") {
    auto R = py::range(10u);

    CHECK(!R.empty());
    CHECK(R.contains(4));
    CHECK(R[3] == 3);

    auto count = 0;
    for (auto a : R) {
        ++count;
    }
    CHECK(count == R.size());
}

TEST_CASE("Test Range2", "[range2]") {
    auto R = py::range2(-10, 10);

    CHECK(!R.empty());
    CHECK(R.contains(4));
    CHECK(R[3] == -7);

    auto count = 0;
    for (auto a : R) {
        ++count;
    }
    CHECK(count == R.size());
}
