#ifndef CKPTTNCPP_FMKWAYGAINMGR_HPP
#define CKPTTNCPP_FMKWAYGAINMGR_HPP 1

#include "FMGainMgr.hpp"
#include "FMKWayGainCalc.hpp"

/*!
 * @brief FMKWayGainMgr
 *
 */
class FMKWayGainMgr;

class FMKWayGainMgr : public FMGainMgr<FMKWayGainCalc, FMKWayGainMgr>
{

private:
    robin<uint8_t> RR;

public:
    using Base      = FMGainMgr<FMKWayGainCalc, FMKWayGainMgr>;
    using GainCalc_ = FMKWayGainCalc;
    using index_t   = typename SimpleNetlist::index_t;

    /*!
     * @brief Construct a new FMKWayGainMgr object
     *
     * @param H
     * @param K
     */
    FMKWayGainMgr(SimpleNetlist& H, uint8_t K) : Base{H, K}, RR{K} {}

    /*!
     * @brief
     *
     * @param part
     */
    auto init(const std::vector<uint8_t>& part) -> int;

    /*!
     * @brief (needed by base class)
     *
     * @param part
     * @param w
     * @param keys
     */
    auto modify_key(index_t i_w, uint8_t part_w, const std::vector<int>& keys) -> void
    {
        for (auto&& k : this->RR.exclude(part_w))
        {
            this->gainbucket[k].modify_key(this->gainCalc.vertex_list[k][i_w], keys[k]);
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
    auto lock(index_t whichPart, index_t i_v) -> void
    {
        auto& vlink = this->gainCalc.vertex_list[whichPart][i_v];
        this->gainbucket[whichPart].detach(vlink);
        vlink.lock();
    }

    /*!
     * @brief lock_all
     *
     * @param fromPart
     * @param v
     */
    auto lock_all(index_t /*fromPart*/, index_t i_v) -> void
    {
        for (auto k = 0U; k < this->K; ++k)
        {
            this->lock(k, i_v);
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
    auto set_key(index_t whichPart, index_t i_v, int key) -> void
    {
        this->gainbucket[whichPart].set_key(this->gainCalc.vertex_list[whichPart][i_v], key);
    }
};

#endif
