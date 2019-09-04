#pragma once

// **Special code for two-pin nets**
// Take a snapshot when a move make **negative** gain.
// Snapshot in the form of "interface"???
#include "PartMgrBase.hpp"

/*!
 * @brief FM Partition Manager
 *
 * @tparam GainMgr
 * @tparam ConstrMgr
 */
template<typename GainMgr, typename ConstrMgr> //
class FMPartMgr : public PartMgrBase<GainMgr, ConstrMgr, FMPartMgr>
{
    using Base = PartMgrBase<GainMgr, ConstrMgr, FMPartMgr>;

public:
    /*!
     * @brief Construct a new FMPartMgr object
     *
     * @param H
     * @param gainMgr
     * @param constrMgr
     */
    FMPartMgr(SimpleNetlist& H, GainMgr& gainMgr, ConstrMgr& constrMgr)
        : Base{H, gainMgr, constrMgr}
    {
    }

    /*!
     * @brief
     *
     * @param part
     * @return Snapshot
     */
    auto take_snapshot(const std::vector<uint8_t>& part) -> std::vector<uint8_t>
    {
        auto snapshot = part;
        return snapshot;
    }

    /*!
     * @brief
     *
     * @param snapshot
     * @param part
     */
    auto restore_part(std::vector<uint8_t>& snapshot, std::vector<uint8_t>& part) -> void
    {
        part.swap(snapshot);
    }
};
