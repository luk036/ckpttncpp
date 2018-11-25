#include <catch.hpp>
#include <ckpttncpp/FMKWayGainMgr.hpp>   // import FMKWayGainMgr
#include <ckpttncpp/FMKWayConstrMgr.hpp> // import FMKWayConstrMgr
#include <ckpttncpp/FMKWayPart.hpp>      // import FMKWayPartMgr

extern Netlist create_test_netlist(); // import create_test_netlist
extern Netlist create_drawf(); // import create_drawf

void run_FMKWayPartMgr(Netlist& H, size_t K) {
    auto gainMgr = FMKWayGainMgr(H, K);
    auto constrMgr = FMKWayConstrMgr(H, K, 0.7);
    // CHECK(H.G.nodes[0].get('weight', 1) == 5844);
    auto partMgr = FMKWayPartMgr(H, K, gainMgr, constrMgr);
    partMgr.init();
    auto totalcostbefore = partMgr.totalcost;
    partMgr.optimize();
    CHECK(partMgr.totalcost <= totalcostbefore);
    // print(partMgr.snapshot);
}

TEST_CASE("Test FMKWayPartMgr 2", "[test_FMKWayPartMgr_2]") {
    auto H = create_drawf();
    run_FMKWayPartMgr(H, 3);
}
