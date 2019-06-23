#include <ckpttncpp/FMKWayGainCalc.hpp>
#include <ckpttncpp/FMKWayGainMgr.hpp>

/**
 * @brief
 *
 * @param part_info
 * @return int
 */
auto FMKWayGainMgr::init(const std::vector<uint8_t> &part) -> int {
    auto totalcost = Base::init(part);

    for (auto k = 0U; k < this->K; ++k) {
        this->gainbucket[k].clear();
    }
    for (auto i_v = 0U; i_v < this->H.number_of_modules(); ++i_v) {
        auto pv = part[i_v];
        for (auto &&k : this->RR.exclude(pv)) {
            auto &vlink = this->gainCalc.vertex_list[k][i_v];
            this->gainbucket[k].append_direct(vlink);
        }
        auto &vlink = this->gainCalc.vertex_list[pv][i_v];
        this->gainbucket[pv].set_key(vlink, 0);
        this->waitinglist.append(vlink);
    }
    for (auto v : this->H.module_fixed) {
        auto i_v = this->H.module_map[v];
        this->lock_all(part[i_v], i_v);
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
auto FMKWayGainMgr::update_move_v(const MoveInfoV &move_info_v, int gain)
    -> void {
    auto &&[fromPart, toPart, i_v] = move_info_v;

    for (auto k = 0U; k < this->K; ++k) {
        if (fromPart == k || toPart == k) {
            continue;
        }
        this->gainbucket[k].modify_key(this->gainCalc.vertex_list[k][i_v],
                                        this->gainCalc.deltaGainV[k]);
    }
    this->set_key(fromPart, i_v, -gain);
    // this->set_key(toPart, v, -2*this->pmax);
}
