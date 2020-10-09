#include <ckpttncpp/FMBiConstrMgr.hpp> // import FMBiConstrMgr
#include <ckpttncpp/FMBiGainMgr.hpp>   // import FMBiGainMgr
#include <ckpttncpp/FMPartMgr.hpp>     // import FMBiPartMgr
#include <doctest.h>
#include <string_view>

extern SimpleNetlist create_test_netlist(); // import create_test_netlist
extern SimpleNetlist create_dwarf();        // import create_dwarf
extern SimpleNetlist readNetD(std::string_view netDFileName);
extern void readAre(SimpleNetlist& H, std::string_view areFileName);

/**
 * @brief Run test cases
 *
 * @param[in] H
 */
void run_FMBiPartMgr(const SimpleNetlist& H)
{
    auto gainMgr = FMBiGainMgr {H};
    auto constrMgr = FMBiConstrMgr {H, 0.4};
    auto partMgr =
        FMPartMgr<FMBiGainMgr, FMBiConstrMgr> {H, gainMgr, constrMgr};
    auto part = std::vector<std::uint8_t>(H.number_of_modules(), 0);
    partMgr.legalize(part);
    const auto totalcostbefore = partMgr.totalcost;
    partMgr.optimize(part);
    CHECK(totalcostbefore >= 0);
    CHECK(partMgr.totalcost <= totalcostbefore);
    CHECK(partMgr.totalcost >= 0);
}

TEST_CASE("Test FMBiPartMgr")
{
    const auto H = create_test_netlist();
    run_FMBiPartMgr(H);
}

TEST_CASE("Test FMBiPartMgr dwarf")
{
    const auto H = create_dwarf();
    run_FMBiPartMgr(H);
}

TEST_CASE("Test FMBiPartMgr p1")
{
    const auto H = readNetD("../../../testcases/p1.net");
    run_FMBiPartMgr(H);
}

TEST_CASE("Test FMBiPartMgr ibm01")
{
    auto H = readNetD("../../../testcases/ibm01.net");
    readAre(H, "../../../testcases/ibm01.are");
    run_FMBiPartMgr(H);
}
