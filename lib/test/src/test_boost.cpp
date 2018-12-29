// -*- coding: utf-8 -*-
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/properties.hpp>
#include <boost/property_map/property_map.hpp>
#include <catch.hpp>
#include <utility> // for std::pair
#include <vector>

using graph_t =
    boost::adjacency_list<boost::hash_setS, boost::vecS, boost::undirectedS>;
using node_t = typename boost::graph_traits<graph_t>::vertex_descriptor;
using edge_t = typename boost::graph_traits<graph_t>::edge_iterator;

/**
 * @brief Create a test netlist object
 *
 * @return Netlist
 */
auto test_netlist() -> void {
    using Edge = std::pair<int, int>;
    auto const num_nodes = 6;
    enum nodes { a1, a2, a3, n1, n2, n3 };

    auto g = graph_t{num_nodes};
    // using node_t = typename boost::graph_traits<graph_t>::vertex_descriptor;
    // using IndexMap =
    //     typename boost::property_map<graph_t, boost::vertex_index_t>::type;
    auto index = boost::get(boost::vertex_index, g);
    boost::add_edge(a1, n1, g);
    boost::add_edge(a1, n1, g);
    CHECK(boost::num_edges(g) == 1);
}

TEST_CASE("Test BOOST", "[test_boost]") { test_netlist(); }
