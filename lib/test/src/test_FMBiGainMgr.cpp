#include <catch.hpp>
#include <ckpttncpp/FMBiGainMgr.hpp> // import FMBiGainMgr
#include <ckpttncpp/netlist.hpp>     // import Netlist

extern SimpleNetlist create_test_netlist(); // import create_test_netlist
extern SimpleNetlist create_dwarf();        // import create_dwarf

/*!
 * @brief
 *
 * @param H
 * @param part_test
 */
void run_FMBiGainMgr(SimpleNetlist &H, std::vector<uint8_t> &part) {
    auto mgr = FMBiGainMgr{H};
    // auto &[part, _] = part_info;
    mgr.init(part);
    while (!mgr.is_empty()) {
        // Take the gainmax with v from gainbucket
        auto [move_info_v, gainmax] = mgr.select(part);
        if (gainmax <= 0) {
            continue;
        }
        mgr.update_move(part, move_info_v);
        mgr.update_move_v(move_info_v, gainmax);
        auto &&[fromPart, toPart, i_v] = move_info_v;
        part[i_v] = toPart;
        CHECK(i_v >= 0);
    }
}

TEST_CASE("Test FMBiGainMgr", "[test_FMBiGainMgr]") {
    auto H = create_test_netlist();
    auto part_test = std::vector<uint8_t>{0, 1, 0};
    // auto part_info = PartInfo{std::move(part_test), py::set<node_t>{}};
    run_FMBiGainMgr(H, part_test);
}

TEST_CASE("Test FMBiGainMgr 2", "[test_FMBiGainMgr2]") {
    auto H = create_dwarf();
    auto part_test = std::vector<uint8_t>{0, 0, 0, 0, 1, 1, 1};
    // auto part_info = PartInfo{std::move(part_test), py::set<node_t>{}};
    run_FMBiGainMgr(H, part_test);
}
