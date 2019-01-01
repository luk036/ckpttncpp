#ifndef _HOME_UBUNTU_GITHUB_CKPTTNCPP_FMPARTMGR_HPP
#define _HOME_UBUNTU_GITHUB_CKPTTNCPP_FMPARTMGR_HPP 1

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

/**
 * @brief FM Partition Manager
 * 
 * @tparam GainMgr 
 * @tparam ConstrMgr 
 */
template <typename GainMgr, typename ConstrMgr> //
class FMPartMgr : public PartMgrBase<GainMgr, ConstrMgr, FMPartMgr> {
    using Base = PartMgrBase<GainMgr, ConstrMgr, FMPartMgr>;

  public:
    /**
     * @brief Construct a new FMPartMgr object
     *
     * @param H
     * @param gainMgr
     * @param constrMgr
     */
    FMPartMgr(SimpleNetlist &H, GainMgr &gainMgr, ConstrMgr &constrMgr)
        : Base{H, gainMgr, constrMgr} {}

    /**
     * @brief
     *
     * @param part_info
     * @return Snapshot
     */
    auto take_snapshot(const PartInfo &part_info) -> std::vector<std::uint8_t> {
        auto const &[part, extern_nets] = part_info;
        auto snapshot = part;
        return std::move(snapshot);
    }

    /**
     * @brief
     *
     * @param snapshot
     * @return PartInfo
     */
    auto restore_part_info(std::vector<std::uint8_t> &snapshot) -> PartInfo {
        auto part = snapshot;
        return PartInfo{std::move(part), py::set<node_t>{} };
    }
};

#endif