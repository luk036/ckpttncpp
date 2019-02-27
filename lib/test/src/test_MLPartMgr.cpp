#include <catch.hpp>
#include <ckpttncpp/FMBiConstrMgr.hpp>   // import FMBiConstrMgr
#include <ckpttncpp/FMBiGainMgr.hpp>     // import FMBiGainMgr
#include <ckpttncpp/FMKWayConstrMgr.hpp> // import FMKWayConstrMgr
#include <ckpttncpp/FMKWayGainMgr.hpp>   // import FMKWayGainMgr
#include <ckpttncpp/FMPartMgr.hpp>       // import FMBiPartMgr
#include <ckpttncpp/MLPartMgr.hpp>       // import MLBiPartMgr

extern SimpleNetlist create_test_netlist(); // import create_test_netlist
extern SimpleNetlist create_dwarf();        // import create_dwarf
extern SimpleNetlist readNetD(const char *netDFileName);
extern void readAre(SimpleNetlist &H, const char *areFileName);

TEST_CASE("Test MLBiPartMgr dwarf", "[test_MLBiPartMgr]") {
    auto H = create_dwarf();
    auto partMgr = MLPartMgr{0.3};
    auto part = std::vector<uint8_t>(H.number_of_modules(), 0);
    auto part_info = PartInfo{std::move(part), py::set<size_t>()};
    partMgr.run_FMPartition<FMPartMgr<FMBiGainMgr, FMBiConstrMgr>>(H,
                                                                   part_info);
    CHECK(partMgr.totalcost == 2);
}

TEST_CASE("Test MLKWayPartMgr dwarf", "[test_MLKWayPartMgr]") {
    auto H = create_dwarf();
    auto partMgr = MLPartMgr{0.4, 3}; // 0.3???
    auto part = std::vector<uint8_t>(H.number_of_modules(), 0);
    auto part_info = PartInfo{std::move(part), py::set<size_t>()};
    partMgr.run_FMPartition<FMPartMgr<FMKWayGainMgr, FMKWayConstrMgr>>(
        H, part_info);
    CHECK(partMgr.totalcost == 4);
}

TEST_CASE("Test MLBiPartMgr p1", "[test_MLBiPartMgr]") {
    auto H = readNetD("../../testcases/p1.net");
    auto partMgr = MLPartMgr{0.3};
    auto part = std::vector<uint8_t>(H.number_of_modules(), 0);
    auto part_info = PartInfo{std::move(part), py::set<size_t>()};
    partMgr.run_FMPartition<FMPartMgr<FMBiGainMgr, FMBiConstrMgr>>(H, part_info, 100);
    CHECK(partMgr.totalcost == 38);
}

// TEST_CASE("Test MLBiPartMgr ibm01", "[test_MLBiPartMgr]") {
//     auto H = readNetD("../../testcases/ibm01.net");
//     readAre(H, "../../testcases/ibm01.are");
//     auto partMgr = MLPartMgr{0.45};
//     auto part = std::vector<uint8_t>(H.number_of_modules(), 0);
//     auto part_info = PartInfo{std::move(part), py::set<size_t>()};
//     partMgr.run_FMPartition<FMPartMgr<FMBiGainMgr, FMBiConstrMgr> >(H, part_info, 300);
//     CHECK(partMgr.totalcost <= 310);
// }

TEST_CASE("Test MLBiPartMgr ibm03", "[test_MLBiPartMgr]") {
    auto H = readNetD("../../testcases/ibm03.net");
    readAre(H, "../../testcases/ibm03.are");
    auto partMgr = MLPartMgr{0.45};
    auto part = std::vector<uint8_t>(H.number_of_modules(), 0);
    auto part_info = PartInfo{std::move(part), py::set<size_t>()};
    partMgr.run_FMPartition<FMPartMgr<FMBiGainMgr, FMBiConstrMgr> >(H, part_info, 300);
    CHECK(partMgr.totalcost <= 1469);
}
