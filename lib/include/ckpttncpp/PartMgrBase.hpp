#ifndef _HOME_UBUNTU_GITHUB_CKPTTNCPP_PARTMGRBASE_HPP
#define _HOME_UBUNTU_GITHUB_CKPTTNCPP_PARTMGRBASE_HPP 1

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
 * @brief Partition Manager Base
 * 
 * @tparam GainMgr 
 * @tparam ConstrMgr 
 * @tparam Derived 
 */
template <typename GainMgr, typename ConstrMgr, template
          <typename _GainMgr, typename _ConstrMgr> class Derived> //
class PartMgrBase {

    using Der = Derived<GainMgr, ConstrMgr>;
    Der &self = *static_cast<Der *>(this);

  protected:
    SimpleNetlist &H;
    GainMgr &gainMgr;
    ConstrMgr &validator;
    std::uint8_t K;
    // std::vector<std::uint8_t> snapshot;
    // std::vector<std::uint8_t> part;

  public:
    int totalcost;

    /**
     * @brief Construct a new FDPartMgr object
     *
     * @param H
     * @param gainMgr
     * @param constrMgr
     */
    PartMgrBase(SimpleNetlist &H, GainMgr &gainMgr, ConstrMgr &constrMgr)
        : H{H}, gainMgr{gainMgr}, validator{constrMgr}, K{gainMgr.K},
          // part(this->H.number_of_modules(), 0),
          totalcost{0} {}

    /**
     * @brief
     *
     * @param part_info
     */
    void init(PartInfo &part_info);

    /**
     * @brief
     *
     * @param part_info
     */
    size_t legalize(PartInfo &part_info);

    /**
     * @brief
     *
     * @param part_info
     */
    void optimize(PartInfo &part_info);

  private:
    /**
     * @brief
     *
     * @param part_info
     */
    void optimize_1pass(PartInfo &part_info);

    /**
     * @brief
     *
     * @param part_info
     * @return Snapshot
     */
    auto take_snapshot(const PartInfo &part_info) -> Snapshot;

    /**
     * @brief
     *
     * @param snapshot
     * @return PartInfo
     */
    auto restore_part_info(Snapshot &snapshot) -> PartInfo;
};

#endif