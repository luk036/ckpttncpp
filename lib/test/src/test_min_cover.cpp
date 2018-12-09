#include <ckpttncpp/netlist.hpp> // import Netlist
#include <py2cpp/py2cpp.hpp>
#include <catch.hpp>

extern Netlist create_test_netlist(); // import create_test_netlist
extern Netlist create_dwarf(); // import create_dwarf
extern Netlist readNetD(const char *netDFileName);
void readAre(Netlist& H, const char *areFileName);

extern std::tuple<py::set<size_t>, size_t> min_net_cover_pd(Netlist &, const std::vector<size_t> &);

//
// Primal-dual algorithm for minimum vertex cover problem
//

TEST_CASE("Test min_net_cover_pd dwarf", "[test_min_cover]") {
    auto H = create_dwarf();
    auto [S, cost1] = min_net_cover_pd(H, H.module_weight);
    CHECK(cost1 == 4);
}

TEST_CASE("Test min_net_cover_pd ibm01", "[test_min_cover]") {
    auto H = readNetD("../../testcases/ibm01.net");
    readAre(H, "../../testcases/ibm01.are");
    auto [S, cost1] = min_net_cover_pd(H, H.net_weight);
    CHECK(cost1 == 4873);
}