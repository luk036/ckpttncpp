#ifndef _HOME_UBUNTU_GITHUB_CKPTTNCPP_FMKWayGAINMGR_HPP
#define _HOME_UBUNTU_GITHUB_CKPTTNCPP_FMKWayGAINMGR_HPP 1

#include "FMGainMgr.hpp"
#include "FMKWayGainCalc.hpp"

/**
 * @brief FMKWayGainMgr
 *
 */
class FMKWayGainMgr;

class FMKWayGainMgr : public FMGainMgr<FMKWayGainCalc, FMKWayGainMgr> {
    using Base = FMGainMgr<FMKWayGainCalc, FMKWayGainMgr>;
  private:
    robin RR;

  public:
    /**
     * @brief Construct a new FMKWayGainMgr object
     *
     * @param H
     * @param K
     */
    FMKWayGainMgr(SimpleNetlist &H, std::uint8_t K) : Base{H, K}, RR{K} {}

    /**
     * @brief
     *
     * @param part
     */
    auto init(const std::vector<std::uint8_t> &part) -> void;

    /**
     * @brief (needed by base class)
     *
     * @param part
     * @param w
     * @param keys
     */
    auto modify_key(node_t w, std::uint8_t part_w,
                    const std::vector<int> &keys) -> void {
        for (auto k : this->RR.exclude(part_w)) {
            this->gainbucket[k]->modify_key(this->gainCalc.vertex_list[k][w],
                                            keys[k]);
        }
    }

    /**
     * @brief
     *
     * @param part
     * @param move_info_v
     * @param gain
     */
    auto update_move_v(const std::vector<std::uint8_t> &part,
                       const MoveInfoV &move_info_v, int gain) -> void;

  private:
    /**
     * @brief Set the key object
     *
     * @param whichPart
     * @param v
     * @param key
     */
    auto set_key(size_t whichPart, node_t v, int key) -> void {
        this->gainbucket[whichPart]->set_key(
            this->gainCalc.vertex_list[whichPart][v], key);
    }
};

#endif
