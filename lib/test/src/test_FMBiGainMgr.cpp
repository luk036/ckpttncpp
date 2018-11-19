#include <catch.hpp>
#include <ckpttncpp/netlist.hpp>     // import Netlist
#include <ckpttncpp/FMBiGainMgr.hpp> // import FMBiGainMgr

extern Netlist create_test_netlist(); // import create_test_netlist

TEST_CASE("Test FMBiGainMgr", "[test_FMBiGainMgr]")
{
    auto H = create_test_netlist();
    auto mgr = FMBiGainMgr(H);
    auto part_test = std::vector<size_t>{0, 1, 0};
    auto part = part_test;
    mgr.init(part);
    auto gain_before = mgr.gainbucket.get_key(mgr.vertex_list[0]);
    CHECK(gain_before == 1);
    CHECK(part == part_test);
    for (auto &v : H.cell_list)
    {
        mgr.update_move(part, v);
    }
    auto gain_after = mgr.gainbucket.get_key(mgr.vertex_list[0]);
    auto part_result = std::vector<size_t>{1, 0, 1};
    CHECK(part == part_result);
    CHECK(gain_after == gain_before);
}
