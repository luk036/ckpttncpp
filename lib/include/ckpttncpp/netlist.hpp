#pragma once

// import networkx as nx
#include <algorithm>
#include <gsl/span>
#include <iterator>
#include <py2cpp/py2cpp.hpp>
#include <utility>
#include <vector>
#include <xnetwork/classes/graph.hpp>

using node_t = int;

// struct PartInfo
// {
//     std::vector<std::uint8_t> part;
//     py::set<node_t> extern_nets;
// };

/*!
 * @brief Netlist
 *
 * Netlist is implemented by xn::Graph, which is a networkx-like graph.
 *
 */
template <typename nodeview_t, typename nodemap_t>
struct Netlist
{
    using node_t = typename nodeview_t::value_type;
    using index_t = typename nodemap_t::value_type;
    using graph_t = xn::Graph<nodeview_t, nodemap_t>;

    graph_t G;
    nodeview_t modules;
    nodeview_t nets;
    nodemap_t module_map;
    // nodemap_t net_map;
    size_t num_modules;
    size_t num_nets;
    size_t num_pads = 0U;
    size_t max_degree {};
    size_t max_net_degree {};
    bool has_fixed_modules {};
    std::uint8_t cost_model = 0;
    std::vector<int> module_weight;
    std::vector<int> net_weight;
    py::set<node_t> module_fixed;

    /* For multi-level algorithms */
    const Netlist<nodeview_t, nodemap_t>* parent;
    py::dict<node_t, index_t> node_up_map;
    py::dict<index_t, node_t> node_down_map;
    py::dict<index_t, node_t> cluster_down_map;

    /*!
     * @brief Construct a new Netlist object
     *
     * @param[in] G
     * @param[in] module_list
     * @param[in] net_list
     * @param[in] module_fixed
     */
    Netlist(graph_t G, const nodeview_t& modules, const nodeview_t& nets,
        const nodemap_t& module_map);

    /**
     * @brief Construct a new Netlist object
     *
     * @param[in] G
     * @param[in] num_modules
     * @param[in] num_nets
     */
    Netlist(graph_t G, int num_modules, int num_nets);

    /**
     * @brief Get the number of modules
     *
     * @return size_t
     */
    auto number_of_modules() const -> size_t
    {
        return this->num_modules;
    }

    /*!
     * @brief Get the number of nets
     *
     * @return size_t
     */
    auto number_of_nets() const -> size_t
    {
        return this->num_nets;
    }

    /*!
     * @brief Get the number of nodes
     *
     * @return size_t
     */
    auto number_of_nodes() const -> size_t
    {
        return this->G.number_of_nodes();
    }

    // /*!
    //  * @brief
    //  *
    //  * @return index_t
    //  */
    // auto number_of_pins() const -> index_t { return
    // this->G.number_of_edges(); }

    /*!
     * @brief Get the max degree
     *
     * @return size_t
     */
    auto get_max_degree() const -> size_t
    {
        return this->max_degree;
    }

    /*!
     * @brief Get the max net degree
     *
     * @return index_t
     */
    auto get_max_net_degree() const -> size_t
    {
        return this->max_net_degree;
    }

    /**
     * @brief Get the module weight
     *
     * @param[in] v
     * @return int
     */
    auto get_module_weight(node_t v) const -> int
    {
        return this->module_weight.empty() ? 1 : this->module_weight[v];
    }

    /**
     * @brief Get the net weight
     *
     * @return int
     */
    auto get_net_weight(node_t /*net*/) const -> int
    {
        // return this->net_weight.empty() ? 1
        //                                 :
        //                                 this->net_weight[this->net_map[net]];
        return 1;
    }

    // auto project_down(const std::vector<std::uint8_t> &part,
    //                   std::vector<std::uint8_t> &part_down) -> void {
    //     auto &H = *this->parent;
    //     for (auto i_v = 0U; i_v < this->modules.size(); ++ {
    //         auto v = this->modules[v];
    //         if (this->cluster_down_map.contains(v)) {
    //             auto net = this->cluster_down_map[v];
    //             for (auto v2 : H.G[net]) {
    //                 auto i_v2 = H.module_map[v2];
    //                 part_down[v2] = part[v];
    //             }
    //         } else {
    //             auto v2 = this->node_down_map[v];
    //             auto i_v2 = H.module_map[v2];
    //             part_down[v2] = part[v];
    //         }
    //     }
    // }

    // auto project_up(const std::vector<std::uint8_t> &part,
    //                 std::vector<std::uint8_t> &part_up) -> void {
    //     auto &H = *this->parent;
    //     // for (auto [v, v] : py::enumerate(H.modules)) {
    //     for (auto i_v = 0U; i_v < H.number_of_modules(); ++ {
    //         auto v = H.modules[v];
    //         part_up[this->node_up_map[v]] = part[v];
    //     }
    // }

    /*!
     * @brief projection down
     *
     * @param[in] part
     * @param[out] part_down
     */
    void projection_down(gsl::span<const std::uint8_t> part,
        gsl::span<std::uint8_t> part_down) const;

    /*!
     * @brief projection up
     *
     * @param[in] part
     * @param[out] part_up
     */
    void projection_up(gsl::span<const std::uint8_t> part,
        gsl::span<std::uint8_t> part_up) const;
};

/**
 * @brief Construct a new Netlist object
 *
 * @tparam nodeview_t
 * @tparam nodemap_t
 * @param[in] G
 * @param[in] modules
 * @param[in] nets
 * @param[in] module_map
 */
template <typename nodeview_t, typename nodemap_t>
Netlist<nodeview_t, nodemap_t>::Netlist(graph_t G, const nodeview_t& modules,
    const nodeview_t& nets, const nodemap_t& module_map)
    : G {std::move(G)}
    , modules {modules}
    , nets {nets}
    , module_map {module_map}
    , num_modules(modules.size())
    , num_nets(nets.size())
{
    this->has_fixed_modules = (!this->module_fixed.empty());

    // Some compilers does not accept py::range()->iterator as a forward
    // iterator auto deg_cmp = [this](node_t v, node_t w) -> index_t {
    //     return this->G.degree(v) < this->G.degree(w);
    // };
    // const auto result1 =
    //     std::max_element(this->modules.begin(), this->modules.end(),
    //     deg_cmp);
    // this->max_degree = this->G.degree(*result1);
    // const auto result2 =
    //     std::max_element(this->nets.begin(), this->nets.end(), deg_cmp);
    // this->max_net_degree = this->G.degree(*result2);

    this->max_degree = 0U;
    for (const auto& v : this->modules)
    {
        if (this->max_degree < this->G.degree(v))
        {
            this->max_degree = this->G.degree(v);
        }
    }

    this->max_net_degree = 0U;
    for (const auto& net : this->nets)
    {
        if (this->max_net_degree < this->G.degree(net))
        {
            this->max_net_degree = this->G.degree(net);
        }
    }
}

template <typename nodeview_t, typename nodemap_t>
Netlist<nodeview_t, nodemap_t>::Netlist(
    graph_t G, int numModules, int numNets)
    : Netlist {std::move(G), py::range<int>(numModules),
          py::range<int>(numModules, numModules + numNets),
          py::range<int>(numModules)}
{
}

using RngIter = decltype(py::range<int>(0, 1));
using graph_t = xn::Graph<RngIter, RngIter>;
using index_t = int;
using SimpleNetlist = Netlist<RngIter, RngIter>;

struct MoveInfo
{
    node_t net;
    node_t v;
    std::uint8_t fromPart;
    std::uint8_t toPart;
};

struct MoveInfoV
{
    node_t v;
    std::uint8_t fromPart;
    std::uint8_t toPart;
    // node_t v;
};

struct Snapshot
{
    py::set<node_t> extern_nets;
    py::dict<index_t, std::uint8_t> extern_modules;
};
