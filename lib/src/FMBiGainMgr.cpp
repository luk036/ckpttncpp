#include <ckpttncpp/FMBiGainMgr.hpp>
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
auto FMBiGainMgr::init(std::vector<size_t> &part) -> void
{
    auto gainCalc = FMBiGainCalc{this->H};
    gainCalc.init(part, this->vertex_list);
    for (auto &v : this->H.module_fixed) {
        // auto i_v = this->H.module_dict[v];
        // force to the lowest gain
        this->vertex_list[v].key = -this->pmax;
    }
    this->gainbucket.appendfrom(this->vertex_list);
}

/**
 * @brief
 *
 * @param part
 * @param v
 */
auto FMBiGainMgr::update_move(std::vector<size_t> &part,
                              const MoveInfoV& move_info_v,
                              int gain) -> void
{
    auto [fromPart, toPart, v] = move_info_v;
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
    this->vertex_list[v].key -= 2 * gain;
}

/**
 * @brief
 *
 * @param net
 * @param part
 * @param fromPart
 * @param v
 */
auto FMBiGainMgr::update_move_2pin_net(std::vector<size_t> &part,
                          const MoveInfo& move_info) -> void
{
    auto gainCalc = FMBiGainCalc{this->H};
    auto [w, deltaGainW] =
        gainCalc.update_move_2pin_net(part, move_info);
    this->gainbucket.modify_key(this->vertex_list[w], deltaGainW);
}

/**
 * @brief
 *
 * @param net
 * @param part
 * @param fromPart
 * @param v
 */
auto FMBiGainMgr::update_move_general_net(std::vector<size_t> &part,
                             const MoveInfo& move_info) -> void
{
    auto gainCalc = FMBiGainCalc{this->H};
    auto [IdVec, deltaGain] =
        gainCalc.update_move_general_net(part, move_info);
    auto degree = std::size(IdVec);
    for (auto idx = 0u; idx < degree; ++idx) {
        this->gainbucket.modify_key(this->vertex_list[IdVec[idx]],
                                    deltaGain[idx]);
    }
}
