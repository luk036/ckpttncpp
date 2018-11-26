#ifndef _HOME_UBUNTU_GITHUB_CKPTTNCPP_FMKWayCONSTRMGR_HPP
#define _HOME_UBUNTU_GITHUB_CKPTTNCPP_FMKWayCONSTRMGR_HPP 1

#include "netlist.hpp" // import Netlist
#include <algorithm>
// Check if (the move of v can satisfied, makebetter, or notsatisfied

struct FMKWayConstrMgr {
    Netlist &H;
    size_t K;
    double ratio;
    std::vector<size_t> diff;
    std::vector<bool> illegal;
    size_t upperbound;
    size_t lowerbound;
    size_t weight; // cache value

    /**
     * @brief Construct a new FMKWayConstrMgr object
     *
     * @param H
     * @param ratio
     */
    FMKWayConstrMgr(Netlist &H, size_t K, double ratio)
        : H{H}, K{K}, ratio{ratio}, diff(K, 0),
          illegal(K, true), upperbound{0}, lowerbound{0}, weight{0} {}

    /**
     * @brief
     *
     * @param part
     * @return auto
     */
    auto init(std::vector<size_t> &part) -> void {
        auto totalweight = 0;
        for (auto &v : this->H.module_list) {
            // auto weight = this->H.G.nodes[v].get('weight', 1);
            auto weight = 10;
            this->diff[part[v]] += weight;
            totalweight += weight;
        }
        auto totalweightK = totalweight * 2. / this->K;
        this->upperbound = std::round(totalweightK * this->ratio);
        this->lowerbound = std::round(totalweightK - this->upperbound);
        for (auto k = 0u; k < this->K; ++k) {
            this->illegal[k] = (this->diff[k] < this->lowerbound ||
                                this->diff[k] > this->upperbound);
        }
    }

    auto select_togo() const -> size_t {
        auto it = std::min_element(this->diff.cbegin(), this->diff.cend());
        return std::distance(this->diff.cbegin(), it);
    }

    /**
     * @brief
     *
     * @param fromPart
     * @param v
     * @return auto
     */
    auto check_legal(const MoveInfoV& move_info_v) {
        auto [fromPart, toPart, v] = move_info_v;
        this->weight = this->H.get_module_weight(v);
        auto diffTo = this->diff[toPart] + this->weight;
        auto diffFrom = this->diff[fromPart] - this->weight;
        if (diffTo > this->upperbound || diffFrom < this->lowerbound) {
            return 0; // not ok, don't move
        }
        if (diffFrom > this->upperbound || diffTo < this->lowerbound) {
            return 1; // get better, but still illegal
        }
        this->illegal[fromPart] = this->illegal[toPart] = false;
        for (auto b : this->illegal) {
            if (b)
                return 1; // get better, but still illegal
        }
        return 2; // all satisfied
    }

    /**
     * @brief
     *
     * @param fromPart
     * @param v
     * @return true
     * @return false
     */
    auto check_constraints(const MoveInfoV& move_info_v) -> bool {
        auto [fromPart, toPart, v] = move_info_v;
        this->weight = this->H.get_module_weight(v);
        auto diffTo = this->diff[toPart] + this->weight;
        auto diffFrom = this->diff[fromPart] - this->weight;
        return diffTo <= this->upperbound && diffFrom >= this->lowerbound;
    }

    /**
     * @brief
     *
     * @param fromPart
     * @param v
     */
    auto update_move(const MoveInfoV& move_info_v) -> void {
        auto [fromPart, toPart, v] = move_info_v;
        this->diff[toPart] += this->weight;
        this->diff[fromPart] -= this->weight;
    }
};

#endif