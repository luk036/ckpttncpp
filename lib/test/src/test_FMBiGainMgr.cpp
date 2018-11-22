#include <catch.hpp>
#include <ckpttncpp/netlist.hpp>     // import Netlist
#include <ckpttncpp/FMBiGainMgr.hpp> // import FMBiGainMgr

extern Netlist create_test_netlist(); // import create_test_netlist

TEST_CASE("Test FMBiGainMgr", "[test_FMBiGainMgr]")
{
    auto H = create_test_netlist();
    FMBiGainMgr mgr{H};
    auto part_test = std::vector<size_t>{0, 1, 0};
    auto part = part_test;
    mgr.init(part);
    auto gain_before = mgr.gainbucket.get_key(mgr.vertex_list[0]);
    auto max_before = mgr.gainbucket.get_max();
    // CHECK(gain_before == 1);
    CHECK(part == part_test);
    // for (auto &v : H.cell_list)
    for (auto v = 0u; v < H.cell_list.size(); ++v)
    {
        // auto &v = H.cell_list[i_v];
        auto fromPart = part[v]; 
        mgr.update_move(part, fromPart, v);
        part[v] = 1 - fromPart;
    }
    auto gain_after = mgr.gainbucket.get_key(mgr.vertex_list[0]);
    auto part_result = std::vector<size_t>{1, 0, 1};
    auto max_after = mgr.gainbucket.get_max();
    CHECK(part == part_result);
    CHECK(gain_after == gain_before);
    CHECK(max_after == max_before);

    auto waitinglist = dllink{};
    while (!mgr.gainbucket.is_empty()) {
        // Take the gainmax with v from gainbucket
        auto gainmax = mgr.gainbucket.get_max();
        auto& vlink = mgr.gainbucket.popleft();
        waitinglist.append(vlink);
        auto v = mgr.get_vertex_id(vlink);
        CHECK(v >= 0);
        CHECK(v < 3);
        // auto& v = H.cell_list[i_v];
    }

}
