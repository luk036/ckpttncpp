#include <doctest.h>
#include <ckpttncpp/FMKWayConstrMgr.hpp> // import FMKWayConstrMgr
#include <ckpttncpp/FMKWayGainMgr.hpp>   // import FMKWayGainMgr
#include <ckpttncpp/FMPartMgr.hpp>       // import FMKWayPartMgr
#include <string_view>

extern SimpleNetlist create_test_netlist(); // import create_test_netlist
extern SimpleNetlist create_dwarf();        // import create_dwarf
extern SimpleNetlist readNetD(std::string_view netDFileName);
extern void readAre(SimpleNetlist& H, std::string_view areFileName);

/**
 * @brief Run test cases
 *
 * @param H
 * @param K
 */
void run_FMKWayPartMgr(const SimpleNetlist& H, std::uint8_t K)
{
    auto gainMgr = FMKWayGainMgr {H, K};
    auto constrMgr = FMKWayConstrMgr {H, 0.4, K};
    auto partMgr = FMPartMgr {H, gainMgr, constrMgr};
    auto part = std::vector<std::uint8_t>(H.number_of_modules(), 0);

    partMgr.legalize(part);
    const auto totalcostbefore = partMgr.totalcost;
    partMgr.optimize(part);
    CHECK(totalcostbefore >= 0);
    CHECK(partMgr.totalcost <= totalcostbefore);
    CHECK(partMgr.totalcost >= 0);
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

// TEST_CASE("Test FMKWayPartMgr ibm01", "[test_FMKWayPartMgr]") {
//     const auto H = readNetD("../../../testcases/ibm01.net");
//     readAre(H, "../../../testcases/ibm01.are");
//     run_FMKWayPartMgr(H, 3);
// }
