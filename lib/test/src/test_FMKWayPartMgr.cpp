#include <catch.hpp>
#include <ckpttncpp/FMKWayGainMgr.hpp>   // import FMKWayGainMgr
#include <ckpttncpp/FMKWayConstrMgr.hpp> // import FMKWayConstrMgr
#include <ckpttncpp/FMPartMgr.hpp>      // import FMKWayPartMgr

extern SimpleNetlist create_test_netlist(); // import create_test_netlist
extern SimpleNetlist create_dwarf(); // import create_dwarf
extern SimpleNetlist readNetD(const char *netDFileName);
void readAre(SimpleNetlist & H, const char *areFileName);

/**
 * @brief Run test cases
 * 
 * @param H 
 * @param K 
 */
void run_FMKWayPartMgr(SimpleNetlist & H, std::uint8_t K) {
    auto gainMgr = FMKWayGainMgr{H, K};
    auto constrMgr = FMKWayConstrMgr{H, 0.45, K};
    // CHECK(H.G.nodes[0].get('weight', 1) == 5844);
    auto partMgr = FMPartMgr{H, gainMgr, constrMgr};
    auto part = std::vector<uint8_t>(H.number_of_modules(), 0);
    partMgr.init(part);
    partMgr.legalize(part);
    auto totalcostbefore = partMgr.totalcost;
    partMgr.optimize(part);
    CHECK(partMgr.totalcost <= totalcostbefore);
    // print(partMgr.snapshot);
}

TEST_CASE("Test FMKWayPartMgr", "[test_FMKWayPartMgr]") {
    auto H = create_dwarf();
    run_FMKWayPartMgr(H, 3);
}

// TEST_CASE("Test FMKWayPartMgr p1", "[test_FMKWayPartMgr]") {
//     auto H = readNetD("../../testcases/p1.net");
//     run_FMKWayPartMgr(H, 3);
// }

TEST_CASE("Test FMKWayPartMgr ibm01", "[test_FMKWayPartMgr]") {
    auto H = readNetD("../../testcases/ibm01.net");
    readAre(H, "../../testcases/ibm01.are");
    run_FMKWayPartMgr(H, 3);
}
