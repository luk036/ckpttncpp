#include <ckpttncpp/FMKWayGainMgr.hpp>
#include <ckpttncpp/FMKWayGainCalc.hpp>

/* linux-2.6.38.8/include/linux/compiler.h */
#include <stdio.h>
#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

/**
 * @brief Construct a new FMKWayGainMgr object
 * 
 * @param H 
 * @param K 
 */
FMKWayGainMgr::FMKWayGainMgr(Netlist &H, size_t K)
    : H{H}, K{K}, pmax{H.get_max_degree()},
      num_modules{H.number_of_modules()}, waitinglist{}, deltaGainV(K, 0)
{
    for (auto k = 0u; k < this->K; ++k) {
        this->gainbucket.push_back(
            std::make_unique<bpqueue>(-this->pmax, this->pmax));
        this->vertex_list.emplace_back(
            std::vector<dllink>(this->num_modules));
    }
}

/**
 * @brief
 *
 * @param part
 */
auto FMKWayGainMgr::init(std::vector<size_t> &part) -> void
{
    auto gainCalc = FMKWayGainCalc{this->H, this->K};
    gainCalc.init(part, this->vertex_list);
    for (auto v : this->H.module_fixed) {
        for (auto k = 0u; k < this->K; ++k) {
            this->vertex_list[k][v].key = -this->pmax;
        }
    }
    for (auto v : this->H.module_list) {
        for (auto k = 0u; k < this->K; ++k) {
            auto &vlink = this->vertex_list[k][v];
            if (part[v] == k) {
                assert(vlink.key == 0);
                this->gainbucket[k]->set_key(vlink, 0);
                this->waitinglist.append(vlink);
            } else {
                this->gainbucket[k]->append(vlink, vlink.key);
            }
        }
    }
}

/**
 * @brief 
 * 
 * @param part 
 * @param move_info_v 
 * @param gain 
 */
auto FMKWayGainMgr::update_move(std::vector<size_t> &part,
            const MoveInfoV& move_info_v, int gain) -> void
{
    auto [fromPart, toPart, v] = move_info_v;
    std::fill_n(this->deltaGainV.begin(), this->K, 0);
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
    for (auto k = 0u; k < this->K; ++k) {
        if (fromPart == k || toPart == k) {
            continue;
        }
        this->gainbucket[k]->modify_key(this->vertex_list[k][v],
                                        this->deltaGainV[k]);
    }
    this->set_key(fromPart, v, -gain);
    this->set_key(toPart, v, 0); // actually don't care
}

/**
 * @brief 
 * 
 * @param part 
 * @param move_info 
 */
auto FMKWayGainMgr::update_move_2pin_net(std::vector<size_t> &part,
                          const MoveInfo& move_info) -> void
{
    auto gainCalc = FMKWayGainCalc{this->H, this->K};
    auto [w, deltaGainW, deltaGainV] =
        gainCalc.update_move_2pin_net(part, move_info);
    this->modify_key(part, w, deltaGainW);
    for (auto k = 0u; k < this->K; ++k) {
        this->deltaGainV[k] += deltaGainV[k];
    }
}

/**
 * @brief 
 * 
 * @param part 
 * @param move_info 
 */
auto FMKWayGainMgr::update_move_general_net(std::vector<size_t> &part,
                             const MoveInfo& move_info) -> void
{
    auto gainCalc = FMKWayGainCalc{this->H, this->K};
    auto [IdVec, deltaGain, deltaGainV] = 
        gainCalc.update_move_general_net(part, move_info);
    auto degree = std::size(IdVec);
    for (auto idx = 0u; idx < degree; ++idx) {
        this->modify_key(part, IdVec[idx], deltaGain[idx]);
    }
    for (auto k = 0u; k < this->K; ++k) {
        this->deltaGainV[k] += deltaGainV[k];
    }
}
