#ifndef _HOME_UBUNTU_GITHUB_CKPTTNCPP_FDBIGAINMGR_HPP
#define _HOME_UBUNTU_GITHUB_CKPTTNCPP_FDBIGAINMGR_HPP 1

#include "FDBiGainCalc.hpp"
#include "FDGainMgr.hpp"

/**
 * @brief FDBiGainMgr
 *
 */
struct FDBiGainMgr : public FDGainMgr<FDBiGainCalc, FDBiGainMgr> {
    using Base = FDGainMgr<FDBiGainCalc, FDBiGainMgr>;

    /**
     * @brief Construct a new FDBiGainMgr object
     * 
     * @param H 
     * @param K 
     */
    explicit FDBiGainMgr(SimpleNetlist &H, std::uint8_t K=2) : Base{H} {}

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
     * @param w 
     * @param part_w 
     * @param key 
     */
    auto modify_key(node_t w, std::uint8_t part_w, int key)
        -> void {
        this->gainbucket[1 - part_w]->modify_key(
            this->gainCalc.vertex_list[w], key);
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
        auto const &[fromPart, toPart, v] = move_info_v;
        this->set_key(fromPart, v, -gain);
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
