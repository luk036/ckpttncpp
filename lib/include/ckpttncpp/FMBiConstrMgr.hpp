#ifndef _HOME_UBUNTU_GITHUB_CKPTTNCPP_FMBICONSTRMGR_HPP
#define _HOME_UBUNTU_GITHUB_CKPTTNCPP_FMBICONSTRMGR_HPP 1

#include "netlist.hpp" // import Netlist

// Check if (the move of v can satisfied, makebetter, or notsatisfied

struct FMBiConstrMgr
{
    Netlist &H;
    double ratio;
    size_t diff[2];
    size_t upperbound;
    size_t lowerbound;

    /**
     * @brief Construct a new FMBiConstrMgr object
     * 
     * @param H 
     * @param ratio 
     */
    FMBiConstrMgr(Netlist &H, double ratio)
        : H{H},
          ratio{ratio},
          diff{0, 0},
          upperbound{0},
          lowerbound{0} {}

    /**
     * @brief 
     * 
     * @param part 
     * @return auto 
     */
    auto init(std::vector<size_t> &part)
    {
        auto totalweight = 0;
        for (auto &v : this->H.cell_list)
        {
            // auto weight = this->H.G.nodes[v].get('weight', 1);
            auto weight = 10;
            this->diff[part[v]] += weight;
            totalweight += weight;
        }
        this->lowerbound = std::round(totalweight * this->ratio);
        this->upperbound = totalweight - this->lowerbound;
    }

    /**
     * @brief 
     * 
     * @param fromPart 
     * @param v 
     * @return auto 
     */
    auto check_legal(size_t fromPart, node_t v, size_t weight)
    {
        // auto weight = this->H.G.nodes[v].get('weight', 1);
        // auto weight = 10;
        auto toPart = 1 - fromPart;
        auto diffToBefore = this->diff[toPart];
        auto diffToAfter = diffToBefore + weight;
        auto diffFromBefore = this->diff[fromPart];
        auto diffFromAfter = diffFromBefore - weight;
        if (diffToAfter <= this->upperbound &&
            diffFromAfter >= this->lowerbound)
        {
            return 2; // constraints satisfied
        }
        if (std::labs(diffFromAfter - diffToAfter) <
            std::labs(diffFromBefore - diffToBefore))
        {
            return 1; // get better
        }
        return 0; // not ok
    }

    /**
     * @brief 
     * 
     * @param fromPart 
     * @param v 
     * @return true 
     * @return false 
     */
    auto check_constraints(size_t fromPart, node_t v, size_t weight) -> bool
    {
        // auto weight = this->H.G.nodes[v].get('weight', 1);
        // auto weight = 10;
        auto toPart = 1 - fromPart;
        return (this->diff[toPart] + weight <= this->upperbound &&
                this->diff[fromPart] - weight >= this->lowerbound);
    }

    /**
     * @brief 
     * 
     * @param fromPart 
     * @param v 
     */
    auto update_move(size_t fromPart, node_t v, size_t weight) -> void
    {
        // auto weight = this->H.G.nodes[v].get('weight', 1);
        // auto weight = 10;
        auto toPart = 1 - fromPart;
        this->diff[toPart] += weight;
        this->diff[fromPart] -= weight;
    }
};

#endif