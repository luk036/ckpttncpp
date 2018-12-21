#include <catch.hpp>
#include <ckpttncpp/FMBiGainMgr2.hpp> // import FMBiGainMgr
#include <ckpttncpp/netlist.hpp>      // import Netlist

extern SimpleNetlist create_test_netlist(); // import create_test_netlist
extern SimpleNetlist create_dwarf();        // import create_dwarf

void run_FMBiGainMgr(SimpleNetlist &H, std::vector<std::uint8_t> &part_test) {
    FMBiGainMgr mgr{H};
    auto part = part_test;
    mgr.init(part);
    while (!mgr.is_empty()) {
        // Take the gainmax with v from gainbucket
        auto [move_info_v, gainmax] = mgr.select(part);
        if (gainmax <= 0)
            continue;
        mgr.update_move(part, move_info_v);
        mgr.update_move_v(part, move_info_v, gainmax);
        auto const &[fromPart, toPart, v] = move_info_v;
        part[v] = toPart;
        CHECK(v >= 0);
    }
}

TEST_CASE("Test FMBiGainMgr", "[test_FMBiGainMgr]") {
    auto H = create_test_netlist();
    auto part_test = std::vector<std::uint8_t>{0, 1, 0};
    run_FMBiGainMgr(H, part_test);
}

TEST_CASE("Test FMBiGainMgr 2", "[test_FMBiGainMgr2]") {
    auto H = create_dwarf();
    auto part_test = std::vector<std::uint8_t>{0, 0, 0, 0, 1, 1, 1};
    run_FMBiGainMgr(H, part_test);
}
