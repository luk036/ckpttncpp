#ifndef CKPTTNCPP_FMBICONSTRMGR_HPP
#define CKPTTNCPP_FMBICONSTRMGR_HPP 1

#include "FMConstrMgr.hpp" // import FMConstrMgr

// Check if (the move of v can satisfied, makebetter, or notsatisfied

class FMBiConstrMgr : public FMConstrMgr {
  public:
    /**
     * @brief Construct a new FMBiConstrMgr object
     *
     * @param H
     * @param BalTol
     */
    FMBiConstrMgr(SimpleNetlist &H, double BalTol, std::uint8_t K = 2)
        : FMConstrMgr{H, BalTol, 2} {}

    /**
     * @brief
     *
     * @return std::uint8_t
     */
    [[nodiscard]] auto select_togo() const -> std::uint8_t {
        return this->diff[0] < this->diff[1] ? 0 : 1;
    }
};

#endif