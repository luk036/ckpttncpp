#include <catch2/catch.hpp>
#include <ckpttncpp/FMBiGainMgr.hpp> // import FMBiGainMgr
#include <ckpttncpp/netlist.hpp>     // import Netlist

extern SimpleNetlist create_test_netlist(); // import create_test_netlist
extern SimpleNetlist create_dwarf();        // import create_dwarf

/**
 * @brief
 *
 * @param H
 * @param part_test
 */
void run_FMBiGainMgr(const SimpleNetlist& H, gsl::span<uint8_t> part)
{
    auto mgr = FMBiGainMgr {H};
    mgr.init(part);
    while (not mgr.is_empty())
    {
        // Take the gainmax with v from gainbucket
        const auto [move_info_v, gainmax] = mgr.select(part);
        if (gainmax <= 0)
        {
            continue;
        }
        mgr.update_move(part, move_info_v);
        mgr.update_move_v(move_info_v, gainmax);
        const auto& [v, _, toPart] = move_info_v;

        part[v] = toPart;
    }
}

TEST_CASE("Test FMBiGainMgr", "[test_FMBiGainMgr]")
{
    const auto H = create_test_netlist();
    auto part_test = std::vector<uint8_t> {0, 1, 0};
    run_FMBiGainMgr(H, part_test);
}

TEST_CASE("Test FMBiGainMgr 2", "[test_FMBiGainMgr2]")
{
    const auto H = create_dwarf();
    auto part_test = std::vector<uint8_t> {0, 0, 0, 0, 1, 1, 1};
    run_FMBiGainMgr(H, part_test);
}
