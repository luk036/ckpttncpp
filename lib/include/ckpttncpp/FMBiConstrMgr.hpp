#ifndef _HOME_UBUNTU_GITHUB_CKPTTNCPP_FMBICONSTRMGR_HPP
#define _HOME_UBUNTU_GITHUB_CKPTTNCPP_FMBICONSTRMGR_HPP 1

#include "FMConstrMgr.hpp" // import FMConstrMgr
#include "netlist.hpp"     // import Netlist

// Check if (the move of v can satisfied, makebetter, or notsatisfied

class FMBiConstrMgr : public FMConstrMgr {
  public:
    /**
     * @brief Construct a new FMBiConstrMgr object
     *
     * @param H
     * @param ratio
     */
    FMBiConstrMgr(Netlist &H, double ratio) : FMConstrMgr(H, ratio, 2) {}

    /**
     * @brief
     *
     * @return std::uint8_t
     */
    auto select_togo() const -> std::uint8_t {
        return this->diff[0] < this->diff[1] ? 0 : 1;
    }
};

#endif