#ifndef CKPTTNCPP_FMBICONSTRMGR_HPP
#define CKPTTNCPP_FMBICONSTRMGR_HPP 1

#include "FMConstrMgr.hpp" // import FMConstrMgr

/**
 * @brief Constraint Manager
 *
 * Check if (the move of v can satisfied, makebetter, or notsatisfied 
 */
struct FMBiConstrMgr : FMConstrMgr {
    /**
     * @brief Construct a new FMBiConstrMgr object
     *
     * @param H
     * @param BalTol
     */
    FMBiConstrMgr(SimpleNetlist &H, double BalTol, uint8_t K = 2)
        : FMConstrMgr{H, BalTol, 2} {}

    /**
     * @brief
     *
     * @return uint8_t
     */
    auto select_togo() const -> uint8_t {
        return this->diff[0] < this->diff[1] ? 0 : 1;
    }
};

#endif