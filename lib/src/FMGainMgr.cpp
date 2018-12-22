#include <ckpttncpp/FMGainMgr.hpp>
// #include <ckpttncpp/FMGainCalc.hpp>

/* linux-2.6.38.8/include/linux/compiler.h */
#include <stdio.h>
#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

/**
 * @brief Construct a new FMGainMgr object
 *
 * @param H
 * @param K
 */
template <typename GainCalc, class Derived>
FMGainMgr<GainCalc, Derived>::FMGainMgr(SimpleNetlist &H, std::uint8_t K)
    : H{H}, K{K}, gainCalc{H, K}, pmax{H.get_max_degree()}, waitinglist{},
      totalcost{0}
{
    static_assert(std::is_base_of_v<FMGainMgr<GainCalc, Derived>, Derived>);
    for (auto k = 0u; k < this->K; ++k) {
        this->gainbucket.push_back(
            std::make_unique<bpqueue>(-this->pmax, this->pmax));
    }
}

/**
 * @brief
 *
 * @param part
 */
template <typename GainCalc, class Derived>
auto FMGainMgr<GainCalc, Derived>::init(const std::vector<std::uint8_t> &part)
    -> void {
    this->gainCalc.init(part);
    this->totalcost = this->gainCalc.totalcost;
    this->waitinglist.clear();
    
    for (auto v : this->H.module_fixed) {
        this->gainCalc.set_key(v, -2*this->pmax);
    }
}

/**
 * @brief
 *
 * @param part
 * @return std::tuple<MoveInfoV, int>
 */
template <typename GainCalc, class Derived>
auto FMGainMgr<GainCalc, Derived>::select(const std::vector<std::uint8_t> &part)
    -> std::tuple<MoveInfoV, int> {
    auto gainmax = std::vector<int>(this->K);
    for (auto k = 0u; k < this->K; ++k) {
        gainmax[k] = this->gainbucket[k]->get_max();
    }
    auto it = std::max_element(gainmax.cbegin(), gainmax.cend());
    std::uint8_t toPart = std::distance(gainmax.cbegin(), it);
    auto &vlink = this->gainbucket[toPart]->popleft();
    this->waitinglist.append(vlink);
    // node_t v = &vlink - this->gainCalc.start_ptr(toPart);
    node_t v = std::distance(this->gainCalc.start_ptr(toPart), &vlink);
    // node_t v = this->H.modules[v];
    auto fromPart = part[v];
    auto move_info_v = MoveInfoV{fromPart, toPart, v};
    return std::tuple{std::move(move_info_v), gainmax[toPart]};
}

template <typename GainCalc, class Derived>
auto FMGainMgr<GainCalc, Derived>::select_togo(std::uint8_t toPart)
    -> std::tuple<node_t, int> {
    auto gainmax = this->gainbucket[toPart]->get_max();
    auto &vlink = this->gainbucket[toPart]->popleft();
    this->waitinglist.append(vlink);
    // node_t v = &vlink - this->gainCalc.start_ptr(toPart);
    node_t v = std::distance(this->gainCalc.start_ptr(toPart), &vlink);
    // node_t v = this->H.modules[v];
    return std::tuple{v, gainmax};
}

/**
 * @brief
 *
 * @param part
 * @param move_info_v
 * @param gain
 */
template <typename GainCalc, class Derived>
auto FMGainMgr<GainCalc, Derived>::update_move(
    const std::vector<std::uint8_t> &part, const MoveInfoV &move_info_v)
    -> void {
    // std::fill_n(this->deltaGainV.begin(), this->K, 0);
    this->gainCalc.update_move_init();

    auto const &[fromPart, toPart, v] = move_info_v;
    for (auto net : this->H.G[v]) {
        auto move_info = MoveInfo{net, fromPart, toPart, v};
        if (this->H.G.degree(net) == 2) {
            this->update_move_2pin_net(part, move_info);
        } else if (unlikely(this->H.G.degree(net) < 2)) {
            continue; // does not provide any gain change when move
        } else {
            this->update_move_general_net(part, move_info);
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
auto FMGainMgr<GainCalc, Derived>::update_move_2pin_net(
    const std::vector<std::uint8_t> &part, const MoveInfo &move_info) -> void {
    auto [w, deltaGainW] =
        this->gainCalc.update_move_2pin_net(part, move_info);
    self.modify_key(part, w, deltaGainW);
}

/**
 * @brief
 *
 * @param part
 * @param move_info
 */
template <typename GainCalc, class Derived>
auto FMGainMgr<GainCalc, Derived>::update_move_general_net(
    const std::vector<std::uint8_t> &part, const MoveInfo &move_info) -> void {
    auto [IdVec, deltaGain] =
        this->gainCalc.update_move_general_net(part, move_info);
    auto degree = std::size(IdVec);
    for (auto idx = 0u; idx < degree; ++idx) {
        self.modify_key(part, IdVec[idx], deltaGain[idx]);
    }
}

#include <ckpttncpp/FMBiGainMgr.hpp> // import FMBiGainMgr
template class FMGainMgr<FMBiGainCalc, FMBiGainMgr>;

#include <ckpttncpp/FMKWayGainMgr.hpp> // import FMKWayGainMgr
template class FMGainMgr<FMKWayGainCalc, FMKWayGainMgr>;
