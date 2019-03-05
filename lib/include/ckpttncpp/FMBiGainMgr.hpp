#ifndef CKPTTNCPP_FMBIGAINMGR_HPP
#define CKPTTNCPP_FMBIGAINMGR_HPP 1

#include "FMBiGainCalc.hpp"
#include "FMGainMgr.hpp"

struct FMBiGainMgr;

/**
 * @brief FMBiGainMgr
 *
 */
struct FMBiGainMgr : public FMGainMgr<FMBiGainCalc, FMBiGainMgr> {
    using Base = FMGainMgr<FMBiGainCalc, FMBiGainMgr>;
    using GainCalc_ = FMBiGainCalc;

    FMBiGainMgr(SimpleNetlist &H) : Base{H, 2} {}

    /**
     * @brief Construct a new FMBiGainMgr object
     *
     * @param H
     */
    explicit FMBiGainMgr(SimpleNetlist &H, std::uint8_t /* K */) : Base{H, 2} {}

    /**
     * @brief 
     * 
     * @param part_info 
     * @return int 
     */
    auto init(const PartInfo &part_info) -> int;

    /**
     * @brief (needed by base class)
     *
     * @param part
     * @param w
     * @param key
     */
    auto modify_key(size_t i_w, std::uint8_t part_w, int key) -> void {
        this->gainbucket[1 - part_w]->modify_key(this->gainCalc.vertex_list[i_w],
                                                 key);
    }

    /**
     * @brief
     *
     * @param part
     * @param move_info_v
     * @param gain
     */
    auto update_move_v(const MoveInfoV &move_info_v, int gain) -> void {
        // this->vertex_list[i_v].key -= 2 * gain;
        auto const &[fromPart, toPart, i_v] = move_info_v;
        this->set_key(fromPart, i_v, -gain);
    }

    /**
     * @brief lock
     *
     * @param whichPart
     * @param v
     */
    auto lock(std::uint8_t whichPart, size_t i_v) -> void {
        auto &vlink = this->gainCalc.vertex_list[i_v];
        this->gainbucket[whichPart]->detach(vlink);
        vlink.lock();
    }

    /**
     * @brief lock_all
     *
     * @param fromPart
     * @param v
     */
    auto lock_all(std::uint8_t fromPart, size_t i_v) -> void {
        auto toPart = 1 - fromPart;
        this->lock(toPart, i_v);
    }

  private:
    /**
     * @brief Set the key object
     *
     * @param whichPart
     * @param v
     * @param key
     */
    auto set_key(std::uint8_t whichPart, size_t i_v, int key) -> void {
        this->gainbucket[whichPart]->set_key(this->gainCalc.vertex_list[i_v],
                                             key);
    }
};

#endif
