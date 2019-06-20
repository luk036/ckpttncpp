#ifndef CKPTTNCPP_FDPARTMGR_HPP
#define CKPTTNCPP_FDPARTMGR_HPP 1

// **Special code for two-pin nets**
// Take a snapshot when a move make **negative** gain.
// Snapshot in the form of "interface"???
// #include "netlist.hpp"
// #include <cassert>
// #include <cinttypes>
// #include <iterator>
// #include <py2cpp/py2cpp.hpp>
// #include <type_traits>
// #include <vector>
#include "PartMgrBase.hpp"

/*!
 * @brief FD Partition Manager
 *
 * @tparam GainMgr
 * @tparam ConstrMgr
 */
template <typename GainMgr, typename ConstrMgr> //
class FDPartMgr : public PartMgrBase<GainMgr, ConstrMgr, FDPartMgr> {
    using Base = PartMgrBase<GainMgr, ConstrMgr, FDPartMgr>;

  public:
    /*!
     * @brief Construct a new FDPartMgr object
     *
     * @param H
     * @param gainMgr
     * @param constrMgr
     */
    FDPartMgr(SimpleNetlist &H, GainMgr &gainMgr, ConstrMgr &constrMgr)
        : Base{H, gainMgr, constrMgr} {}

    /*!
     * @brief
     *
     * @param part_info
     * @return Snapshot
     */
    auto take_snapshot(const std::vector<uint8_t> &part) -> Snapshot;

    /*!
     * @brief
     *
     * @param snapshot
     * @return PartInfo
     */
    auto restore_part_info(Snapshot &snapshot, std::vector<uint8_t> &part) -> void;
};

#endif