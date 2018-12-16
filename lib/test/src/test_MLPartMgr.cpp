#include <catch.hpp>
#include <ckpttncpp/FMBiConstrMgr.hpp> // import FMBiConstrMgr
#include <ckpttncpp/FMBiGainMgr2.hpp>  // import FMBiGainMgr
#include <ckpttncpp/FMPartMgr.hpp>     // import FMBiPartMgr
#include <ckpttncpp/MLPartMgr.hpp>     // import MLBiPartMgr

extern SimpleNetlist create_test_netlist(); // import create_test_netlist
extern SimpleNetlist create_dwarf();        // import create_dwarf
extern SimpleNetlist readNetD(const char *netDFileName);
extern void readAre(SimpleNetlist &H, const char *areFileName);


TEST_CASE("Test MLBiPartMgr dwarf", "[test_MLBiPartMgr]") {
    auto H = create_dwarf();
    auto partMgr = MLPartMgr{0.3};
    auto part = std::vector<uint8_t>(H.number_of_modules(), 0);
    partMgr.run_Partition<FMBiGainMgr, FMBiConstrMgr>(H, part);
    CHECK(partMgr.totalcost == 2);
}

// TEST_CASE("Test MLBiPartMgr p1", "[test_MLBiPartMgr]") {
//     auto H = readNetD("../../testcases/p1.net");
//     run_MLBiPartMgr(H);
// }

// TEST_CASE("Test MLBiPartMgr ibm01", "[test_MLBiPartMgr]") {
//     auto H = readNetD("../../testcases/ibm01.net");
//     readAre(H, "../../testcases/ibm01.are");
//     auto partMgr = MLPartMgr{0.45};
//     auto part = std::vector<uint8_t>(H.number_of_modules(), 0);
//     partMgr.run_Partition<FMBiGainMgr, FMBiConstrMgr>(H, part);
//     CHECK(partMgr.totalcost == 200);
// }
