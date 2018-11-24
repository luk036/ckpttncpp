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

/**
 * @brief Create a test netlist object
 * 
 * @return Netlist 
 */
auto create_drawf() -> Netlist
{
    using Edge = std::pair<int, int>;
    const int num_nodes = 12;
    enum nodes
    {
        a0,
        a1,
        a2,
        a3,
        p1,
        p2,
        p3,
        n1,
        n2,
        n3,
        n4,
        n5
    };
    // static std::vector<nodes> cell_name_list = {a1, a2, a3};
    // static std::vector<nodes> net__name_list = {n1, n2, n3};

    // char name[] = "ABCDE";
    static Edge edge_array[] = {
        Edge(p1, n1),
        Edge(a0, n1),
        Edge(a1, n1),
        Edge(a0, n2),
        Edge(a2, n2),
        Edge(a3, n2),
        Edge(a1, n3),
        Edge(a2, n3),
        Edge(a3, n3),
        Edge(a2, n4),
        Edge(p2, n4),
        Edge(a3, n5),
        Edge(p3, n5)
    };
    // std::size_t indices[] = {0, 1, 2, 3, 4, 5};
    int num_arcs = sizeof(edge_array) / sizeof(Edge);
    static graph_t g(edge_array, edge_array + num_arcs, num_nodes);
    using node_t = typename boost::graph_traits<graph_t>::vertex_descriptor;
    using IndexMap = typename boost::property_map<graph_t, boost::vertex_index_t>::type;
    IndexMap index = boost::get(boost::vertex_index, g);
    static auto G = xn::grAdaptor<graph_t>(g);
    static std::vector<node_t> cell_list(7);
    static std::vector<node_t> net_list(5);
    static std::vector<node_t> node_weight = 
        {1, 3, 4, 2, 0, 0, 0, 1, 1, 1, 1, 1};
  
    for (node_t v : G)
    {
        size_t i = index[v];
        if (i < 7)
        {
            cell_list[i] = v;
        }
        else
        {
            net_list[i - 7] = v;
        }
    }
    auto H = Netlist(G, cell_list, net_list);
    H.node_weight = node_weight;
    return H;
}

/**
 * @brief Create a test netlist object
 * 
 * @return Netlist 
 */
auto create_test_netlist() -> Netlist
{
    using Edge = std::pair<int, int>;
    const int num_nodes = 6;
    enum nodes
    {
        a1,
        a2,
        a3,
        n1,
        n2,
        n3
    };
    static std::vector<nodes> cell_name_list = {a1, a2, a3};
    static std::vector<nodes> net__name_list = {n1, n2, n3};

    // char name[] = "ABCDE";
    static Edge edge_array[] = {
        Edge(a1, n1),
        Edge(a1, n2),
        Edge(a2, n1),
        Edge(a2, n2),
        Edge(a3, n2),
        Edge(a1, n3)};
    // std::size_t indices[] = {0, 1, 2, 3, 4, 5};
    int num_arcs = sizeof(edge_array) / sizeof(Edge);
    static graph_t g(edge_array, edge_array + num_arcs, num_nodes);
    using node_t = typename boost::graph_traits<graph_t>::vertex_descriptor;
    using IndexMap = typename boost::property_map<graph_t, boost::vertex_index_t>::type;
    IndexMap index = boost::get(boost::vertex_index, g);
    static auto G = xn::grAdaptor<graph_t>(g);
    static std::vector<node_t> cell_list(3);
    static std::vector<node_t> net_list(3);
    static std::vector<node_t> node_weight = {3, 4, 2, 1, 1, 1};
    for (node_t v : G)
    {
        size_t i = index[v];
        if (i < 3)
        {
            cell_list[i] = v;
        }
        else
        {
            net_list[i - 3] = v;
        }
    }
    auto H = Netlist(G, cell_list, net_list);
    H.node_weight = node_weight;
    return H;
}

TEST_CASE("Test Netlist", "[test_netlist]")
{
    auto H = create_test_netlist();

    CHECK(H.number_of_cells() == 3);
    CHECK(H.number_of_nets() == 3);
    CHECK(H.number_of_pins() == 6);
    CHECK(H.get_max_degree() == 3);
    CHECK(H.get_max_net_degree() == 3);
    CHECK(!H.has_fixed_cells);
}

TEST_CASE("Test Drawf", "[test_drawf]")
{
    auto H = create_drawf();

    CHECK(H.number_of_cells() == 7);
    CHECK(H.number_of_nets() == 5);
    CHECK(H.number_of_pins() == 13);
    CHECK(H.get_max_degree() == 3);
    CHECK(H.get_max_net_degree() == 3);
    CHECK(!H.has_fixed_cells);
    CHECK(H.node_weight[1] == 3);
}
