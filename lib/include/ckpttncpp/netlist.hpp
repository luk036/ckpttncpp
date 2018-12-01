#ifndef _HOME_UBUNTU_GITHUB_CKPTTNCPP_NETLIST_HPP
#define _HOME_UBUNTU_GITHUB_CKPTTNCPP_NETLIST_HPP 1

// import networkx as nx
#include <iterator>
#include <py2cpp/nx2bgl.hpp>
#include <unordered_map>
#include <vector>

using graph_t =
    boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS>;
using node_t = typename boost::graph_traits<graph_t>::vertex_descriptor;
// using edge_t = typename boost::graph_traits<graph_t>::edge_iterator;

/**
 * @brief Netlist
 *
 */
struct Netlist {
    using nodevec_t = std::vector<node_t>;

    xn::grAdaptor<graph_t> G;
    nodevec_t module_list;
    nodevec_t net_list;
    nodevec_t module_fixed;
    bool has_fixed_modules;
    size_t num_pads = 0;
    size_t max_degree;
    size_t max_net_degree;
    int cost_model = 0;
    // std::vector<size_t> node_weight;
    std::vector<size_t> module_weight;
    std::vector<size_t> net_weight;

    /**
     * @brief Construct a new Netlist object
     *
     * @param G
     * @param module_list
     * @param net_list
     * @param module_fixed
     */
    Netlist(xn::grAdaptor<graph_t> &&G, nodevec_t &&module_list,
            nodevec_t &&net_list, nodevec_t module_fixed = nodevec_t{})
        : G{std::move(G)}, module_list{std::move(module_list)}, net_list{std::move(net_list)}, 
          module_fixed{module_fixed}
    {
        this->has_fixed_modules = (!this->module_fixed.empty());

        auto deg_cmp = [this](const node_t &v, const node_t &w) -> size_t {
            return this->G.degree(v) < this->G.degree(w);
        };

        auto result1 = std::max_element(this->module_list.begin(),
                                        this->module_list.end(), deg_cmp);

        this->max_degree = this->G.degree(*result1);

        auto result2 = std::max_element(this->net_list.begin(),
                                        this->net_list.end(), deg_cmp);

        this->max_net_degree = this->G.degree(*result2);

        //                       for module in this->module_list);
        // this->max_net_degree = max(this->G.degree[net]
        //                       for net in this->net_list);
    }

    /**
     * @brief
     *
     * @return size_t
     */
    auto number_of_modules() const -> size_t {
        return std::size(this->module_list);
    }

    /**
     * @brief
     *
     * @return size_t
     */
    auto number_of_nets() const -> size_t { return std::size(this->net_list); }

    /**
     * @brief
     *
     * @return size_t
     */
    auto number_of_pins() const -> size_t { return this->G.number_of_edges(); }

    /**
     * @brief Get the max degree
     *
     * @return size_t
     */
    auto get_max_degree() const -> size_t { return this->max_degree; }

    /**
     * @brief Get the max net degree
     *
     * @return size_t
     */
    auto get_max_net_degree() const -> size_t { return this->max_net_degree; }

    auto get_module_weight(node_t v) const -> size_t {
        return this->module_weight.empty() ? 1 : this->module_weight[v];
    }

    auto get_net_weight(node_t net) const -> size_t {
        return this->net_weight.empty() ? 1 : 
            this->net_weight[net - number_of_modules()];
    }
};

struct MoveInfo
{
    node_t net;
    std::uint8_t fromPart;
    std::uint8_t toPart;
    node_t v;
};

struct MoveInfoV
{
    std::uint8_t fromPart;
    std::uint8_t toPart;
    node_t v;
};

#endif