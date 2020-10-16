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
     * @param[in] H
     * @param[in] BalTol
     * @param[in] K
     */
    FMKWayConstrMgr(const SimpleNetlist& H, double BalTol, std::uint8_t K)
        : FMConstrMgr {H, BalTol, K}
        , illegal(K, true)
    {
    }

    /*!
     * @brief
     *
     * @return std::uint8_t
     */
    [[nodiscard]] auto select_togo() const -> std::uint8_t
    {
        const auto it =
            std::min_element(this->diff.cbegin(), this->diff.cend());
        return std::uint8_t(std::distance(this->diff.cbegin(), it));
    }

    /*!
     * @brief
     *
     * @param[in] part
     */
    auto init(gsl::span<const std::uint8_t> part) -> void
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
     * @param[in] move_info_v
     * @return LegalCheck
     */
    auto check_legal(const MoveInfoV<node_t>& move_info_v) -> LegalCheck;
};
