#ifndef _HOME_UBUNTU_GITHUB_CKPTTNCPP_NETLIST_HPP
#define _HOME_UBUNTU_GITHUB_CKPTTNCPP_NETLIST_HPP 1

// import networkx as nx
#include <vector>
#include <unordered_map>
#include <iterator>

template <typename graph_t, typename mode_t>
struct Netlist
{
    using nodevec_t = nodevec_t;

    graph_t &G;
    nodevec_t &cell_list;
    nodevec_t &net_list;
    nodevec_t cell_fixed;
    std::unordered_map<node_t *, size_t> cell_dict;
    bool has_fixed_cells;
    size_t max_degree;
    size_t max_net_degree;
    int cost_model = 0;
    std::vector<int> net_weight;
    std::vector<int> cell_weight;

    /**
     * @brief Construct a new Netlist object
     * 
     * @param G 
     * @param cell_list 
     * @param net_list 
     * @param cell_fixed 
     */
    Netlist(Graph &G,
            nodevec_t &cell_list,
            nodevec_t &net_list,
            nodevec_t cell_fixed = nodevec_t{})
        : G{G},
          cell_list{cell_list},
          net_list{net_list},
          cell_fixed{cell_fixed}
    {
        node_t *ptr = &this->cell_list[0];
        for (auto &v : this->cell_list)
        {
            node_t *v_ptr = &v;
            this->cell_dict[v_ptr] = v_ptr - ptr;
        }

        this->has_fixed_cells = (!this->cell_fixed.empty());

        // this->max_degree = max(this->G.degree[cell]
        //                       for cell in this->cell_list);
        // this->max_net_degree = max(this->G.degree[net]
        //                       for net in this->net_list);
    }

    /**
     * @brief 
     * 
     * @return size_t 
     */
    auto number_of_cells() const -> size_t
    {
        return std::size(this->cell_list);
    }

    /**
     * @brief 
     * 
     * @return size_t 
     */
    auto number_of_nets() const -> size_t
    {
        return std::size(this->net_list);
    }

    /**
     * @brief 
     * 
     * @return size_t 
     */
    auto number_of_pins() const -> size_t
    {
        return this->G.number_of_edges();
    }

    /**
     * @brief Get the max degree
     * 
     * @return size_t 
     */
    auto get_max_degree() const -> size_t
    {
        return this->max_degree;
    }

    /**
     * @brief Get the max net degree
     * 
     * @return size_t 
     */
    auto get_max_net_degree() const -> size_t
    {
        return this->max_net_degree;
    }
};

#endif