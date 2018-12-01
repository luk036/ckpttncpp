#ifndef _HOME_UBUNTU_GITHUB_CKPTTNCPP_FMPARTMGR_HPP
#define _HOME_UBUNTU_GITHUB_CKPTTNCPP_FMPARTMGR_HPP 1

// **Special code for two-pin nets**
// Take a snapshot when a move make **negative** gain.
// Snapshot in the form of "interface"???
#include <cassert>
#include <vector>
#include "netlist.hpp"

// class FMGainMgr;
// class FMConstrMgr;

template <typename FMGainMgr, typename FMConstrMgr>
class FMPartMgr {
  private:
    Netlist &H;
    FMGainMgr &gainMgr;
    FMConstrMgr &validator;
    std::vector<std::uint8_t> snapshot;
    std::vector<std::uint8_t> part;

  public:
    int totalcost;

    /**
     * @brief Construct a new FMPartMgr object
     *
     * @param H
     * @param gainMgr
     * @param constrMgr
     */
    FMPartMgr(Netlist &H, FMGainMgr &gainMgr,
                  FMConstrMgr &constrMgr)
        : H{H}, gainMgr{gainMgr}, validator{constrMgr}, snapshot{},
          part(this->H.number_of_modules(), 0), totalcost{0} {}

    /**
     * @brief
     *
     */
    void init();

    /**
     * @brief
     *
     */
    void optimize();
};

#endif