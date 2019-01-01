#include <catch.hpp>
#include <ckpttncpp/FMBiConstrMgr.hpp> // import FMBiConstrMgr
#include <ckpttncpp/FMBiGainMgr.hpp>   // import FMBiGainMgr
#include <ckpttncpp/FMPartMgr.hpp>     // import FMBiPartMgr

extern SimpleNetlist create_test_netlist(); // import create_test_netlist
extern SimpleNetlist create_dwarf();        // import create_dwarf
extern SimpleNetlist readNetD(const char *netDFileName);
void readAre(SimpleNetlist &H, const char *areFileName);

/**
 * @brief Run test cases
 *
 * @param H
 */
void run_FMBiPartMgr(SimpleNetlist &H) {
    auto gainMgr = FMBiGainMgr{H};
    auto constrMgr = FMBiConstrMgr{H, 0.3};
    auto partMgr = FMPartMgr{H, gainMgr, constrMgr};
    auto part = std::vector<uint8_t>(H.number_of_modules(), 0);
    auto part_info = PartInfo{std::move(part), py::set<node_t>{}};
    partMgr.legalize(part_info);
    auto totalcostbefore = partMgr.totalcost;
    CHECK(totalcostbefore >= 0);
    partMgr.optimize(part_info);
    CHECK(partMgr.totalcost <= totalcostbefore);
    CHECK(partMgr.totalcost >= 0);
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

// TEST_CASE("Test FMBiPartMgr ibm01", "[test_FMBiPartMgr]") {
//     auto H = readNetD("../../testcases/ibm01.net");
//     readAre(H, "../../testcases/ibm01.are");
//     run_FMBiPartMgr(H);
// }
