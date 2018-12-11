#include <ckpttncpp/netlist.hpp> // import Netlist
#include <py2cpp/py2cpp.hpp>
#include <catch.hpp>

extern SimpleNetlist create_test_netlist(); // import create_test_netlist
extern SimpleNetlist create_dwarf(); // import create_dwarf
extern SimpleNetlist readNetD(const char *netDFileName);
void readAre(SimpleNetlist & H, const char *areFileName);

extern std::tuple<py::set<node_t>, int>
min_net_cover_pd(SimpleNetlist &, const std::vector<size_t> &);

//
// Primal-dual algorithm for minimum vertex cover problem
//

TEST_CASE("Test min_net_cover_pd dwarf", "[test_min_cover]") {
    auto H = create_dwarf();
    auto [S, cost1] = min_net_cover_pd(H, H.module_weight);
    CHECK(cost1 == 3);
}

TEST_CASE("Test min_net_cover_pd ibm01", "[test_min_cover]") {
    auto H = readNetD("../../testcases/ibm01.net");
    readAre(H, "../../testcases/ibm01.are");
    auto [S, cost1] = min_net_cover_pd(H, H.net_weight);
    CHECK(cost1 == 4053);
}