#include <catch.hpp>
#include <ckpttncpp/FMKWayGainMgr.hpp>   // import FMKWayGainMgr
#include <ckpttncpp/FMKWayConstrMgr.hpp> // import FMKWayConstrMgr
#include <ckpttncpp/FMKWayPart.hpp>      // import FMKWayPartMgr

extern Netlist create_test_netlist(); // import create_test_netlist
extern Netlist create_dwarf(); // import create_dwarf
extern Netlist readNetD(const char *netDFileName);
void readAre(Netlist& H, const char *areFileName);

void run_FMKWayPartMgr(Netlist& H, size_t K) {
    auto gainMgr = FMKWayGainMgr(H, K);
    auto constrMgr = FMKWayConstrMgr(H, K, 0.45);
    // CHECK(H.G.nodes[0].get('weight', 1) == 5844);
    auto partMgr = FMKWayPartMgr(H, K, gainMgr, constrMgr);
    partMgr.init();
    auto totalcostbefore = partMgr.totalcost;
    partMgr.optimize();
    CHECK(partMgr.totalcost <= totalcostbefore);
    // print(partMgr.snapshot);
}

TEST_CASE("Test FMKWayPartMgr", "[test_FMKWayPartMgr]") {
    auto H = create_dwarf();
    run_FMKWayPartMgr(H, 3);
}

TEST_CASE("Test FMKWayPartMgr p1", "[test_FMKWayPartMgr]") {
    auto H = readNetD("../../testcases/p1.net");
    run_FMKWayPartMgr(H, 3);
}

TEST_CASE("Test FMKWayPartMgr ibm01", "[test_FMKWayPartMgr]") {
    auto H = readNetD("../../testcases/ibm01.net");
    readAre(H, "../../testcases/ibm01.are");
    run_FMKWayPartMgr(H, 3);
}
