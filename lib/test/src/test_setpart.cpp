#include <array>
#include <catch.hpp>
#include <ckpttncpp/set_partition.hpp> // import  setpart
// #include <iostream>

TEST_CASE("Test set partition", "[set_partition]")
{
    constexpr auto n = 5;
    constexpr auto k = 3;
    auto cnt = 1;

    std::array<int, n + 1> b {0, 0, 0, 0, 1, 2};

    for (auto [x, y] : set_partition(n, k))
    {
        // auto old = b[x];
        b[x] = y;
        cnt += 1;
        // std::cout << "Move " << x << " from block " << old << " to " << y
        //           << "\n";
    }

    auto totalNumber = Stirling2nd<n, k>();
    CHECK(totalNumber == cnt);
}