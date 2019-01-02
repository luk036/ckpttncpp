#include <ckpttncpp/FDKWayGainCalc.hpp>
#include <ckpttncpp/FDKWayGainMgr.hpp>

/**
 * @brief
 *
 * @param part
 */
auto FDKWayGainMgr::init(const PartInfo &part_info) -> int {
    auto totalcost = Base::init(part_info);
    auto const &[part, extern_nets] = part_info;

    for (auto k = 0u; k < this->K; ++k) {
        this->gainbucket[k]->clear();
    }

    for (auto v = 0u; v < this->H.number_of_modules(); ++v) {
        auto pv = part[v];
        for (auto &&k : this->RR.exclude(pv)) {
            auto &vlink = this->gainCalc.vertex_list[k][v];
            this->gainbucket[k]->append_direct(vlink);
        }
        auto &vlink = this->gainCalc.vertex_list[pv][v];
        this->gainbucket[pv]->set_key(vlink, 0);
        this->waitinglist.append(vlink);
    }
    for (auto v : this->H.module_fixed) {
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
auto FDKWayGainMgr::update_move_v(const MoveInfoV &move_info_v, int gain)
    -> void {
    auto const &[fromPart, toPart, v] = move_info_v;

    for (auto k = 0u; k < this->K; ++k) {
        if (fromPart == k || toPart == k) {
            continue;
        }
        this->gainbucket[k]->modify_key(this->gainCalc.vertex_list[k][v],
                                        this->gainCalc.deltaGainV[k]);
    }
    this->set_key(fromPart, v, -gain);
    // this->set_key(toPart, v, -2*this->pmax);
}
