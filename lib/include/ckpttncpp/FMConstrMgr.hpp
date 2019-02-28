#ifndef CKPTTNCPP_FMCONSTRMGR_HPP
#define CKPTTNCPP_FMCONSTRMGR_HPP 1

#include "netlist.hpp"
#include <algorithm>
#include <cinttypes>
#include <vector>
// Check if (the move of v can satisfied, makebetter, or notsatisfied

class MoveInfoV;

/**
 * @brief FM Partition Constraint Manager
 *
 */
class FMConstrMgr {
  protected:
    SimpleNetlist &H;
    double BalTol;
    std::uint8_t K;
    std::vector<size_t> diff;
    size_t lowerbound;
    size_t totalweight;
    uint32_t weight; // cache value

    /**
     * @brief Construct a new FMConstrMgr object
     *
     * @param H
     * @param K
     * @param BalTol
     */
    FMConstrMgr(SimpleNetlist &H, double BalTol, std::uint8_t K = 2)
        : H{H}, BalTol{BalTol}, K{K},
          diff(K, 0), lowerbound{0}, weight{0}, totalweight{0} {
        for (auto v = 0U; v < this->H.number_of_modules(); ++v) {
            weight = this->H.get_module_weight_by_id(v);
            this->totalweight += weight;
        }
        auto totalweightK = this->totalweight * (2. / this->K);
        this->lowerbound = std::round(totalweightK * this->BalTol);
    }

  public:
    /**
     * @brief
     *
     * @param part
     */
    auto init(const std::vector<std::uint8_t> &part) -> void;

    /**
     * @brief
     *
     * @param move_info_v
     * @return size_t
     */
    auto check_legal(const MoveInfoV &move_info_v) -> size_t;

    /**
     * @brief
     *
     * @param move_info_v
     * @return true
     * @return false
     */
    auto check_constraints(const MoveInfoV &move_info_v) -> bool;

    /**
     * @brief
     *
     * @param move_info_v
     */
    auto update_move(const MoveInfoV &move_info_v) -> void;
};

#endif