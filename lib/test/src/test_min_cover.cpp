#include <catch.hpp>
#include <ckpttncpp/netlist.hpp> // import Netlist
#include <py2cpp/py2cpp.hpp>

extern SimpleNetlist create_test_netlist(); // import create_test_netlist
extern SimpleNetlist create_dwarf();        // import create_dwarf
extern SimpleNetlist readNetD(const char *netDFileName);
extern void readAre(SimpleNetlist &H, const char *areFileName);
extern std::tuple<py::set<node_t>, int>
min_net_cover_pd(SimpleNetlist &, const std::vector<size_t> &);
extern std::tuple<py::set<node_t>, int>
max_independent_net(SimpleNetlist &, const std::vector<size_t> &);
extern SimpleNetlist create_contraction_subgraph(SimpleNetlist &);

//
// Primal-dual algorithm for minimum vertex cover problem
//

TEST_CASE("Test min_net_cover_pd dwarf", "[test_min_cover]") {
    auto H = create_dwarf();
    auto [S, cost] = min_net_cover_pd(H, H.module_weight);
    CHECK(cost == 3);
}

TEST_CASE("Test min_net_cover_pd ibm01", "[test_min_cover]") {
    auto H = readNetD("../../testcases/ibm01.net");
    readAre(H, "../../testcases/ibm01.are");
    auto [S, cost] = min_net_cover_pd(H, H.net_weight);
    CHECK(cost == 4053);
}

TEST_CASE("Test max_independent_net dwarf", "[test_max_independent_net]") {
    auto H = create_dwarf();
    auto [S, cost] = max_independent_net(H, H.module_weight);
    CHECK(cost == 3);
}

TEST_CASE("Test max_independent_net ibm01", "[test_max_independent_net]") {
    auto H = readNetD("../../testcases/ibm01.net");
    readAre(H, "../../testcases/ibm01.are");
    auto [S, cost] = max_independent_net(H, H.net_weight);
    CHECK(cost == 3157);
}

TEST_CASE("Test contraction subgraph dwarf", "[test_contractio_subgraph]") {
    auto H = create_dwarf();
    auto H2 = create_contraction_subgraph(H);
    CHECK(H2.number_of_modules() < 7);
    CHECK(H2.number_of_nets() == 2);
    CHECK(H2.number_of_pins() < 13);
    CHECK(H2.get_max_net_degree() <= 3);
}

TEST_CASE("Test contraction subgraph ibm01", "[test_contractio_subgraph]") {
    auto H = readNetD("../../testcases/ibm01.net");
    readAre(H, "../../testcases/ibm01.are");
    auto H2 = create_contraction_subgraph(H);
    CHECK(H2.number_of_modules() < H.number_of_modules());
    CHECK(H2.number_of_nets() < H.number_of_nets());
    CHECK(H2.number_of_pins() < H.number_of_pins());
    CHECK(H2.get_max_net_degree() <= H.get_max_net_degree());
}
