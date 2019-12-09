#include <ckpttncpp/FMKWayGainCalc.hpp>
#include <ckpttncpp/FMKWayGainMgr.hpp>

/**
 * @brief
 *
 * @param part
 * @return int
 */
int FMKWayGainMgr::init(gsl::span<const uint8_t> part)
{
    auto totalcost = Base::init(part);

    for (auto k = 0U; k != this->K; ++k)
    {
        this->gainbucket[k].clear();
    }
    for (auto&& v : this->H.modules)
    {
        const auto pv = part[v];
        for (auto&& k : this->RR.exclude(pv))
        {
            auto& vlink = this->gainCalc.vertex_list[k][v];
            this->gainbucket[k].append_direct(vlink);
        }
        auto& vlink = this->gainCalc.vertex_list[pv][v];
        this->gainbucket[pv].set_key(vlink, 0);
        this->waitinglist.append(vlink);
    }
    for (auto&& v : this->H.module_fixed)
    {
        this->lock_all(part[v], v);
    }
    return totalcost;
}

/**
 * @brief
 *
 * @param part
 * @param move_info_v
 * @param gain
 */
void FMKWayGainMgr::update_move_v(const MoveInfoV& move_info_v, int gain)
{
    const auto& [fromPart, toPart, v] = move_info_v;

    for (auto k = 0U; k != this->K; ++k)
    {
        if (fromPart == k || toPart == k)
        {
            continue;
        }
        this->gainbucket[k].modify_key(
            this->gainCalc.vertex_list[k][v], this->gainCalc.deltaGainV[k]);
    }
    this->__set_key(fromPart, v, -gain);
    // this->__set_key(toPart, v, -2*this->pmax);
}
