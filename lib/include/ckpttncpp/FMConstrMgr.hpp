#pragma once

#include <algorithm>
#include <cinttypes>
#include <cmath>
#include <vector>
#include "netlist.hpp"
// Check if (the move of v can satisfied, makebetter, or notsatisfied

struct MoveInfoV;

/*!
 * @brief FM Partition Constraint Manager
 *
 */
class FMConstrMgr
{
    uint32_t weight; // cache value

protected:
    SimpleNetlist&   H;
    double           BalTol;
    uint8_t          K;
    std::vector<int> diff;
    int              lowerbound{0};
    int              totalweight{0};

    /*!
     * @brief Construct a new FMConstrMgr object
     *
     * @param H
     * @param K
     * @param BalTol
     */
    FMConstrMgr(SimpleNetlist& H, double BalTol, uint8_t K = 2)
        : H{H}, BalTol{BalTol}, K{K}, diff(K, 0)
    {
        for (auto i_v = 0U; i_v < this->H.number_of_modules(); ++i_v)
        {
            weight = this->H.get_module_weight_by_id(i_v);
            this->totalweight += weight;
        }
        auto totalweightK = this->totalweight * (2. / this->K);
        this->lowerbound  = std::round(totalweightK * this->BalTol);
    }

public:
    /*!
     * @brief
     *
     * @param part
     */
    auto init(const std::vector<uint8_t>& part) -> void;

    /*!
     * @brief
     *
     * @param move_info_v
     * @return size_t
     */
    auto check_legal(const MoveInfoV& move_info_v) -> size_t;

    /*!
     * @brief
     *
     * @param move_info_v
     * @return true
     * @return false
     */
    auto check_constraints(const MoveInfoV& move_info_v) -> bool;

    /*!
     * @brief
     *
     * @param move_info_v
     */
    auto update_move(const MoveInfoV& move_info_v) -> void;
};
