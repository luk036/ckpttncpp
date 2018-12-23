#include <ckpttncpp/FMKWayGainCalc.hpp>
#include <ckpttncpp/FMKWayGainMgr.hpp>

/**
 * @brief
 *
 * @param part
 */
auto FMKWayGainMgr::init(const std::vector<std::uint8_t> &part) -> void {
    Base::init(part);

    for (auto k = 0u; k < this->K; ++k) {
        this->gainbucket[k]->clear();
    }

    for (auto v = 0u; v < this->H.number_of_modules(); ++v) {
        auto pv = part[v];
        for (auto k : this->RR.exclude(pv)) {
            auto &vlink = this->gainCalc.vertex_list[k][v];
            this->gainbucket[k]->append_direct(vlink);
        }
        auto &vlink = this->gainCalc.vertex_list[pv][v];
        this->gainbucket[pv]->set_key(vlink, -2*this->pmax);
        this->waitinglist.append(vlink);
    }
}

/**
 * @brief
 *
 * @param part
 * @param move_info_v
 * @param gain
 */
auto FMKWayGainMgr::update_move_v(const std::vector<std::uint8_t> &part,
                                  const MoveInfoV &move_info_v, int gain)
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
    this->set_key(toPart, v, -2*this->pmax);
}
