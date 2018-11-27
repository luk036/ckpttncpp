// -*- coding: utf-8 -*-
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/properties.hpp>
#include <boost/property_map/property_map.hpp>
#include <catch.hpp>
#include <py2cpp/nx2bgl.hpp>
#include <utility> // for std::pair
#include <vector>
#include <ckpttncpp/netlist.hpp>

using graph_t =
    boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS>;
using node_t = typename boost::graph_traits<graph_t>::vertex_descriptor;
using edge_t = typename boost::graph_traits<graph_t>::edge_iterator;

extern Netlist readNetD(const char *netDFileName);


TEST_CASE("Test Read Dwarf", "[test_readwrite]")
{
    auto H = readNetD("testcases/dwarf1.netD");

    CHECK(H.number_of_modules() == 7);
    CHECK(H.number_of_nets() == 5);
    CHECK(H.number_of_pins() == 13);
    CHECK(H.get_max_degree() == 3);
    CHECK(H.get_max_net_degree() == 3);
    CHECK(!H.has_fixed_modules);
    CHECK(H.get_module_weight(1) == 1);
}
