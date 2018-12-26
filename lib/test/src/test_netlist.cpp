// -*- coding: utf-8 -*-
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/properties.hpp>
#include <boost/property_map/property_map.hpp>
#include <catch.hpp>
#include <ckpttncpp/netlist.hpp>
#include <py2cpp/nx2bgl.hpp>
#include <py2cpp/py2cpp.hpp>
#include <utility> // for std::pair
#include <vector>

using graph_t =
    boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS>;
using node_t = typename boost::graph_traits<graph_t>::vertex_descriptor;
using edge_t = typename boost::graph_traits<graph_t>::edge_iterator;

/**
 * @brief Create a test netlist object
 *
 * @return Netlist
 */
auto create_dwarf() -> SimpleNetlist {
    using Edge = std::pair<int, int>;
    const int num_nodes = 13;
    enum nodes { a0, a1, a2, a3, p1, p2, p3, n1, n2, n3, n4, n5, n6 };
    // static std::vector<nodes> module_name_list = {a1, a2, a3};
    // static std::vector<nodes> net__name_list = {n1, n2, n3};

    // char name[] = "ABCDE";
    Edge edge_array[] = {Edge(p1, n1), Edge(a0, n1), Edge(a1, n1), Edge(a0, n2),
                         Edge(a2, n2), Edge(a3, n2), Edge(a1, n3), Edge(a2, n3),
                         Edge(a3, n3), Edge(a2, n4), Edge(p2, n4), Edge(a3, n5),
                         Edge(p3, n5), Edge(a0, n6)};
    // std::size_t indices[] = {0, 1, 2, 3, 4, 5};
    int num_arcs = sizeof(edge_array) / sizeof(Edge);
    graph_t g(edge_array, edge_array + num_arcs, num_nodes);
    using node_t = typename boost::graph_traits<graph_t>::vertex_descriptor;
    using IndexMap =
        typename boost::property_map<graph_t, boost::vertex_index_t>::type;
    IndexMap index = boost::get(boost::vertex_index, g);
    auto G = xn::grAdaptor<graph_t>(std::move(g));

    // std::vector<node_t> module_list(7);
    // std::vector<node_t> net_list(5);
    std::vector<node_t> module_weight = {1, 3, 4, 2, 0, 0, 0};
    auto H = Netlist(std::move(G), py::range2(0, 7), py::range2(7, 13),
                     py::range2(-7, 6));
    H.module_weight = module_weight;
    H.num_pads = 3;
    return H;
}

/**
 * @brief Create a test netlist object
 *
 * @return Netlist
 */
auto create_test_netlist() -> SimpleNetlist {
    using Edge = std::pair<int, int>;
    auto const num_nodes = 6;
    enum nodes { a1, a2, a3, n1, n2, n3 };

    // char name[] = "ABCDE";
    Edge edge_array[] = {Edge(a1, n1), Edge(a1, n2), Edge(a2, n1),
                         Edge(a2, n2), Edge(a3, n2), Edge(a1, n3)};
    // std::size_t indices[] = {0, 1, 2, 3, 4, 5};
    auto num_arcs = sizeof(edge_array) / sizeof(Edge);
    auto g = graph_t{edge_array, edge_array + num_arcs, num_nodes};
    // using node_t = typename boost::graph_traits<graph_t>::vertex_descriptor;
    // using IndexMap =
    //     typename boost::property_map<graph_t, boost::vertex_index_t>::type;
    auto index = boost::get(boost::vertex_index, g);
    auto G = xn::grAdaptor<graph_t>{std::move(g)};
    // std::vector<node_t> module_list(3);
    // std::vector<node_t> net_list(3);
    auto module_weight = std::vector<node_t>{3, 4, 2};
    auto H = Netlist{std::move(G), py::range2(0, 3), py::range2(3, 6),
                     py::range2(-3, 3)};
    H.module_weight = std::move(module_weight);
    return H;
}

TEST_CASE("Test Netlist", "[test_netlist]") {
    auto H = create_test_netlist();

    CHECK(H.number_of_modules() == 3);
    CHECK(H.number_of_nets() == 3);
    CHECK(H.number_of_pins() == 6);
    CHECK(H.get_max_degree() == 3);
    CHECK(H.get_max_net_degree() == 3);
    CHECK(!H.has_fixed_modules);
}

TEST_CASE("Test dwarf", "[test_dwarf]") {
    auto H = create_dwarf();

    CHECK(H.number_of_modules() == 7);
    CHECK(H.number_of_nets() == 6);
    CHECK(H.number_of_pins() == 14);
    CHECK(H.get_max_degree() == 3);
    CHECK(H.get_max_net_degree() == 3);
    CHECK(!H.has_fixed_modules);
    CHECK(H.get_module_weight(1) == 3);
}
