#ifndef _HOME_UBUNTU_GITHUB_CKPTTNCPP_FMBICONSTRMGR_HPP
#define _HOME_UBUNTU_GITHUB_CKPTTNCPP_FMBICONSTRMGR_HPP 1

#include "netlist.hpp" // import Netlist
#include "FMConstrMgr.hpp" // import FMConstrMgr

// Check if (the move of v can satisfied, makebetter, or notsatisfied

class FMBiConstrMgr : public FMConstrMgr {
  public:
    /**
     * @brief Construct a new FMBiConstrMgr object
     *
     * @param H
     * @param ratio
     */
    FMBiConstrMgr(Netlist &H, double ratio)
        : FMConstrMgr(H, ratio, 2) {}

    /**
     * @brief 
     * 
     * @return size_t 
     */
    auto select_togo() const -> size_t {
        return this->diff[0] < this->diff[1] ? 0 : 1;
    }
};

#endif