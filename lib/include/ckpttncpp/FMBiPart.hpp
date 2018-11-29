#ifndef _HOME_UBUNTU_GITHUB_CKPTTNCPP_FMBIPART_HPP
#define _HOME_UBUNTU_GITHUB_CKPTTNCPP_FMBIPART_HPP 1

// **Special code for two-pin nets**
// Take a snapshot when a move make **negative** gain.
// Snapshot in the form of "interface"???
#include <cassert>
#include <vector>
#include "netlist.hpp"

class FMBiConstrMgr;
class FMBiGainMgr;

class FMBiPartMgr {
  private:
    Netlist &H;
    FMBiGainMgr &gainMgr;
    FMBiConstrMgr &validator;
    std::vector<size_t> snapshot;
    std::vector<size_t> part;

  public:
    int totalcost;

    /**
     * @brief Construct a new FMBiPartMgr object
     *
     * @param H
     * @param gainMgr
     * @param constrMgr
     */
    FMBiPartMgr(Netlist &H, FMBiGainMgr &gainMgr, FMBiConstrMgr &constrMgr)
        : H{H}, gainMgr{gainMgr}, validator{constrMgr}, snapshot{},
          part(this->H.number_of_modules(), 0), totalcost{0} {}

    /**
     * @brief
     *
     */
    auto init() -> void;

    /**
     * @brief
     *
     */
    auto optimize() -> void;
};

#endif