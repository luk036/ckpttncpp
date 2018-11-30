#ifndef _HOME_UBUNTU_GITHUB_CKPTTNCPP_FMBICONSTRMGR_HPP
#define _HOME_UBUNTU_GITHUB_CKPTTNCPP_FMBICONSTRMGR_HPP 1

#include "netlist.hpp" // import Netlist

// Check if (the move of v can satisfied, makebetter, or notsatisfied

class FMBiConstrMgr {
  private:
    Netlist &H;
    double ratio;
    size_t diff[2];
    // size_t upperbound;
    size_t lowerbound;
    size_t weight; // cache value

  public:
    /**
     * @brief Construct a new FMBiConstrMgr object
     *
     * @param H
     * @param ratio
     */
    FMBiConstrMgr(Netlist &H, double ratio)
        : H{H}, ratio{ratio}, diff{0, 0},
          // upperbound{0},
          lowerbound{0},
          weight{0} {}

    /**
     * @brief 
     * 
     * @param part 
     */
    auto init(const std::vector<size_t> &part) -> void {
        auto totalweight = 0;
        for (auto &v : this->H.module_list) {
            auto weight = this->H.get_module_weight(v);
            this->diff[part[v]] += weight;
            totalweight += weight;
        }
        this->lowerbound = std::round(totalweight * this->ratio);
        // this->lowerbound = totalweight - this->upperbound;
    }

    auto select() const -> size_t {
        return this->diff[0] < this->diff[1] ? 0 : 1;
    }

    /**
     * @brief 
     * 
     * @param move_info_v 
     * @return size_t 
     */
    auto check_legal(const MoveInfoV& move_info_v) -> size_t {
        auto const &[fromPart, toPart, v] = move_info_v;
        this->weight = this->H.get_module_weight(v);
        auto diffFrom = this->diff[fromPart] - this->weight;
        if (diffFrom < this->lowerbound) {
            return 0;
        }
        auto diffTo = this->diff[toPart] + this->weight;
        if (diffTo < this->lowerbound) {
            return 1;
        }
        return 2;
    }

    /**
     * @brief 
     * 
     * @param move_info_v 
     * @return true 
     * @return false 
     */
    auto check_constraints(const MoveInfoV& move_info_v) -> bool {
        auto const &[fromPart, toPart, v] = move_info_v;
        this->weight = this->H.get_module_weight(v);
        return this->diff[fromPart] - this->weight >= this->lowerbound;
    }

    /**
     * @brief
     *
     * @param fromPart
     * @param v
     */
    auto update_move(const MoveInfoV& move_info_v) -> void {
        auto const &[fromPart, toPart, v] = move_info_v;
        this->diff[toPart] += this->weight;
        this->diff[fromPart] -= this->weight;
    }
};

#endif