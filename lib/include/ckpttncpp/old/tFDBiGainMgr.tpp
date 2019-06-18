#ifndef CKPTTNCPP_FDBIGAINMGR_HPP
#define CKPTTNCPP_FDBIGAINMGR_HPP 1

#include "FDBiGainCalc.hpp"
#include "FDGainMgr.hpp"

/*!
 * @brief FDBiGainMgr
 *
 */
struct FDBiGainMgr : public FDGainMgr<FDBiGainCalc, FDBiGainMgr> {
    using Base = FDGainMgr<FDBiGainCalc, FDBiGainMgr>;
    using GainCalc_ = FDBiGainCalc;
    using index_t = typename SimpleNetlist::index_t;

    explicit FDBiGainMgr(SimpleNetlist &H) : Base{H, 2} {}

    /*!
     * @brief Construct a new FDBiGainMgr object
     *
     * @param H
     * @param K
     */
    FDBiGainMgr(SimpleNetlist &H, uint8_t /* K */) : Base{H, 2} {}

    /*!
     * @brief
     *
     * @param part_info
     * @return int
     */
    auto init(const std::vector<uint8_t> &part) -> int;

    /*!
     * @brief (needed by base class)
     *
     * @param w
     * @param part_w
     * @param key
     */
    auto modify_key(index_t i_w, uint8_t part_w, int key) -> void {
        this->gainbucket[1 - part_w].modify_key(
            this->gainCalc.vertex_list[i_w], key);
    }

    /*!
     * @brief
     *
     * @param part
     * @param move_info_v
     * @param gain
     */
    auto update_move_v(const MoveInfoV &move_info_v, int gain) -> void {
        auto &&[fromPart, _, i_v] = move_info_v;
        this->set_key(fromPart, i_v, -gain);
    }

    auto lock(uint8_t whichPart, index_t i_v) -> void {
        auto &vlink = this->gainCalc.vertex_list[i_v];
        this->gainbucket[whichPart].detach(vlink);
        vlink.lock();
    }

    auto lock_all(uint8_t fromPart, index_t i_v) -> void {
        this->lock(1 - fromPart, i_v);
    }

  private:
    /*!
     * @brief Set the key object
     *
     * @param whichPart
     * @param v
     * @param key
     */
    auto set_key(uint8_t whichPart, index_t i_v, int key) -> void {
        this->gainbucket[whichPart].set_key(this->gainCalc.vertex_list[i_v],
                                             key);
    }
};

#endif
