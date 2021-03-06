#include <ckpttncpp/netlist.hpp>      // import Netlist
#include <ckpttncpp/netlist_algo.hpp> // import Netlist
#include <doctest/doctest.h>
// #include <memory> //std::unique_ptr
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

TEST_CASE("Test min_vertex_cover dwarf")
{
    const auto H = create_dwarf();
    auto weight = py::dict<node_t, int> {};
    auto covset = py::set<node_t> {};
    for (auto node : H)
    {
        weight[node] = 1;
        // covset[node] = false;
    }
    min_vertex_cover(H, weight, covset);
}

//
// Primal-dual algorithm for minimum vertex cover problem
//


TEST_CASE("Test min_maximal_matching dwarf")
{
    const auto H = create_dwarf();
    // const auto N = H.number_of_nets();
    auto weight = py::dict<node_t, int> {};
    auto matchset = py::set<node_t> {};
    auto dep = py::set<node_t> {};
    for (auto net : H.nets)
    {
        // matchset[net] = false;
        weight[net] = 1;
    }
    // for (auto v : H)
    // {
    //     dep[v] = false;
    // }
    min_maximal_matching(H, weight, matchset, dep);
}
