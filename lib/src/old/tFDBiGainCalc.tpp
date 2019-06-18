#include <ckpttncpp/FDBiGainCalc.hpp>
#include <functional>

/* linux-2.6.38.8/include/linux/compiler.h */
#include <cstdio>
#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

/*!
 * @brief init_gain
 *
 * @param net
 * @param part_info
 */
void FDBiGainCalc::init_gain( //
    node_t net, const std::vector<uint8_t> &part) {
    auto degree = this->H.G.degree(net);
    if (unlikely(degree < 2)) {
        return; // does not provide any gain when moving
    }
    auto &&[part, extern_nets] = part_info;
    auto weight = this->H.get_net_weight(net);
    if (extern_nets.contains(net)) {
        this->totalcost += weight;
        switch (degree) {
        case 2:
            for (auto w : this->H.G[net]) {
                auto i_w = this->H.module_map[w];
                this->modify_gain(i_w, weight);
            }
            break;
        case 3:
            this->init_gain_3pin_net(net, part, weight);
            break;
        default:
            this->init_gain_general_net(net, part, weight);
        }
    } else { // 90%
        for (auto w : this->H.G[net]) {
            auto i_w = this->H.module_map[w];
            this->modify_gain(i_w, -weight);
        }
    }
}

/*!
 * @brief
 *
 * @param net
 * @param part
 * @param weight
 */
void FDBiGainCalc::init_gain_3pin_net(node_t net,
                                      const std::vector<uint8_t> &part,
                                      int weight) {
    auto netCur = this->H.G[net].begin();
    auto w = *netCur;
    auto v = *++netCur;
    auto u = *++netCur;
    auto i_w = this->H.module_map[w];
    auto i_v = this->H.module_map[v];
    auto i_u = this->H.module_map[u];

    if (part[i_u] == part[i_v]) {
        this->modify_gain(i_w, weight);
    } else if (part[i_w] == part[i_v]) {
        this->modify_gain(u, weight);
    } else { // part[i_u] == part[i_w]
        this->modify_gain(i_v, weight);
    }
}

/*!
 * @brief
 *
 * @param net
 * @param part
 * @param weight
 */
void FDBiGainCalc::init_gain_general_net(node_t net,
                                         const std::vector<uint8_t> &part,
                                         int weight) {
    index_t num[2] = {0, 0};
    auto IdVec = std::vector<index_t>{};
    for (auto const &w : this->H.G[net]) {
        auto i_w = this->H.module_map[w];
        num[part[i_w]] += 1;
        IdVec.push_back(i_w);
    }
    for (auto &&k : {0, 1}) {
        if (num[k] == 1) {
            for (auto i_w : IdVec) {
                if (part[i_w] == k) {
                    this->modify_gain(i_w, weight);
                    break;
                }
            }
        }
    }
}

/*!
 * @brief
 *
 * @param part
 * @param move_info
 * @return ret_2pin_info
 */
auto FDBiGainCalc::update_move_2pin_net(std::vector<uint8_t> &part,
                                        const MoveInfo &move_info)
    -> ret_2pin_info {
    auto &&[net, fromPart, _, v] = move_info;
    // auto &[part, extern_nets] = part_info;
    auto netCur = this->H.G[net].begin();
    auto w = (*netCur != v) ? *netCur : *++netCur;
    auto i_w = this->H.module_map[w];

    auto weight = this->H.get_net_weight(net);
    if (part[i_w] == fromPart) {
        extern_nets.insert(net);
    } else {
        extern_nets.erase(net);
        weight = -weight;
    }
    return std::tuple{i_w, 2 * weight};
}

/*!
 * @brief Update move for 3-pin nets
 *
 * @param part
 * @param move_info
 * @return ret_info
 */
auto FDBiGainCalc::update_move_3pin_net(std::vector<uint8_t> &part,
                                        const MoveInfo &move_info) -> ret_info {
    auto &&[net, fromPart, _, v] = move_info;
    // auto &[part, extern_nets] = part_info;
    index_t num[2] = {0, 0};
    auto IdVec = std::vector<index_t>{};
    for (auto const &w : this->H.G[net]) {
        if (w == v) {
            continue;
        }
        auto i_w = this->H.module_map[w];
        IdVec.push_back(i_w);
    }
    auto deltaGain = std::vector{0, 0};
    auto weight = this->H.get_net_weight(net);

    auto part_w = part[IdVec[0]];
    if (part_w == part[IdVec[1]]) {
        if (part_w == fromPart) {
            extern_nets.insert(net);
            deltaGain[0] += weight;
            deltaGain[1] += weight;
        } else { // part_w == toPart
            extern_nets.erase(net);
            deltaGain[0] -= weight;
            deltaGain[1] -= weight;
        }
    } else {
        if (part_w == fromPart) {
            deltaGain[0] += weight;
            deltaGain[1] -= weight;
        } else { // part_w == toPart
            deltaGain[0] -= weight;
            deltaGain[1] += weight;
        }
    }
    return std::tuple{std::move(IdVec), std::move(deltaGain)};
}

/*!
 * @brief Update move for general nets
 *
 * @param part
 * @param move_info
 * @return ret_info
 */
auto FDBiGainCalc::update_move_general_net(std::vector<uint8_t> &part,
                                           const MoveInfo &move_info)
    -> ret_info {
    auto &&[net, fromPart, toPart, v] = move_info;
    // auto &[part, extern_nets] = part_info;
    index_t num[2] = {0, 0};
    auto IdVec = std::vector<index_t>{};
    for (auto const &w : this->H.G[net]) {
        if (w == v) {
            continue;
        }
        auto i_w = this->H.module_map[w];
        num[part[i_w]] += 1;
        IdVec.push_back(i_w);
    }
    auto degree = std::size(IdVec);
    auto deltaGain = std::vector(degree, 0);
    auto weight = this->H.get_net_weight(net);
    std::function<void(node_t)> action1 = [&](node_t n) -> void {
        extern_nets.erase(n);
    };
    std::function<void(node_t)> action2 = [&](node_t n) -> void {
        extern_nets.insert(n);
    };
    auto action = std::vector{action1, action2};
    auto l = fromPart, u = toPart;
    for (auto &&i : {0, 1}) {
        if (num[l] == 0) {
            action[i](net);
            for (auto idx = 0U; idx < degree; ++idx) {
                deltaGain[idx] -= weight;
            }
        } else if (num[l] == 1) {
            for (auto idx = 0U; idx < degree; ++idx) {
                auto part_w = part[IdVec[idx]];
                if (part_w == l) {
                    deltaGain[idx] += weight;
                    break;
                }
            }
        }
        weight = -weight;
        std::swap(l, u);
    }
    return std::tuple{std::move(IdVec), std::move(deltaGain)};
}
