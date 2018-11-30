#include <catch.hpp>
#include <ckpttncpp/FMBiGainMgr.hpp>   // import FMBiGainMgr
#include <ckpttncpp/FMBiConstrMgr.hpp> // import FMBiConstrMgr
#include <ckpttncpp/FMPartMgr.hpp>      // import FMBiPartMgr

extern Netlist create_test_netlist(); // import create_test_netlist
extern Netlist create_dwarf(); // import create_dwarf
extern Netlist readNetD(const char *netDFileName);
void readAre(Netlist& H, const char *areFileName);

void run_FMBiPartMgr(Netlist& H) {
    auto gainMgr = FMBiGainMgr{H};
    auto constrMgr = FMBiConstrMgr{H, 0.3};
    // CHECK(H.G.nodes[0].get('weight', 1) == 5844);
    auto partMgr = FMPartMgr{H, gainMgr, constrMgr};
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

TEST_CASE("Test FMBiPartMgr dwarf", "[test_FMBiPartMgr]") {
    auto H = create_dwarf();
    run_FMBiPartMgr(H);
}

TEST_CASE("Test FMBiPartMgr p1", "[test_FMBiPartMgr]") {
    auto H = readNetD("../../testcases/p1.net");
    run_FMBiPartMgr(H);
}

TEST_CASE("Test FMBiPartMgr ibm01", "[test_FMBiPartMgr]") {
    auto H = readNetD("../../testcases/ibm01.net");
    readAre(H, "../../testcases/ibm01.are");
    run_FMBiPartMgr(H);
}

