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

    xn::grAdaptor<graph_t> &G;
    nodevec_t &cell_list;
    nodevec_t &net_list;
    nodevec_t cell_fixed;
    std::unordered_map<node_t, size_t> cell_dict;
    std::unordered_map<node_t, size_t> net_dict;
    bool has_fixed_cells;
    size_t max_degree;
    size_t max_net_degree;
    int cost_model = 0;
    std::vector<size_t> node_weight;

    /**
     * @brief Construct a new Netlist object
     *
     * @param G
     * @param cell_list
     * @param net_list
     * @param cell_fixed
     */
    Netlist(xn::grAdaptor<graph_t> &G, nodevec_t &cell_list,
            nodevec_t &net_list, nodevec_t cell_fixed = nodevec_t{})
        : G{G}, cell_list{cell_list}, net_list{net_list}, cell_fixed{
                                                              cell_fixed} {
        node_t *vptr = &this->cell_list[0];
        for (auto &v : this->cell_list) {
            node_t *v_ptr = &v;
            this->cell_dict[v] = v_ptr - vptr;
        }

        node_t *nptr = &this->net_list[0];
        for (auto &n : this->net_list) {
            node_t *n_ptr = &n;
            this->net_dict[n] = n_ptr - nptr;
        }

        this->has_fixed_cells = (!this->cell_fixed.empty());

        auto deg_cmp = [this](const node_t &v, const node_t &w) -> size_t {
            return this->G.degree(v) < this->G.degree(w);
        };

        auto result1 = std::max_element(this->cell_list.begin(),
                                        this->cell_list.end(), deg_cmp);

        this->max_degree = this->G.degree(*result1);

        auto result2 = std::max_element(this->net_list.begin(),
                                        this->net_list.end(), deg_cmp);

        this->max_net_degree = this->G.degree(*result2);

        //                       for cell in this->cell_list);
        // this->max_net_degree = max(this->G.degree[net]
        //                       for net in this->net_list);
    }

    /**
     * @brief
     *
     * @return size_t
     */
    auto number_of_cells() const -> size_t {
        return std::size(this->cell_list);
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
};

struct MoveInfo
{
    node_t net;
    size_t fromPart;
    size_t toPart;
    node_t v;
};

struct MoveInfoV
{
    size_t fromPart;
    size_t toPart;
    node_t v;
};

#endif