#ifndef _HOME_UBUNTU_GITHUB_CKPTTNCPP_FMBICONSTRMGR_HPP
#define _HOME_UBUNTU_GITHUB_CKPTTNCPP_FMBICONSTRMGR_HPP 1

#include "netlist.hpp" // import Netlist

// Check if (the move of v can satisfied, makebetter, or notsatisfied

struct FMBiConstrMgr {
    Netlist &H;
    double ratio;
    size_t diff[2];
    size_t upperbound;
    // size_t lowerbound;
    size_t weight; // cache value

    /**
     * @brief Construct a new FMBiConstrMgr object
     *
     * @param H
     * @param ratio
     */
    FMBiConstrMgr(Netlist &H, double ratio)
        : H{H}, ratio{ratio}, diff{0, 0}, upperbound{0},
          // lowerbound{0},
          weight{0} {}

    /**
     * @brief
     *
     * @param part
     * @return auto
     */
    auto init(std::vector<size_t> &part) -> void {
        auto totalweight = 0;
        for (auto &v : this->H.cell_list) {
            // auto weight = this->H.G.nodes[v].get('weight', 1);
            auto weight = 10;
            this->diff[part[v]] += weight;
            totalweight += weight;
        }
        this->upperbound = std::round(totalweight * this->ratio);
        //this->lowerbound = totalweight - this->upperbound;
    }

    auto pick_move() const -> size_t {
        return this->diff[0] < this->diff[1] ? 0 : 1;
    }

    /**
     * @brief
     *
     * @param fromPart
     * @param v
     * @return auto
     */
    auto check_legal(size_t fromPart, node_t v) {
        // auto weight = this->H.G.nodes[v].get('weight', 1);
        this->weight = this->H.node_weight[v];
        auto toPart = 1 - fromPart;
        auto diffTo = this->diff[toPart] + this->weight;
        if (diffTo > this->upperbound) return 0;
        auto diffFrom = this->diff[fromPart] - this->weight;
        if (diffFrom > this->upperbound) return 1;
        return 2;
    }

    /**
     * @brief
     *
     * @param fromPart
     * @param v
     * @return true
     * @return false
     */
    auto check_constraints(size_t fromPart, node_t v) -> bool {
        // auto weight = this->H.G.nodes[v].get('weight', 1);
        this->weight = this->H.node_weight[v];
        auto toPart = 1 - fromPart;
        return this->diff[toPart] + this->weight <= this->upperbound;
    }

    /**
     * @brief
     *
     * @param fromPart
     * @param v
     */
    auto update_move(size_t fromPart, node_t v) -> void {
        auto toPart = 1 - fromPart;
        this->diff[toPart] += this->weight;
        this->diff[fromPart] -= this->weight;
    }
};

#endif