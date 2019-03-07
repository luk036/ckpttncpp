#include <ckpttncpp/FDGainMgr.hpp>
// #include <ckpttncpp/FDGainCalc.hpp>

/* linux-2.6.38.8/include/linux/compiler.h */
#include <cstdio>
#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

/**
 * @brief Construct a new FDGainMgr object
 *
 * @param H
 * @param K
 */
template <typename GainCalc, class Derived>
FDGainMgr<GainCalc, Derived>::FDGainMgr(SimpleNetlist &H, std::uint8_t K)
    : H{H}, gainCalc{H, K}, pmax{H.get_max_degree()},  K{K} {
    static_assert(std::is_base_of_v<FDGainMgr<GainCalc, Derived>, Derived>);
    for (auto k = 0U; k < this->K; ++k) {
        this->gainbucket.push_back(
            std::make_unique<bpqueue<int>>(-this->pmax, this->pmax));
    }
}

/**
 * @brief
 *
 * @param part
 */
template <typename GainCalc, class Derived>
auto FDGainMgr<GainCalc, Derived>::init(const PartInfo &part_info) -> int {
    auto totalcost = this->gainCalc.init(part_info);
    this->waitinglist.clear();
    return totalcost;
}

/**
 * @brief
 *
 * @param part
 * @return std::tuple<MoveInfoV, int>
 */
template <typename GainCalc, class Derived>
auto FDGainMgr<GainCalc, Derived>::select(const std::vector<std::uint8_t> &part)
    -> std::tuple<MoveInfoV, int> {
    auto gainmax = std::vector<int>(this->K);
    for (auto k = 0U; k < this->K; ++k) {
        gainmax[k] = this->gainbucket[k]->get_max();
    }
    auto it = std::max_element(gainmax.cbegin(), gainmax.cend());
    std::uint8_t toPart = std::distance(gainmax.cbegin(), it);
    auto &vlink = this->gainbucket[toPart]->popleft();
    this->waitinglist.append(vlink);
    index_t i_v = std::distance(this->gainCalc.start_ptr(toPart), &vlink);
    // node_t v = this->H.modules[v];
    auto fromPart = part[i_v];
    auto move_info_v = MoveInfoV{fromPart, toPart, i_v};
    return std::tuple{move_info_v, gainmax[toPart]};
}

/**
 * @brief 
 * 
 * @param toPart 
 * @return std::tuple<index_t, int> 
 */
template <typename GainCalc, class Derived>
auto FDGainMgr<GainCalc, Derived>::select_togo(std::uint8_t toPart)
    -> std::tuple<index_t, int> {
    auto gainmax = this->gainbucket[toPart]->get_max();
    auto &vlink = this->gainbucket[toPart]->popleft();
    this->waitinglist.append(vlink);
    index_t i_v = std::distance(this->gainCalc.start_ptr(toPart), &vlink);
    // node_t v = this->H.modules[v];
    return std::tuple{i_v, gainmax};
}

/**
 * @brief 
 * 
 * @param part_info 
 * @param move_info_v 
 */
template <typename GainCalc, class Derived>
auto FDGainMgr<GainCalc, Derived>::update_move(PartInfo &part_info,
                                               const MoveInfoV &move_info_v)
    -> void {
    this->gainCalc.update_move_init();

    auto const &[fromPart, toPart, i_v] = move_info_v;
    auto v = this->H.modules[i_v];
    for (node_t net : this->H.G[v]) {
        auto degree = this->H.G.degree(net);
        if (unlikely(degree < 2)) {
            continue; // does not provide any gain change when moving
        }
        auto move_info = MoveInfo{net, fromPart, toPart, v};
        switch (degree) {
        case 2: 
            this->update_move_2pin_net(part_info, move_info);
            break;
        case 3:
            this->update_move_3pin_net(part_info, move_info);
            break;
        default:
            this->update_move_general_net(part_info, move_info);
        }
    }
}

/**
 * @brief
 *
 * @param part
 * @param move_info
 */
template <typename GainCalc, class Derived>
auto FDGainMgr<GainCalc, Derived>::update_move_2pin_net(
    PartInfo &part_info, const MoveInfo &move_info) -> void {
    auto [i_w, deltaGainW] =
        this->gainCalc.update_move_2pin_net(part_info, move_info);
    auto &[part, extern_nets] = part_info;
    self.modify_key(i_w, part[i_w], deltaGainW);
}

/**
 * @brief
 *
 * @param part
 * @param move_info
 */
template <typename GainCalc, class Derived>
auto FDGainMgr<GainCalc, Derived>::update_move_3pin_net(
    PartInfo &part_info, const MoveInfo &move_info) -> void {
    auto [IdVec, deltaGain] =
        this->gainCalc.update_move_3pin_net(part_info, move_info);
    auto &[part, extern_nets] = part_info;
    auto degree = std::size(IdVec);
    for (auto idx = 0U; idx < degree; ++idx) {
        auto i_w = IdVec[idx];
        self.modify_key(i_w, part[i_w], deltaGain[idx]);
    }
}

/**
 * @brief
 *
 * @param part
 * @param move_info
 */
template <typename GainCalc, class Derived>
auto FDGainMgr<GainCalc, Derived>::update_move_general_net(
    PartInfo &part_info, const MoveInfo &move_info) -> void {
    auto [IdVec, deltaGain] =
        this->gainCalc.update_move_general_net(part_info, move_info);
    auto &[part, extern_nets] = part_info;
    auto degree = std::size(IdVec);
    for (auto idx = 0U; idx < degree; ++idx) {
        auto i_w = IdVec[idx];
        self.modify_key(i_w, part[i_w], deltaGain[idx]);
    }
}

#include <ckpttncpp/FDBiGainMgr.hpp> // import FDBiGainMgr
template class FDGainMgr<FDBiGainCalc, FDBiGainMgr>;

#include <ckpttncpp/FDKWayGainMgr.hpp> // import FDKWayGainMgr
template class FDGainMgr<FDKWayGainCalc, FDKWayGainMgr>;
