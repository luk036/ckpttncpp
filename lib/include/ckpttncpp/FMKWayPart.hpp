#ifndef _HOME_UBUNTU_GITHUB_CKPTTNCPP_FMKWayPART_HPP
#define _HOME_UBUNTU_GITHUB_CKPTTNCPP_FMKWayPART_HPP 1

// **Special code for two-pin nets**
// Take a snapshot when a move make **negative** gain.
// Snapshot in the form of "interface"???
#include <cassert>
#include <vector>
#include "netlist.hpp"

class FMKWayGainMgr;
class FMKWayConstrMgr;

class FMKWayPartMgr {
  private:
    Netlist &H;
    size_t K; //> number_of_partitions
    FMKWayGainMgr &gainMgr;
    FMKWayConstrMgr &validator;
    std::vector<size_t> snapshot;
    std::vector<size_t> part;

  public:
    int totalcost;

    /**
     * @brief Construct a new FMKWayPartMgr object
     *
     * @param H
     * @param gainMgr
     * @param constrMgr
     */
    FMKWayPartMgr(Netlist &H, size_t K, FMKWayGainMgr &gainMgr,
                  FMKWayConstrMgr &constrMgr)
        : H{H}, K{K}, gainMgr{gainMgr}, validator{constrMgr}, snapshot{},
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