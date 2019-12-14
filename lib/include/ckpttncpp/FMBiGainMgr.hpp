#pragma once

#include "FMBiGainCalc.hpp"
#include "FMGainMgr.hpp"
#include <gsl/span>

// struct FMBiGainMgr;

/*!
 * @brief FMBiGainMgr
 *
 */
class FMBiGainMgr : public FMGainMgr<FMBiGainCalc, FMBiGainMgr>
{
  public:
    using Base = FMGainMgr<FMBiGainCalc, FMBiGainMgr>;
    using GainCalc_ = FMBiGainCalc;
    using node_t = typename SimpleNetlist::node_t;

    explicit FMBiGainMgr(const SimpleNetlist& H)
        : Base {H, 2}
    {
    }

    /*!
     * @brief Construct a new FMBiGainMgr object
     *
     * @param H
     */
    FMBiGainMgr(const SimpleNetlist& H, uint8_t /* K */)
        : Base {H, 2}
    {
    }

    /*!
     * @brief
     *
     * @param part
     * @return int
     */
    auto init(gsl::span<const uint8_t> part) -> int;

    /*!
     * @brief (needed by base class)
     *
     * @param part
     * @param w
     * @param key
     */
    auto modify_key(node_t w, uint8_t part_w, int key) -> void
    {
        this->gainbucket[1 - part_w].modify_key(
            this->gainCalc.vertex_list[w], key);
    }

    /*!
     * @brief
     *
     * @param part
     * @param move_info_v
     * @param gain
     */
    auto update_move_v(const MoveInfoV& move_info_v, int gain) -> void
    {
        // this->vertex_list[v].key -= 2 * gain;
        // auto [fromPart, _ = move_info_v;
        this->_set_key(move_info_v.fromPart, move_info_v.v, -gain);
    }

    /*!
     * @brief lock
     *
     * @param whichPart
     * @param v
     */
    auto lock(uint8_t whichPart, node_t v) -> void
    {
        auto& vlink = this->gainCalc.vertex_list[v];
        this->gainbucket[whichPart].detach(vlink);
        vlink.lock();
    }

    /*!
     * @brief lock_all
     *
     * @param fromPart
     * @param v
     */
    auto lock_all(uint8_t fromPart, node_t v) -> void
    {
        this->lock(1 - fromPart, v);
    }

  private:
    /*!
     * @brief Set the key object
     *
     * @param whichPart
     * @param v
     * @param key
     */
    auto _set_key(uint8_t whichPart, node_t v, int key) -> void
    {
        this->gainbucket[whichPart].set_key(this->gainCalc.vertex_list[v], key);
    }
};
