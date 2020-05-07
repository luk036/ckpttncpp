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
     * @param[in] H
     * @param[in,out] gainMgr
     * @param[in,out] constrMgr
     */
    FMPartMgr(const SimpleNetlist& H, GainMgr& gainMgr, ConstrMgr& constrMgr)
        : Base {H, gainMgr, constrMgr}
    {
    }

    /*!
     * @brief
     *
     * @param[in] part
     * @return Snapshot
     */
    auto take_snapshot(gsl::span<const std::uint8_t> part)
        -> std::vector<std::uint8_t>
    {
        const auto N = part.size();        
        auto snapshot = std::vector<std::uint8_t>(N, 0U);
        // snapshot.reserve(N);
        for (auto i = 0U; i != N; ++i)
        {
            snapshot[i] = part[i];
        }
        return snapshot;
    }

    /*!
     * @brief
     *
     * @param[in] snapshot
     * @param[in,out] part
     */
    auto restore_part(const std::vector<std::uint8_t>& snapshot,
        gsl::span<std::uint8_t> part) -> void
    {
        const auto N = part.size();
        for (auto i = 0U; i != N; ++i)
        {
            part[i] = snapshot[i];
        }
    }
};
