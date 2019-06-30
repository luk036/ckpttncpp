#include <catch.hpp>
#include <ckpttncpp/FMKWayConstrMgr.hpp> // import FMKWayConstrMgr
#include <ckpttncpp/FMKWayGainMgr.hpp>   // import FMKWayGainMgr
#include <ckpttncpp/FMPartMgr.hpp>       // import FMKWayPartMgr

extern SimpleNetlist create_test_netlist(); // import create_test_netlist
extern SimpleNetlist create_dwarf();        // import create_dwarf
extern SimpleNetlist readNetD(const char* netDFileName);
extern void          readAre(SimpleNetlist& H, const char* areFileName);

/**
 * @brief Run test cases
 *
 * @param H
 * @param K
 */
void run_FMKWayPartMgr(SimpleNetlist& H, uint8_t K)
{
    auto gainMgr   = FMKWayGainMgr{H, K};
    auto constrMgr = FMKWayConstrMgr{H, 0.4, K};
    auto partMgr   = FMPartMgr{H, gainMgr, constrMgr};
    auto part      = std::vector<uint8_t>(H.number_of_modules(), 0);
    // auto part_info = PartInfo{std::move(part), py::set<node_t>{}};
    partMgr.legalize(part);
    auto totalcostbefore = partMgr.totalcost;
    partMgr.optimize(part);
    CHECK(totalcostbefore >= 0);
    CHECK(partMgr.totalcost <= totalcostbefore);
    CHECK(partMgr.totalcost >= 0);
}

TEST_CASE("Test FMKWayPartMgr", "[test_FMKWayPartMgr]")
{
    auto H = create_dwarf();
    run_FMKWayPartMgr(H, 3);
}

TEST_CASE("Test FMKWayPartMgr p1", "[test_FMKWayPartMgr]")
{
    auto H = readNetD("../../../testcases/p1.net");
    run_FMKWayPartMgr(H, 3);
}

// TEST_CASE("Test FMKWayPartMgr ibm01", "[test_FMKWayPartMgr]") {
//     auto H = readNetD("../../../testcases/ibm01.net");
//     readAre(H, "../../../testcases/ibm01.are");
//     run_FMKWayPartMgr(H, 3);
// }
