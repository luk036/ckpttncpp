#include <ckpttncpp/FMKWayConstrMgr.hpp> // import FMKWayConstrMgr
#include <ckpttncpp/FMKWayGainMgr.hpp>   // import FMKWayGainMgr
#include <ckpttncpp/FMPartMgr.hpp>       // import FMKWayPartMgr
#include <doctest/doctest.h>
#include <string_view>

extern auto create_test_netlist()
    -> SimpleNetlist;                        // import create_test_netlist
extern auto create_dwarf() -> SimpleNetlist; // import create_dwarf
extern auto readNetD(std::string_view netDFileName) -> SimpleNetlist;
extern void readAre(SimpleNetlist& H, std::string_view areFileName);

/**
 * @brief Run test cases
 *
 * @param[in] H
 * @param[in] K
 */
void run_FMKWayPartMgr(const SimpleNetlist& H, std::uint8_t K)
{
    auto gainMgr = FMKWayGainMgr {H, K};
    auto constrMgr = FMKWayConstrMgr {H, 0.4, K};
    auto partMgr =
        FMPartMgr<FMKWayGainMgr, FMKWayConstrMgr> {H, gainMgr, constrMgr, K};
    auto part = std::vector<std::uint8_t>(H.number_of_modules(), 0);

    partMgr.legalize(part);
    auto totalcostbefore = partMgr.totalcost;
    partMgr.optimize(part);
    CHECK(totalcostbefore >= 0);
    CHECK(partMgr.totalcost <= totalcostbefore);
    CHECK(partMgr.totalcost >= 0);
    totalcostbefore = partMgr.totalcost;
    partMgr.init(part);
    CHECK(partMgr.totalcost == totalcostbefore);
}

TEST_CASE("Test FMKWayPartMgr")
{
    const auto H = create_dwarf();
    run_FMKWayPartMgr(H, 3);
}

TEST_CASE("Test FMKWayPartMgr p1")
{
    const auto H = readNetD("../../../testcases/p1.net");
    run_FMKWayPartMgr(H, 3);
}

TEST_CASE("Test FMKWayPartMgr ibm01")
{
    auto H = readNetD("../../../testcases/ibm01.net");
    readAre(H, "../../../testcases/ibm01.are");
    run_FMKWayPartMgr(H, 3);
}

// TEST_CASE("Test FMKWayPartMgr ibm18")
// {
//     auto H = readNetD("../../../testcases/ibm18.net");
//     readAre(H, "../../../testcases/ibm18.are");
//     run_FMKWayPartMgr(H, 3);
// }
