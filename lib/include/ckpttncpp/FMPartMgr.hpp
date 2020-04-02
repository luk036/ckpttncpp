#pragma once

// **Special code for two-pin nets**
// Take a snapshot when a move make **negative** gain.
// Snapshot in the form of "interface"???
#include "PartMgrBase.hpp"
#include <gsl/span>

/*!
 * @brief FM Partition Manager
 *
 * @tparam GainMgr
 * @tparam ConstrMgr
 */
template <typename GainMgr, typename ConstrMgr> //
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
    FMPartMgr(const SimpleNetlist& H, GainMgr& gainMgr, ConstrMgr& constrMgr)
        : Base {H, gainMgr, constrMgr}
    {
    }

    /*!
     * @brief
     *
     * @param part
     * @return Snapshot
     */
    auto take_snapshot(gsl::span<const std::uint8_t> part) -> std::vector<std::uint8_t>
    {
        auto snapshot = std::vector<std::uint8_t> {};
        const auto N = part.size();
        snapshot.reserve(N);
        for (auto i = 0U; i != N; ++i)
        {
            snapshot[i] = part[i];
        }
        return snapshot;
    }

    /*!
     * @brief
     *
     * @param snapshot
     * @param part
     */
    auto restore_part(std::vector<std::uint8_t>& snapshot, gsl::span<std::uint8_t> part)
        -> void
    {
        const auto N = part.size();
        for (auto i = 0U; i != N; ++i)
        {
            part[i] = snapshot[i];
        }
    }
};
