#include <catch.hpp>
#include <ckpttncpp/FDKWayGainMgr.hpp>   // import FDKWayGainMgr
#include <ckpttncpp/FDPartMgr.hpp>       // import FDKWayPartMgr
#include <ckpttncpp/FMKWayConstrMgr.hpp> // import FMKWayConstrMgr

extern SimpleNetlist create_test_netlist(); // import create_test_netlist
extern SimpleNetlist create_dwarf();        // import create_dwarf
extern SimpleNetlist readNetD(const char *netDFileName);
void readAre(SimpleNetlist &H, const char *areFileName);

/*!
 * @brief Run test cases
 *
 * @param H
 * @param K
 */
void run_FDKWayPartMgr(SimpleNetlist &H, uint8_t K) {
    auto gainMgr = FDKWayGainMgr{H, K};
    auto constrMgr = FMKWayConstrMgr{H, 0.4, K};
    auto partMgr = FDPartMgr{H, gainMgr, constrMgr};
    auto part = std::vector<uint8_t>(H.number_of_modules(), 0);
    // auto part_info = PartInfo{std::move(part), py::set<node_t>()};
    // partMgr.init(part);
    partMgr.legalize(part);
    auto totalcostbefore = partMgr.totalcost;
    CHECK(totalcostbefore >= 0);
    partMgr.optimize(part);
    CHECK(partMgr.totalcost <= totalcostbefore);
    CHECK(partMgr.totalcost >= 0);
    // print(partMgr.snapshot);
}

TEST_CASE("Test FDKWayPartMgr", "[test_FDKWayPartMgr]") {
    auto H = create_dwarf();
    run_FDKWayPartMgr(H, 3);
}

TEST_CASE("Test FDKWayPartMgr p1", "[test_FDKWayPartMgr]") {
    auto H = readNetD("../../testcases/p1.net");
    run_FDKWayPartMgr(H, 3);
}

// TEST_CASE("Test FDKWayPartMgr ibm01", "[test_FDKWayPartMgr]") {
//     auto H = readNetD("../../testcases/ibm01.net");
//     readAre(H, "../../testcases/ibm01.are");
//     run_FDKWayPartMgr(H, 3);
// }

// TEST_CASE("Test FDKWayPartMgr ibm02", "[test_FDKWayPartMgr]") {
//     auto H = readNetD("../../testcases/ibm02.net");
//     readAre(H, "../../testcases/ibm02.are");
//     run_FDKWayPartMgr(H, 3);
// }

// TEST_CASE("Test FDKWayPartMgr ibm03", "[test_FDKWayPartMgr]") {
//     auto H = readNetD("../../testcases/ibm03.net");
//     readAre(H, "../../testcases/ibm03.are");
//     run_FDKWayPartMgr(H, 3);
// }
