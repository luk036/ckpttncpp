#include <catch.hpp>
#include <ckpttncpp/netlist.hpp>     // import Netlist
#include <ckpttncpp/FMBiGainMgr.hpp> // import FMBiGainMgr

extern Netlist create_test_netlist(); // import create_test_netlist
extern Netlist create_drawf(); // import create_drawf

void run_FMBiGainMgr(Netlist& H, std::vector<size_t>& part_test) {
    FMBiGainMgr mgr{H};
    auto part = part_test;
    mgr.init(part);
    while (!mgr.is_empty()) {
        // Take the gainmax with v from gainbucket
        auto [v, gainmax] = mgr.select();
        if (gainmax <= 0) continue;
        auto fromPart = part[v]; 
        auto toPart = 1 - fromPart;
        auto move_info_v = MoveInfoV{fromPart, toPart, v};
        mgr.update_move(part, move_info_v, gainmax);
        part[v] = 1 - fromPart;
        CHECK(v >= 0);
    }

}

TEST_CASE("Test FMBiGainMgr", "[test_FMBiGainMgr]") {
    auto H = create_test_netlist();
    auto part_test = std::vector<size_t>{0, 1, 0};
    run_FMBiGainMgr(H, part_test);
}

TEST_CASE("Test FMBiGainMgr 2", "[test_FMBiGainMgr2]") {
    auto H = create_drawf();
    auto part_test = std::vector<size_t>{0, 0, 0, 0, 1, 1, 1};
    run_FMBiGainMgr(H, part_test);
}
