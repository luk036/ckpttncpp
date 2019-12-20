#pragma once

#include "netlist.hpp"
#include <algorithm>
#include <cinttypes>
#include <cmath>
#include <gsl/span>
#include <vector>

/*!
 * @brief Check if the move of v can satisfied, getbetter, or notsatisfied
 *
 */
enum class LegalCheck
{
    notsatisfied,
    getbetter,
    allsatisfied
};

/*!
 * @brief FM Partition Constraint Manager
 *
 */
class FMConstrMgr
{
    int weight; // cache value

  protected:
    const SimpleNetlist& H;
    double BalTol;
    uint8_t K;
    std::vector<int> diff;
    int lowerbound {};
    int totalweight {};

    /*!
     * @brief Construct a new FMConstrMgr object
     *
     * @param H
     * @param K
     * @param BalTol
     */
    FMConstrMgr(const SimpleNetlist& H, double BalTol, uint8_t K = 2)
        : H {H}
        , BalTol {BalTol}
        , K {K}
        , diff(K, 0)
    {
        for (auto&& v : this->H.modules)
        {
            weight = this->H.get_module_weight(v);
            this->totalweight += weight;
        }
        const auto totalweightK = this->totalweight * (2. / this->K);
        this->lowerbound = std::round(totalweightK * this->BalTol);
    }

  public:
    /*!
     * @brief
     *
     * @param part
     */
    auto init(gsl::span<const uint8_t> part) -> void;

    /*!
     * @brief
     *
     * @param move_info_v
     * @return LegalCheck
     */
    auto check_legal(const MoveInfoV& move_info_v) -> LegalCheck;

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
