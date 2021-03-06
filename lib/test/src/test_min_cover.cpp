#include <ckpttncpp/HierNetlist.hpp> // import Netlist
#include <ckpttncpp/netlist.hpp>     // import Netlist
#include <ckpttncpp/netlist_algo.hpp>
#include <doctest/doctest.h>
#include <memory> //std::unique_ptr
#include <py2cpp/py2cpp.hpp>
#include <string_view>

extern auto create_test_netlist()
    -> SimpleNetlist;                        // import create_test_netlist
extern auto create_dwarf() -> SimpleNetlist; // import create_dwarf
extern auto readNetD(std::string_view netDFileName) -> SimpleNetlist;
extern void readAre(SimpleNetlist& H, std::string_view areFileName);
// extern std::tuple<py::set<node_t>, int>
// min_net_cover_pd(SimpleNetlist &, const std::vector<int> &);

using node_t = SimpleNetlist::node_t;
extern auto create_contraction_subgraph(const SimpleNetlist&,
    const py::set<node_t>&) -> std::unique_ptr<SimpleHierNetlist>;

//
// Primal-dual algorithm for minimum vertex cover problem
//

// TEST_CASE("Test min_net_cover_pd dwarf", "[test_min_cover]") {
//     auto H = create_dwarf();
//     auto [S, cost] = min_net_cover_pd(H, H.module_weight);
//     CHECK(cost == 3);
// }

// TEST_CASE("Test min_net_cover_pd ibm01", "[test_min_cover]") {
//     auto H = readNetD("../../../testcases/ibm01.net");
//     readAre(H, "../../../testcases/ibm01.are");
//     auto [S, cost] = min_net_cover_pd(H, H.net_weight);
//     CHECK(cost == 4053);
// }

TEST_CASE("Test contraction subgraph dwarf")
{
    const auto H = create_dwarf();
    const auto H2 = create_contraction_subgraph(H, py::set<node_t> {});
    // auto H3 = create_contraction_subgraph(*H2, py::set<node_t> {});
    CHECK(H2->number_of_modules() < 7);
    CHECK(H2->number_of_nets() == 3);
    // CHECK(H2->number_of_pins() < 14);
    CHECK(H2->get_max_net_degree() <= 3);

    auto part = std::vector<std::uint8_t>(H.number_of_modules(), 0);
    auto part2 = std::vector<std::uint8_t>(H2->number_of_modules(), 0);
    auto part3 = std::vector<std::uint8_t>(H2->number_of_modules(), 0);
    part2[0] = part2[2] = 1;
    part2[1] = 2;
    H2->projection_down(part2, part);
    H2->projection_up(part, part3);
    CHECK(part2 == part3);
}

TEST_CASE("Test contraction subgraph ibm01")
{
    auto H = readNetD("../../../testcases/ibm01.net");
    readAre(H, "../../../testcases/ibm01.are");
    auto H2 = create_contraction_subgraph(H, py::set<node_t> {});
    auto H3 = create_contraction_subgraph(*H2, py::set<node_t> {});
    CHECK(H2->number_of_modules() < H.number_of_modules());
    CHECK(H2->number_of_nets() < H.number_of_nets());
    // CHECK(H2->number_of_pins() < H.number_of_pins());
    CHECK(H2->get_max_net_degree() <= H.get_max_net_degree());

    auto part2 = std::vector<std::uint8_t>(H2->number_of_modules(), 0);
    auto part3 = std::vector<std::uint8_t>(H3->number_of_modules(), 0);
    auto part4 = std::vector<std::uint8_t>(H3->number_of_modules(), 0);
    auto i = std::uint8_t(0);

    for (auto& item : part3)
    {
        item = ++i % 6;
    }
    H3->projection_down(part3, part2);
    H3->projection_up(part2, part4);
    CHECK(part3 == part4);
}

TEST_CASE("Test contraction subgraph ibm18")
{
    auto H = readNetD("../../../testcases/ibm18.net");
    readAre(H, "../../../testcases/ibm18.are");
    auto H2 = create_contraction_subgraph(H, py::set<node_t> {});
    auto H3 = create_contraction_subgraph(*H2, py::set<node_t> {});
    CHECK(H2->number_of_modules() < H.number_of_modules());
    CHECK(H2->number_of_nets() < H.number_of_nets());
    // CHECK(H2->number_of_pins() < H.number_of_pins());
    CHECK(H2->get_max_net_degree() <= H.get_max_net_degree());

    auto part2 = std::vector<std::uint8_t>(H2->number_of_modules(), 0);
    auto part3 = std::vector<std::uint8_t>(H3->number_of_modules(), 0);
    auto part4 = std::vector<std::uint8_t>(H3->number_of_modules(), 0);
    for (auto i = 0u; i != H3->number_of_modules(); ++i)
    {
        part3[i] = std::uint8_t(i);
    }
    H3->projection_down(part3, part2);
    H3->projection_up(part2, part4);
    CHECK(part3 == part4);
}
