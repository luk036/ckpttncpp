#include <ckpttncpp/FDBiGainCalc.hpp>

/* linux-2.6.38.8/include/linux/compiler.h */
#include <cstdio>
#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

/**
 * @brief init_gain
 *
 * @param net
 * @param part_info
 */
void FDBiGainCalc::init_gain( //
    node_t net, const PartInfo &part_info) {
    auto degree = this->H.G.degree(net);
    if (unlikely(degree < 2)) {
        return; // does not provide any gain when move
    }
    auto const &[part, extern_nets] = part_info;
    auto weight = this->H.get_net_weight(net);
    if (extern_nets.contains(net)) {
        this->totalcost += weight;
        if (degree == 3) {
            this->init_gain_3pin_net(net, part, weight);
        } else if (degree == 2) {
            for (auto w : this->H.G[net]) {
                this->modify_gain(w, weight);
            }
        } else {
            this->init_gain_general_net(net, part, weight);
        }
    } else { // 90%
        for (auto w : this->H.G[net]) {
            this->modify_gain(w, -weight);
        }
    }
}

/**
 * @brief
 *
 * @param net
 * @param part
 * @param weight
 */
void FDBiGainCalc::init_gain_3pin_net(node_t net,
                                      const std::vector<std::uint8_t> &part,
                                      size_t weight)
{
    auto netCur = this->H.G[net].begin();
    auto w = *netCur;
    auto v = *++netCur;
    auto u = *++netCur;
    auto part_w = part[w];
    auto part_v = part[v];
    auto part_u = part[u];
    if (part_u == part_v) {
        this->modify_gain(w, weight);
    } else if (part_w == part_v) {
        this->modify_gain(u, weight);
    } else {
        this->modify_gain(v, weight);
    }
}

/**
 * @brief
 *
 * @param net
 * @param part
 * @param weight
 */
void FDBiGainCalc::init_gain_general_net(node_t net,
                                         const std::vector<std::uint8_t> &part,
                                         size_t weight) {
    size_t num[2] = {0, 0};
    auto IdVec = std::vector<size_t>{};
    for (auto const &w : this->H.G[net]) {
        // auto w = this->H.module_map[w];
        num[part[w]] += 1;
        IdVec.push_back(w);
    }
    for (auto &&k : {0, 1}) {
        if (num[k] == 1) {
            for (auto w : IdVec) {
                if (part[w] == k) {
                    this->modify_gain(w, weight);
                    break;
                }
            }
        }
    }
}

/**
 * @brief
 *
 * @param part
 * @param move_info
 * @return ret_2pin_info
 */
auto FDBiGainCalc::update_move_2pin_net(PartInfo &part_info,
                                        const MoveInfo &move_info)
    -> ret_2pin_info {
    auto const &[net, fromPart, toPart, v] = move_info;
    auto &[part, extern_nets] = part_info;
    auto netCur = this->H.G[net].begin();
    node_t w = (*netCur != v) ? *netCur : *++netCur;
    // auto w = this->H.module_map[w];
    auto part_w = part[w];
    auto weight = this->H.get_net_weight(net);
    int deltaGainW;
    if (part_w == fromPart) {
        deltaGainW = 2 * weight;
        extern_nets.insert(net);
    } else {
        deltaGainW = -2 * weight;
        extern_nets.erase(net);
    }
    return std::tuple{w, deltaGainW};
}

/**
 * @brief Update move for general nets
 *
 * @param part
 * @param move_info
 * @return ret_info
 */
auto FDBiGainCalc::update_move_general_net(PartInfo &part_info,
                                           const MoveInfo &move_info)
    -> ret_info {
    auto const &[net, fromPart, toPart, v] = move_info;
    auto &[part, extern_nets] = part_info;
    size_t num[2] = {0, 0};
    auto IdVec = std::vector<size_t>{};
    for (auto const &w : this->H.G[net]) {
        if (w == v) {
            continue;
        }
        // auto w = this->H.module_map[w];
        num[part[w]] += 1;
        IdVec.push_back(w);
    }
    auto degree = std::size(IdVec);
    auto deltaGain = std::vector(degree, 0);
    auto weight = this->H.get_net_weight(net);

    if (num[fromPart] == 0) {
        extern_nets.erase(net);
        for (auto idx = 0U; idx < degree; ++idx) {
            deltaGain[idx] -= weight;
        }
        return std::tuple{std::move(IdVec), std::move(deltaGain)};
    }
    if (num[toPart] == 0) {
        extern_nets.insert(net);
        for (auto idx = 0U; idx < degree; ++idx) {
            deltaGain[idx] += weight;
        }
        return std::tuple{std::move(IdVec), std::move(deltaGain)};
    }
    for (auto &&l : {fromPart, toPart}) {
        if (num[l] == 1) {
            for (auto idx = 0U; idx < degree; ++idx) {
                auto part_w = part[IdVec[idx]];
                if (part_w == l) {
                    deltaGain[idx] += weight;
                    break;
                }
            }
        }
        weight = -weight;
    }
    return std::tuple{std::move(IdVec), std::move(deltaGain)};
}
