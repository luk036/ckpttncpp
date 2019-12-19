#pragma once

#include "FMConstrMgr.hpp"
#include <gsl/span>
// Check if (the move of v can satisfied, makebetter, or notsatisfied

/*!
 * @brief FM K-Way Partition Constraint Manager
 *
 */
class FMKWayConstrMgr : public FMConstrMgr
{
  private:
    std::vector<bool> illegal;

  public:
    /*!
     * @brief Construct a new FMKWayConstrMgr object
     *
     * @param H
     * @param BalTol
     * @param K
     */
    FMKWayConstrMgr(const SimpleNetlist& H, double BalTol, uint8_t K)
        : FMConstrMgr {H, BalTol, K}
        , illegal(K, true)
    {
    }

    /*!
     * @brief
     *
     * @return uint8_t
     */
    auto select_togo() const -> uint8_t
    {
        const auto it =
            std::min_element(this->diff.cbegin(), this->diff.cend());
        return std::distance(this->diff.cbegin(), it);
    }

    /*!
     * @brief
     *
     * @param part
     */
    auto init(gsl::span<const uint8_t> part) -> void
    {
        FMConstrMgr::init(part);
        for (auto k = 0U; k != this->K; ++k)
        {
            this->illegal[k] = (this->diff[k] < this->lowerbound);
        }
    }

    /*!
     * @brief
     *
     * @param move_info_v
     * @return LegalCheck
     */
    auto check_legal(const MoveInfoV& move_info_v) -> LegalCheck;
};
