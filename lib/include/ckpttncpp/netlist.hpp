#ifndef CKPTTNCPP_NETLIST_HPP
#define CKPTTNCPP_NETLIST_HPP 1

// import networkx as nx
#include <algorithm>
#include <iterator>
// #include <py2cpp/nx2bgl.hpp>
#include <py2cpp/py2cpp.hpp>
#include <utility>
#include <vector>
#include <xnetwork/classes/graph.hpp>

// using node_t = typename boost::graph_traits<graph_t>::vertex_descriptor;
using node_t = int;
using RngIter = decltype(py::range<node_t>(0, 1));
using graph_t = xn::Graph<RngIter, RngIter>;

// using edge_t = typename boost::graph_traits<graph_t>::edge_iterator;

struct PartInfo {
    std::vector<uint8_t> part;
    py::set<node_t> extern_nets;
};

/**
 * @brief Netlist
 *
 * Netlist is implemented by xn::Graph, which is a networkx-like graph.
 *
 */
template <typename nodeview_t, typename nodemap_t> struct Netlist {
    graph_t G;
    nodeview_t modules;
    nodeview_t nets;
    nodemap_t module_map;
    nodemap_t net_map;
    size_t num_modules;
    size_t num_nets;
    size_t num_pads = 0;
    int max_degree{};
    int max_net_degree{};
    bool has_fixed_modules{};
    uint8_t cost_model = 0;
    std::vector<int> module_weight;
    std::vector<int> net_weight;
    py::set<node_t> module_fixed;

    /* For multi-level algorithms */
    Netlist<nodeview_t, nodemap_t> *parent;
    py::dict<node_t, size_t> node_up_map;
    py::dict<size_t, node_t> node_down_map;
    py::dict<size_t, node_t> cluster_down_map;

    /**
     * @brief Construct a new Netlist object
     *
     * @param G
     * @param module_list
     * @param net_list
     * @param module_fixed
     */
    Netlist(graph_t &&G,
            const nodeview_t& modules,
            const nodeview_t& nets,
            const nodemap_t& module_map,
            const nodemap_t& net_map);

    /**
     * @brief
     *
     * @return size_t
     */
    auto number_of_modules() const -> size_t { return this->num_modules; }

    /**
     * @brief
     *
     * @return size_t
     */
    auto number_of_nets() const -> size_t { return this->num_nets; }

    /**
     * @brief
     *
     * @return size_t
     */
    auto number_of_nodes() const -> size_t { return this->G.number_of_nodes(); }

    // /**
    //  * @brief
    //  *
    //  * @return size_t
    //  */
    // auto number_of_pins() const -> size_t { return this->G.number_of_edges(); }

    /**
     * @brief Get the max degree
     *
     * @return size_t
     */
    auto get_max_degree() const -> int { return this->max_degree; }

    /**
     * @brief Get the max net degree
     *
     * @return size_t
     */
    auto get_max_net_degree() const -> int { return this->max_net_degree; }

    auto get_module_weight(node_t v) const -> size_t {
        auto i_v = this->module_map[v];
        return get_module_weight_by_id(i_v);
        // return this->module_weight[this->module_map[v]];
    }

    auto get_module_weight_by_id(size_t i_v) const -> size_t {
        return this->module_weight.empty() ? 1 : this->module_weight[i_v];
    }

    auto get_net_weight(node_t  /*net*/) const -> size_t {
        // return this->net_weight.empty() ? 1
        //                                 :
        //                                 this->net_weight[this->net_map[net]];
        return 1;
    }

    auto project_down(const std::vector<std::uint8_t> &part,
                      std::vector<std::uint8_t> &part_down) -> void {
        auto &H = *this->parent;
        for (auto i_v = 0U; i_v < this->modules.size(); ++i_v) {
            auto v = this->modules[i_v];
            if (this->cluster_down_map.contains(v)) {
                auto net = this->cluster_down_map[v];
                for (auto v2 : H.G[net]) {
                    auto i_v2 = H.module_map[v2];
                    part_down[i_v2] = part[i_v];
                }
            } else {
                auto v2 = this->node_down_map[v];
                auto i_v2 = H.module_map[v2];
                part_down[i_v2] = part[i_v];
            }
        }
    }

    auto project_up(const std::vector<std::uint8_t> &part,
                    std::vector<std::uint8_t> &part_up) -> void {
        auto &H = *this->parent;
        // for (auto [i_v, v] : py::enumerate(H.modules)) {
        for (auto i_v = 0U; i_v < H.number_of_modules(); ++i_v) {
            auto v = H.modules[i_v];
            part_up[this->node_up_map[v]] = part[i_v];
        }
    }

    /**
     * @brief projection down
     *
     * @param part_info
     * @param part_info_down
     */
    void projection_down(const PartInfo &part_info, PartInfo &part_info_down);

    /**
     * @brief projection up
     *
     * @param part_info
     * @param part_info_up
     */
    void projection_up(const PartInfo &part_info, PartInfo &part_info_up);
};

/**
 * @brief Construct a new Netlist object
 *
 * @param G
 * @param module_list
 * @param net_list
 * @param module_fixed
 */
template <typename nodeview_t, typename nodemap_t>
Netlist<nodeview_t, nodemap_t>::
Netlist(graph_t &&G,
        const nodeview_t &modules,
        const nodeview_t &nets,
        const nodemap_t &module_map,
        const nodemap_t &net_map)
    : G{std::forward<graph_t>(G)}, 
      modules{modules},
      nets{nets},
      module_map{module_map},
      net_map{net_map},
      num_modules{modules.size()}, num_nets{nets.size()}
{
    this->has_fixed_modules = (!this->module_fixed.empty());
    auto deg_cmp = [this](node_t v, node_t w) -> size_t {
        return this->G.degree(v) < this->G.degree(w);
    };
    auto result1 =
        std::max_element(this->modules.begin(), this->modules.end(), deg_cmp);
    this->max_degree = this->G.degree(*result1);
    auto result2 =
        std::max_element(this->nets.begin(), this->nets.end(), deg_cmp);
    this->max_net_degree = this->G.degree(*result2);
}

using Rng2Iter = decltype(py::range<node_t>(0, 1));
using SimpleNetlist = Netlist<Rng2Iter, Rng2Iter>;
using NodeMap = py::dict<node_t, size_t>;
using ClusterNetlist = Netlist<std::vector<node_t>, NodeMap>;

struct MoveInfo {
    node_t net;
    std::uint8_t fromPart;
    std::uint8_t toPart;
    node_t v;
};

struct MoveInfoV {
    std::uint8_t fromPart;
    std::uint8_t toPart;
    // node_t v;
    size_t i_v;
};

struct Snapshot {
    py::set<node_t> extern_nets;
    py::dict<size_t, std::uint8_t> extern_modules;
};

#endif