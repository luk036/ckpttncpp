#include <catch.hpp>
#include <ckpttncpp/FMBiConstrMgr.hpp>   // import FMBiConstrMgr
#include <ckpttncpp/FMBiGainMgr.hpp>     // import FMBiGainMgr
#include <ckpttncpp/FMKWayConstrMgr.hpp> // import FMKWayConstrMgr
#include <ckpttncpp/FMKWayGainMgr.hpp>   // import FMKWayGainMgr
#include <ckpttncpp/FMPartMgr.hpp>       // import FMBiPartMgr
#include <ckpttncpp/MLPartMgr.hpp>       // import MLBiPartMgr
#include <experimental/random>

extern SimpleNetlist create_test_netlist(); // import create_test_netlist
extern SimpleNetlist create_dwarf();        // import create_dwarf
extern SimpleNetlist readNetD(const char *netDFileName);
extern void readAre(SimpleNetlist &H, const char *areFileName);

TEST_CASE("Test MLBiPartMgr dwarf", "[test_MLBiPartMgr]") {
    auto H = create_dwarf();
    auto partMgr = MLPartMgr{0.3};
    auto part = std::vector<uint8_t>(H.number_of_modules(), 0);
    auto part_info = PartInfo{std::move(part), py::set<node_t>()};
    partMgr.run_FMPartition<FMPartMgr<FMBiGainMgr, FMBiConstrMgr>>(H,
                                                                   part_info);
    CHECK(partMgr.totalcost == 2);
}

TEST_CASE("Test MLKWayPartMgr dwarf", "[test_MLKWayPartMgr]") {
    auto H = create_dwarf();
    auto partMgr = MLPartMgr{0.4, 3}; // 0.3???
    auto part = std::vector<uint8_t>(H.number_of_modules(), 0);
    auto part_info = PartInfo{std::move(part), py::set<node_t>()};
    partMgr.run_FMPartition<FMPartMgr<FMKWayGainMgr, FMKWayConstrMgr>>(
        H, part_info);
    CHECK(partMgr.totalcost == 4);
}

TEST_CASE("Test MLBiPartMgr p1", "[test_MLBiPartMgr]") {
    auto H = readNetD("../../testcases/p1.net");
    auto partMgr = MLPartMgr{0.3};

    auto mincost = 1000;
    for (auto i=0; i<10; ++i) {
        auto part = std::vector<uint8_t>(H.number_of_modules(), 0);
        for (auto& elem: part) {
            elem = std::experimental::randint(0, 1);
        }
        auto part_info = PartInfo{std::move(part), py::set<node_t>()};
        partMgr.run_FMPartition<FMPartMgr<FMBiGainMgr, FMBiConstrMgr>>(H, part_info,
                                                                   100);
        if (mincost > partMgr.totalcost) {
            mincost = partMgr.totalcost;
        }
    }
    // CHECK(partMgr.totalcost >= 50);
    // CHECK(partMgr.totalcost <= 50);
    CHECK(mincost >= 30);
    CHECK(mincost <= 46);
}

TEST_CASE("Test MLBiPartMgr ibm01", "[test_MLBiPartMgr]") {
    auto H = readNetD("../../testcases/ibm01.net");
    readAre(H, "../../testcases/ibm01.are");
    auto partMgr = MLPartMgr{0.4};
    auto mincost = 1000;
    for (auto i=0; i<10; ++i) {
        auto part = std::vector<uint8_t>(H.number_of_modules(), 0);
        for (auto& elem: part) {
            elem = std::experimental::randint(0, 1);
        }
        auto part_info = PartInfo{std::move(part), py::set<node_t>()};
        partMgr.run_FMPartition<FMPartMgr<FMBiGainMgr, FMBiConstrMgr>>(H, part_info,
                                                                   400);
        if (mincost > partMgr.totalcost) {
            mincost = partMgr.totalcost;
        }
    }
    // CHECK(partMgr.totalcost >= 650);
    // CHECK(partMgr.totalcost <= 650);
    CHECK(mincost >= 230);
    CHECK(mincost <= 291);
}

TEST_CASE("Test MLBiPartMgr ibm03", "[test_MLBiPartMgr]") {
    auto H = readNetD("../../testcases/ibm03.net");
    readAre(H, "../../testcases/ibm03.are");
    auto partMgr = MLPartMgr{0.45};
    auto part = std::vector<uint8_t>(H.number_of_modules(), 0);
    auto part_info = PartInfo{std::move(part), py::set<node_t>()};
    partMgr.run_FMPartition<FMPartMgr<FMBiGainMgr, FMBiConstrMgr>>(H, part_info,
                                                                   300);
    CHECK(partMgr.totalcost >= 1469);
    CHECK(partMgr.totalcost <= 2041);
}

/*

Advantages:

1. Python-like, networkx
2. Check legalization, report
3. Generic
4. K buckets rather than K(K-1)
5. Time, space, and code complexity.
6. Design issues

*/