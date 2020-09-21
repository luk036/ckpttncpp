#include <chrono>
#include <ckpttncpp/FMBiConstrMgr.hpp>   // import FMBiConstrMgr
#include <ckpttncpp/FMBiGainMgr.hpp>     // import FMBiGainMgr
#include <ckpttncpp/FMKWayConstrMgr.hpp> // import FMKWayConstrMgr
#include <ckpttncpp/FMKWayGainMgr.hpp>   // import FMKWayGainMgr
#include <ckpttncpp/FMPartMgr.hpp>       // import FMBiPartMgr
#include <ckpttncpp/MLPartMgr.hpp>       // import MLBiPartMgr
#include <doctest.h>
// #include <experimental/random>
#include <boost/utility/string_view.hpp>
#include <iostream>

// using std::experimental::randint;

extern SimpleNetlist create_test_netlist(); // import create_test_netlist
extern SimpleNetlist create_dwarf();        // import create_dwarf
extern SimpleNetlist readNetD(boost::string_view netDFileName);
extern void readAre(SimpleNetlist& H, boost::string_view areFileName);

TEST_CASE("Test MLBiPartMgr dwarf")
{
    const auto H = create_dwarf();
    auto partMgr = MLPartMgr {0.3};
    auto part = std::vector<std::uint8_t>(H.number_of_modules(), 0);
    partMgr.run_FMPartition<FMPartMgr<FMBiGainMgr, FMBiConstrMgr>>(H, part);
    CHECK(partMgr.totalcost == 2);
}

TEST_CASE("Test MLKWayPartMgr dwarf")
{
    const auto H = create_dwarf();
    auto partMgr = MLPartMgr {0.4, 3}; // 0.3???
    auto part = std::vector<std::uint8_t>(H.number_of_modules(), 0);
    partMgr.run_FMPartition<FMPartMgr<FMKWayGainMgr, FMKWayConstrMgr>>(H, part);
    CHECK(partMgr.totalcost == 4);
}

TEST_CASE("Test MLBiPartMgr p1")
{
    const auto H = readNetD("../../../testcases/p1.net");
    auto partMgr = MLPartMgr {0.3};

    auto mincost = 1000;
    for (auto i = 0; i != 10; ++i)
    {
        auto part = std::vector<std::uint8_t>(H.number_of_modules(), 0);
        auto whichPart = 0U;
        for (auto&& elem : part)
        {
            whichPart ^= 1;
            elem = whichPart;
        }
        partMgr.run_FMPartition<FMPartMgr<FMBiGainMgr, FMBiConstrMgr>>(
            H, part, 100);
        if (mincost > partMgr.totalcost)
        {
            mincost = partMgr.totalcost;
        }
    }
    // CHECK(partMgr.totalcost >= 50);
    // CHECK(partMgr.totalcost <= 50);
    CHECK(mincost >= 29);
    CHECK(mincost <= 73);
}

TEST_CASE("Test MLBiPartMgr ibm01")
{
    auto H = readNetD("../../../testcases/ibm01.net");
    readAre(H, "../../../testcases/ibm01.are");
    auto partMgr = MLPartMgr {0.4};
    auto mincost = 1000;
    for (auto i = 0; i != 10; ++i)
    {
        auto part = std::vector<std::uint8_t>(H.number_of_modules(), 0);
        auto whichPart = 0U;
        for (auto&& elem : part)
        {
            whichPart ^= 1;
            elem = whichPart;
        }
        partMgr.run_FMPartition<FMPartMgr<FMBiGainMgr, FMBiConstrMgr>>(
            H, part, 400);
        if (mincost > partMgr.totalcost)
        {
            mincost = partMgr.totalcost;
        }
    }
    // CHECK(partMgr.totalcost >= 650);
    // CHECK(partMgr.totalcost <= 650);
    CHECK(mincost >= 221);
    CHECK(mincost <= 695);
}

TEST_CASE("Test MLBiPartMgr ibm03")
{
    auto H = readNetD("../../../testcases/ibm03.net");
    readAre(H, "../../../testcases/ibm03.are");
    auto partMgr = MLPartMgr {0.45};
    auto part = std::vector<std::uint8_t>(H.number_of_modules(), 0);
    // auto part_info = PartInfo{std::move(part), py::set<node_t>()};
    auto begin = std::chrono::steady_clock::now();
    partMgr.run_FMPartition<FMPartMgr<FMBiGainMgr, FMBiConstrMgr>>(
        H, part, 300);
    std::chrono::duration<double> last =
        std::chrono::steady_clock::now() - begin;
    std::cout << "time: " << last.count() << std::endl;
    CHECK(partMgr.totalcost >= 1278);
    CHECK(partMgr.totalcost <= 2041);
}

/*

Advantages:

1. Python-like, networkx
2. Check legalization, report
3. Generic
4. K buckets rather than K - 1
5. Time, space, and code complexity.
6. Design issues

*/
