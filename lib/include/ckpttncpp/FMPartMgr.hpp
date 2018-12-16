#ifndef _HOME_UBUNTU_GITHUB_CKPTTNCPP_FMPARTMGR_HPP
#define _HOME_UBUNTU_GITHUB_CKPTTNCPP_FMPARTMGR_HPP 1

// **Special code for two-pin nets**
// Take a snapshot when a move make **negative** gain.
// Snapshot in the form of "interface"???
#include "netlist.hpp"
#include <cassert>
#include <cinttypes>
#include <iterator>
#include <py2cpp/py2cpp.hpp>
#include <type_traits>
#include <vector>

/**
 * @brief FM Partition Manager
 *
 * @tparam FMGainMgr
 * @tparam FMConstrMgr
 */
template <typename FMGainMgr, typename FMConstrMgr> //
class FMPartMgr {
  private:
    SimpleNetlist &H;
    FMGainMgr &gainMgr;
    FMConstrMgr &validator;
    std::vector<std::uint8_t> snapshot;
    // std::vector<std::uint8_t> part;

  public:
    int totalcost;

    /**
     * @brief Construct a new FMPartMgr object
     *
     * @param H
     * @param gainMgr
     * @param constrMgr
     */
    FMPartMgr(SimpleNetlist &H, FMGainMgr &gainMgr, FMConstrMgr &constrMgr)
        : H{H}, gainMgr{gainMgr}, validator{constrMgr}, snapshot{},
          // part(this->H.number_of_modules(), 0),
          totalcost{0} {}

    /**
     * @brief
     *
     * @param part
     */
    void init(std::vector<std::uint8_t> &part);

    /**
     * @brief
     *
     * @param part
     */
    size_t legalize(std::vector<std::uint8_t> &part);

    /**
     * @brief
     *
     * @param part
     */
    void optimize(std::vector<std::uint8_t> &part);
};

#endif