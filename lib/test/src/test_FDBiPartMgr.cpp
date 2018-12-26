#include <catch.hpp>
#include <ckpttncpp/FMBiConstrMgr.hpp> // import FMBiConstrMgr
#include <ckpttncpp/FDBiGainMgr.hpp>  // import FDBiGainMgr
#include <ckpttncpp/FDPartMgr.hpp>     // import FDBiPartMgr

extern SimpleNetlist create_test_netlist(); // import create_test_netlist
extern SimpleNetlist create_dwarf();        // import create_dwarf
extern SimpleNetlist readNetD(const char *netDFileName);
void readAre(SimpleNetlist &H, const char *areFileName);

/**
 * @brief Run test cases
 *
 * @param H
 */
void run_FDBiPartMgr(SimpleNetlist &H) {
    auto gainMgr = FDBiGainMgr{H};
    auto constrMgr = FMBiConstrMgr{H, 0.3};
    // CHECK(H.G.nodes[0].get('weight', 1) == 5844);
    auto partMgr = FDPartMgr{H, gainMgr, constrMgr};
    auto part = std::vector<uint8_t>(H.number_of_modules(), 0);
    auto part_info = PartInfo{std::move(part), py::set<size_t>()};
    // partMgr.init(part_info);
    partMgr.legalize(part_info);
    auto totalcostbefore = partMgr.totalcost;
    CHECK(totalcostbefore >= 0);
    partMgr.optimize(part_info);
    CHECK(partMgr.totalcost <= totalcostbefore);
    CHECK(partMgr.totalcost >= 0);
    // print(partMgr.snapshot);
}

// TEST_CASE("Test FDBiPartMgr", "[test_FDBiPartMgr]") {
//     auto H = create_test_netlist();
//     run_FDBiPartMgr(H);
// }

TEST_CASE("Test FDBiPartMgr dwarf", "[test_FDBiPartMgr]") {
    auto H = create_dwarf();
    run_FDBiPartMgr(H);
}

TEST_CASE("Test FDBiPartMgr p1", "[test_FDBiPartMgr]") {
    auto H = readNetD("../../testcases/p1.net");
    run_FDBiPartMgr(H);
}

// TEST_CASE("Test FDBiPartMgr ibm01", "[test_FDBiPartMgr]") {
//     auto H = readNetD("../../testcases/ibm01.net");
//     readAre(H, "../../testcases/ibm01.are");
//     run_FDBiPartMgr(H);
// }