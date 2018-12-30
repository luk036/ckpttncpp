#ifndef _HOME_UBUNTU_GITHUB_CKPTTNCPP_FMBIGAINMGR_HPP
#define _HOME_UBUNTU_GITHUB_CKPTTNCPP_FMBIGAINMGR_HPP 1

#include "FMBiGainCalc.hpp"
#include "FMGainMgr.hpp"

struct FMBiGainMgr;

/**
 * @brief FMBiGainMgr
 *
 */
struct FMBiGainMgr : public FMGainMgr<FMBiGainCalc, FMBiGainMgr> {
    using Base = FMGainMgr<FMBiGainCalc, FMBiGainMgr>;

    /**
     * @brief Construct a new FMBiGainMgr object
     *
     * @param H
     */
    explicit FMBiGainMgr(SimpleNetlist &H, std::uint8_t K = 2) : Base{H} {}

    /**
     * @brief
     *
     * @param part
     */
    auto init(const std::vector<std::uint8_t> &part) -> int;

    /**
     * @brief (needed by base class)
     *
     * @param part
     * @param w
     * @param key
     */
    auto modify_key(node_t w, std::uint8_t part_w, int key) -> void {
        this->gainbucket[1 - part_w]->modify_key(this->gainCalc.vertex_list[w],
                                                 key);
    }

    /**
     * @brief
     *
     * @param part
     * @param move_info_v
     * @param gain
     */
    auto update_move_v(const std::vector<std::uint8_t> &part,
                       const MoveInfoV &move_info_v, int gain) -> void {
        // this->vertex_list[v].key -= 2 * gain;
        auto const &[fromPart, toPart, v] = move_info_v;
        this->set_key(fromPart, v, -gain);
    }

    /**
     * @brief lock
     * 
     * @param whichPart 
     * @param v 
     */
    auto lock(std::uint8_t whichPart, node_t v) -> void {
        auto &vlink = this->gainCalc.vertex_list[v];
        this->gainbucket[whichPart]->detach(vlink);
        vlink.lock();
    }

    /**
     * @brief lock_all
     * 
     * @param fromPart 
     * @param v 
     */
    auto lock_all(std::uint8_t fromPart, node_t v) -> void {
        auto toPart = 1 - fromPart;
        this->lock(toPart, v);
    }

  private:
    /**
     * @brief Set the key object
     *
     * @param whichPart
     * @param v
     * @param key
     */
    auto set_key(std::uint8_t whichPart, node_t v, int key) -> void {
        this->gainbucket[whichPart]->set_key(this->gainCalc.vertex_list[v],
                                             key);
    }
};

#endif
