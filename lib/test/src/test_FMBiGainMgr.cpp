#include <ckpttncpp/FMBiGainMgr.hpp> // import FMBiGainMgr
#include <ckpttncpp/netlist.hpp>     // import Netlist
#include <doctest/doctest.h>

extern auto create_test_netlist()
    -> SimpleNetlist;                        // import create_test_netlist
extern auto create_dwarf() -> SimpleNetlist; // import create_dwarf

/**
 * @brief
 *
 * @param[in] H
 * @param[in] part_test
 */
void run_FMBiGainMgr(const SimpleNetlist& H, gsl::span<std::uint8_t> part)
{
    auto mgr = FMBiGainMgr {H};
    mgr.init(part);
    while (!mgr.is_empty())
    {
        // Take the gainmax with v from gainbucket
        const auto rslt = mgr.select(part);
        auto&& move_info_v = std::get<0>(rslt);
        auto&& gainmax = std::get<1>(rslt);
        if (gainmax <= 0)
        {
            continue;
        }
        mgr.update_move(part, move_info_v);
        mgr.update_move_v(move_info_v, gainmax);
        // const auto& [v, _, toPart] = move_info_v;

        part[move_info_v.v] = move_info_v.toPart;
    }
}

TEST_CASE("Test FMBiGainMgr")
{
    const auto H = create_test_netlist();
    auto part_test = std::vector<std::uint8_t> {0, 1, 0};
    run_FMBiGainMgr(H, part_test);
}

TEST_CASE("Test FMBiGainMgr 2")
{
    const auto H = create_dwarf();
    auto part_test = std::vector<std::uint8_t> {0, 0, 0, 0, 1, 1, 1};
    run_FMBiGainMgr(H, part_test);
}
