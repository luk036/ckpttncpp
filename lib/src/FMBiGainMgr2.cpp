#include <ckpttncpp/FMBiGainMgr2.hpp>
#include <ckpttncpp/FMBiGainCalc.hpp>

/* linux-2.6.38.8/include/linux/compiler.h */
#include <stdio.h>
#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

/**
 * @brief
 *
 * @param part
 */
auto FMBiGainMgr::init(const std::vector<size_t> &part) -> void
{
    this->gainCalc.init(part, this->vertex_list);
    for (auto &v : this->H.module_fixed) {
        // auto i_v = this->H.module_dict[v];
        // force to the lowest gain
        this->vertex_list[v].key = -this->pmax;
    }
    for (auto v : this->H.module_list) {
        auto &vlink = this->vertex_list[v];
        auto toPart = 1 - part[v];
        this->gainbucket[toPart]->append(vlink, vlink.key);
    }
}

/**
 * @brief
 *
 * @param part
 * @param v
 */
auto FMBiGainMgr::update_move(const std::vector<size_t> &part,
                              const MoveInfoV& move_info_v,
                              int gain) -> void
{
    auto const &[fromPart, toPart, v] = move_info_v;
    for (auto net : this->H.G[v]) {
        auto move_info = MoveInfo{net, fromPart, toPart, v};
        if (this->H.G.degree(net) == 2) {
            this->update_move_2pin_net(part, move_info);
        } else if (unlikely(this->H.G.degree(net) < 2)) {
            break; // does not provide any gain change when move
        } else {
            this->update_move_general_net(part, move_info);
        }
    }
    // this->vertex_list[v].key -= 2 * gain;
    this->gainbucket[fromPart]->set_key(this->vertex_list[v], -gain);
}

/**
 * @brief
 *
 * @param net
 * @param part
 * @param fromPart
 * @param v
 */
auto FMBiGainMgr::update_move_2pin_net(const std::vector<size_t> &part,
                          const MoveInfo& move_info) -> void
{
    auto [w, deltaGainW] =
        this->gainCalc.update_move_2pin_net(part, move_info);
    this->modify_key(part, w, deltaGainW);
}

/**
 * @brief
 *
 * @param net
 * @param part
 * @param fromPart
 * @param v
 */
auto FMBiGainMgr::update_move_general_net(const std::vector<size_t> &part,
                             const MoveInfo& move_info) -> void
{
    auto [IdVec, deltaGain] =
        this->gainCalc.update_move_general_net(part, move_info);
    auto degree = std::size(IdVec);
    for (auto idx = 0u; idx < degree; ++idx) {
        this->modify_key(part, IdVec[idx], deltaGain[idx]);
    }
}
