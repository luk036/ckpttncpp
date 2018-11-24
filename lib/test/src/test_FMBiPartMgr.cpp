#include <catch.hpp>
#include <ckpttncpp/FMBiGainMgr.hpp>   // import FMBiGainMgr
#include <ckpttncpp/FMBiConstrMgr.hpp> // import FMBiConstrMgr
#include <ckpttncpp/FMBiPart.hpp>      // import FMBiPartMgr

extern Netlist create_test_netlist(); // import create_test_netlist
extern Netlist create_drawf(); // import create_drawf

void run_FMBiPartMgr(Netlist& H) {
    auto gainMgr = FMBiGainMgr(H);
    auto constrMgr = FMBiConstrMgr(H, 0.7);
    // CHECK(H.G.nodes[0].get('weight', 1) == 5844);
    auto partMgr = FMBiPartMgr(H, gainMgr, constrMgr);
    partMgr.init();
    auto totalcostbefore = partMgr.totalcost;
    partMgr.optimize();
    CHECK(partMgr.totalcost <= totalcostbefore);
    // print(partMgr.snapshot);
}

TEST_CASE("Test FMBiPartMgr", "[test_FMBiPartMgr]") {
    auto H = create_test_netlist();
    run_FMBiPartMgr(H);
}

TEST_CASE("Test FMBiPartMgr 2", "[test_FMBiPartMgr_2]") {
    auto H = create_drawf();
    run_FMBiPartMgr(H);
}
