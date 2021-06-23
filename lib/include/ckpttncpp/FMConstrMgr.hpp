#pragma once

#include "netlist.hpp"
#include <algorithm>
#include <cinttypes>
#include <cmath>
#include <gsl/span>
#include <range/v3/core.hpp>
#include <range/v3/numeric/accumulate.hpp>
#include <range/v3/view/transform.hpp>
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
  private:
    const SimpleNetlist& H;
    double BalTol;
    int totalweight;
    int weight {}; // cache value

  protected:
    std::vector<int> diff;
    int lowerbound {};
    std::uint8_t K;

    /*!
     * @brief Construct a new FMConstrMgr object
     *
     * @param[in] H
     * @param[in] BalTol
     */
    FMConstrMgr(const SimpleNetlist& H, double BalTol)
        : FMConstrMgr(H, BalTol, 2)
    {
    }

    /*!
     * @brief Construct a new FMConstrMgr object
     *
     * @param[in] H
     * @param[in] BalTol
     * @param[in] K
     */
    FMConstrMgr(const SimpleNetlist& H, double BalTol, std::uint8_t K)
        : H {H}
        , BalTol {BalTol}
        , diff(K, 0)
        , K {K}
    {
        this->totalweight = 0;
        for (const auto& v : this->H)
        {
            this->totalweight += this->H.get_module_weight(v);
        }
        // auto r_weight = this->H | ranges::views::transform([&](auto v){return
        // this->H.get_module_weight(v);}); this->totalweight =
        // ranges::accumulate(r_weight, 0);
        const auto totalweightK = this->totalweight * (2. / this->K);
        this->lowerbound = int(std::round(totalweightK * this->BalTol));
    }

  public:
    using node_t = typename SimpleNetlist::node_t;

    /*!
     * @brief
     *
     * @param[in] part
     */
    auto init(gsl::span<const std::uint8_t> part) -> void;

    /*!
     * @brief
     *
     * @param[in] move_info_v
     * @return LegalCheck
     */
    auto check_legal(const MoveInfoV<node_t>& move_info_v) -> LegalCheck;

    /*!
     * @brief
     *
     * @param[in] move_info_v
     * @return true
     * @return false
     */
    auto check_constraints(const MoveInfoV<node_t>& move_info_v) -> bool;

    /*!
     * @brief
     *
     * @param[in] move_info_v
     */
    auto update_move(const MoveInfoV<node_t>& move_info_v) -> void;
};
