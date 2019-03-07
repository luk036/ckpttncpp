// -*- coding: utf-8 -*-
#include <catch.hpp>
#include <py2cpp/py2cpp.hpp>
#include <xnetwork/classes/graph.hpp>

/**
 * @brief Create a test netlist object
 *
 * @return Netlist
 */
TEST_CASE("Test xnetwork", "[test_xnetwork]") {
    auto const num_nodes = 6;
    enum nodes { a1, a2, a3, n1, n2, n3 };
    auto R = py::range<uint8_t>(0, 6);
    auto G = xn::Graph{R, R};
    G.add_edge(a1, n1);
    G.add_edge(a1, n1);
    G.add_edge(a1, n2);
    G.add_edge(a2, n2);

    auto count = 0;
    for (auto v : G) {
        ++count;
    }

    CHECK(G.number_of_nodes() == count);

    auto deg = 0;
    for (auto v : G[a1]) {
        ++deg;
    }

    CHECK(G.degree(a1) == deg);
}
