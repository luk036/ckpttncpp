#pragma once

#include "FMGainMgr.hpp"
#include "FMKWayGainCalc.hpp"
#include <gsl/span>

// class FMKWayGainMgr;

/*!
 * @brief FMKWayGainMgr
 *
 */
class FMKWayGainMgr : public FMGainMgr<FMKWayGainCalc, FMKWayGainMgr>
{

  private:
    robin<uint8_t> RR;

  public:
    using Base = FMGainMgr<FMKWayGainCalc, FMKWayGainMgr>;
    using GainCalc_ = FMKWayGainCalc;
    using node_t = typename SimpleNetlist::node_t;

    /*!
     * @brief Construct a new FMKWayGainMgr object
     *
     * @param H
     * @param K
     */
    FMKWayGainMgr(const SimpleNetlist& H, uint8_t K)
        : Base {H, K}
        , RR {K}
    {
    }

    /*!
     * @brief
     *
     * @param part
     */
    auto init(gsl::span<const uint8_t> part) -> int;

    /*!
     * @brief (needed by base class)
     *
     * @param part
     * @param w
     * @param keys
     */
    auto modify_key(node_t w, uint8_t part_w, const std::vector<int>& keys)
        -> void
    {
        for (auto k : this->RR.exclude(part_w))
        {
            this->gainbucket[k].modify_key(
                this->gainCalc.vertex_list[k][w], keys[k]);
        }
    }

    /*!
     * @brief
     *
     * @param part
     * @param move_info_v
     * @param gain
     */
    auto update_move_v(const MoveInfoV& move_info_v, int gain) -> void;

    /*!
     * @brief lock
     *
     * @param whichPart
     * @param v
     */
    auto lock(uint8_t whichPart, node_t v) -> void
    {
        auto& vlink = this->gainCalc.vertex_list[whichPart][v];
        this->gainbucket[whichPart].detach(vlink);
        vlink.lock();
    }

    /*!
     * @brief lock_all
     *
     * @param fromPart
     * @param v
     */
    auto lock_all(uint8_t /*fromPart*/, node_t v) -> void
    {
        for (auto k = 0U; k < this->K; ++k)
        {
            this->lock(k, v);
        }
    }

  private:
    /*!
     * @brief Set the key object
     *
     * @param whichPart
     * @param v
     * @param key
     */
    auto __set_key(uint8_t whichPart, node_t v, int key) -> void
    {
        this->gainbucket[whichPart].set_key(
            this->gainCalc.vertex_list[whichPart][v], key);
    }
};
