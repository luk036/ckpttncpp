#include <catch.hpp>
#include <ckpttncpp/FDBiGainMgr.hpp>     // import FDBiGainMgr
#include <ckpttncpp/FDKWayGainMgr.hpp>   // import FDKWayGainMgr
#include <ckpttncpp/FDPartMgr.hpp>       // import FDBiPartMgr
#include <ckpttncpp/FMBiConstrMgr.hpp>   // import FDBiConstrMgr
#include <ckpttncpp/FMKWayConstrMgr.hpp> // import FMKWayConstrMgr
#include <ckpttncpp/MLPartMgr.hpp>       // import MLBiPartMgr

extern SimpleNetlist create_test_netlist(); // import create_test_netlist
extern SimpleNetlist create_dwarf();        // import create_dwarf
extern SimpleNetlist readNetD(const char *netDFileName);
extern void readAre(SimpleNetlist &H, const char *areFileName);

TEST_CASE("Test MLFDBiPartMgr dwarf", "[test_MLFDBiPartMgr]") {
    auto H = create_dwarf();
    auto partMgr = MLPartMgr{0.3};
    auto part = std::vector<uint8_t>(H.number_of_modules(), 0);
    auto part_info = PartInfo{std::move(part), py::set<size_t>()};
    partMgr.run_Partition<FDPartMgr<FDBiGainMgr, FMBiConstrMgr>>(H, part_info);
    CHECK(partMgr.totalcost == 2);
}

TEST_CASE("Test MLFDKWayPartMgr dwarf", "[test_MLFDKWayPartMgr]") {
    auto H = create_dwarf();
    auto partMgr = MLPartMgr{0.4, 3}; // 0.3???
    auto part = std::vector<uint8_t>(H.number_of_modules(), 0);
    auto part_info = PartInfo{std::move(part), py::set<size_t>()};
    partMgr.run_Partition<FDPartMgr<FDKWayGainMgr, FMKWayConstrMgr>>(H,
                                                                     part_info);
    CHECK(partMgr.totalcost == 4);
}

TEST_CASE("Test MLFDBiPartMgr p1", "[test_MLFDBiPartMgr]") {
    auto H = readNetD("../../testcases/p1.net");
    auto partMgr = MLPartMgr{0.3};
    auto part = std::vector<uint8_t>(H.number_of_modules(), 0);
    auto part_info = PartInfo{std::move(part), py::set<size_t>()};
    partMgr.run_Partition<FDPartMgr<FDBiGainMgr, FMBiConstrMgr> >(H, part_info, 200);
    CHECK(partMgr.totalcost <= 71);
}

TEST_CASE("Test MLFDBiPartMgr ibm03", "[test_MLFDBiPartMgr]") {
    auto H = readNetD("../../testcases/ibm03.net");
    readAre(H, "../../testcases/ibm03.are");
    auto partMgr = MLPartMgr{0.45};
    auto part = std::vector<uint8_t>(H.number_of_modules(), 0);
    auto part_info = PartInfo{std::move(part), py::set<size_t>()};
    partMgr.run_Partition<FDPartMgr<FDBiGainMgr, FMBiConstrMgr> >(H, part_info, 2000);
    auto &[part_s, extern_nets] = part_info;
    CHECK(partMgr.totalcost == extern_nets.size());
    CHECK(partMgr.totalcost <= 1474);
}
