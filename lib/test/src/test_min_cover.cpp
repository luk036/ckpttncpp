#include <catch.hpp>
#include <ckpttncpp/netlist.hpp> // import Netlist
#include <py2cpp/py2cpp.hpp>

extern SimpleNetlist create_test_netlist(); // import create_test_netlist
extern SimpleNetlist create_dwarf();        // import create_dwarf
extern SimpleNetlist readNetD(const char *netDFileName);
extern void readAre(SimpleNetlist &H, const char *areFileName);
// extern std::tuple<py::set<node_t>, int>
// min_net_cover_pd(SimpleNetlist &, const std::vector<int> &);
extern std::tuple<py::set<node_t>, int>
max_independent_net(SimpleNetlist &, const std::vector<int> &,
                    const py::set<node_t> &);
extern std::unique_ptr<SimpleNetlist>
  create_contraction_subgraph(SimpleNetlist &,
                            const py::set<node_t> &);

//
// Primal-dual algorithm for minimum vertex cover problem
//

// TEST_CASE("Test min_net_cover_pd dwarf", "[test_min_cover]") {
//     auto H = create_dwarf();
//     auto [S, cost] = min_net_cover_pd(H, H.module_weight);
//     CHECK(cost == 3);
// }

// TEST_CASE("Test min_net_cover_pd ibm01", "[test_min_cover]") {
//     auto H = readNetD("../../testcases/ibm01.net");
//     readAre(H, "../../testcases/ibm01.are");
//     auto [S, cost] = min_net_cover_pd(H, H.net_weight);
//     CHECK(cost == 4053);
// }

TEST_CASE("Test max_independent_net dwarf", "[test_max_independent_net]") {
    auto H = create_dwarf();
    auto [S, cost] = max_independent_net(H, H.module_weight, py::set<node_t>{});
    CHECK(cost == 3);
}

// TEST_CASE("Test max_independent_net ibm01", "[test_max_independent_net]") {
//     auto H = readNetD("../../testcases/ibm01.net");
//     readAre(H, "../../testcases/ibm01.are");
//     auto [S, cost] = max_independent_net(H, H.net_weight);
//     CHECK(cost == 3157);
// }

TEST_CASE("Test contraction subgraph dwarf", "[test_contractio_subgraph]") {
    auto H = create_dwarf();
    auto H2 = create_contraction_subgraph(H, py::set<node_t>{});
    auto H3 = create_contraction_subgraph(*H2, py::set<node_t>{});
    CHECK(H2->number_of_modules() < 7);
    CHECK(H2->number_of_nets() == 3);
    // CHECK(H2->number_of_pins() < 14);
    CHECK(H2->get_max_net_degree() <= 3);

    auto part = std::vector<uint8_t>(H.number_of_modules(), 0);
    auto part2 = std::vector<uint8_t>(H2->number_of_modules(), 0);
    auto part3 = std::vector<uint8_t>(H2->number_of_modules(), 0);
    part2[0] = part2[2] = 1;
    part2[1] = 2;
    H2->project_down(part2, part);
    H2->project_up(part, part3);
    CHECK(part2 == part3);
}

// TEST_CASE("Test contraction subgraph ibm01", "[test_contractio_subgraph]") {
//     auto H = readNetD("../../testcases/ibm01.net");
//     readAre(H, "../../testcases/ibm01.are");
//     auto H2 = create_contraction_subgraph(H, py::set<node_t>{});
//     auto H3 = create_contraction_subgraph(*H2, py::set<node_t>{});
//     CHECK(H2->number_of_modules() < H.number_of_modules());
//     CHECK(H2->number_of_nets() < H.number_of_nets());
//     CHECK(H2->number_of_pins() < H.number_of_pins());
//     CHECK(H2->get_max_net_degree() <= H.get_max_net_degree());

//     auto part2 = std::vector<uint8_t>(H2->number_of_modules(), 0);
//     auto part3 = std::vector<uint8_t>(H3.number_of_modules(), 0);
//     auto part4 = std::vector<uint8_t>(H3.number_of_modules(), 0);
//     for (auto i = 0u; i < H3.number_of_modules(); ++i) {
//         part3[i] = i;
//     }
//     H3.project_down(part3, part2);
//     H3.project_up(part2, part4);
//     CHECK(part3 == part4);
// }
