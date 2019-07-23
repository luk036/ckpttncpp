#include <ckpttncpp/FDKWayGainCalc.hpp>
#include <functional>

/* linux-2.6.38.8/include/linux/compiler.h */
#include <cstdio>
#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

/**
 * @brief
 *
 * @param net
 * @param part
 * @param vertex_list
 */
auto FDKWayGainCalc::__init_gain(node_t net, const std::vector<uint8_t> &part) -> void {
    auto degree = this->H.G.degree(net);
    if (unlikely(degree < 2)) {
        return; // does not provide any gain when moving
    }
    auto &&[part, extern_nets] = part_info;
    if (extern_nets.contains(net)) {
        switch (degree) {
        case 2:
            this->__init_gain_2pin_net(net, part);
            break;
        case 3:
            this->__init_gain_3pin_net(net, part);
            break;
        default:
            this->__init_gain_general_net(net, part);
        }
    } else { // 90%
        auto weight = this->H.get_net_weight(net);
        for (const auto &w : this->H.G[net]) {
            auto i_w = this->H.module_map[w];
            for (auto k : this->RR.exclude(part[i_w])) {
                this->vertex_list[k][i_w].key -= weight;
            }
        }
    }
}

/**
 * @brief __init_gain_2pin_net
 *
 * @param net
 * @param part
 */
auto FDKWayGainCalc::__init_gain_2pin_net(node_t net,
                                        const std::vector<uint8_t> &part)
    -> void {
    auto weight = this->H.get_net_weight(net);
    this->totalcost += weight;
    auto netCur = this->H.G[net].begin();
    auto w = *netCur;
    auto v = *++netCur;
    auto i_w = this->H.module_map[w];
    auto i_v = this->H.module_map[v];
    auto part_w = part[i_w];
    auto part_v = part[i_v];
    this->vertex_list[part_v][i_w].key += weight;
    this->vertex_list[part_w][i_v].key += weight;
}

/**
 * @brief
 *
 * @param net
 * @param part
 */
auto FDKWayGainCalc::__init_gain_3pin_net(node_t net,
                                        const std::vector<uint8_t> &part)
    -> void {
    auto netCur = this->H.G[net].begin();
    auto w = *netCur;
    auto v = *++netCur;
    auto u = *++netCur;
    auto i_w = this->H.module_map[w];
    auto i_v = this->H.module_map[v];
    auto i_u = this->H.module_map[u];
    auto part_w = part[i_w];
    auto part_v = part[i_v];
    auto part_u = part[i_u];

    auto weight = this->H.get_net_weight(net);

    if (part_u == part_v) {
        this->vertex_list[part_v][i_w].key += weight;
        for (auto i_a : {i_u, i_v}) {
            this->__modify_gain(i_a, part_v, -weight);
            this->vertex_list[part_w][i_a].key += weight;
        }
    } else if (part_w == part_v) {
        this->vertex_list[part_v][i_u].key += weight;
        for (auto i_a : {i_w, i_v}) {
            this->__modify_gain(i_a, part_v, -weight);
            this->vertex_list[part_u][i_a].key += weight;
        }
    } else if (part_w == part_u) {
        this->vertex_list[part_w][i_v].key += weight;
        for (auto i_a : {i_w, i_u}) {
            this->__modify_gain(i_a, part_w, -weight);
            this->vertex_list[part_v][i_a].key += weight;
        }
    } else {
        this->vertex_list[part_v][i_u].key += weight;
        this->vertex_list[part_w][i_u].key += weight;
        this->vertex_list[part_w][i_v].key += weight;
        this->vertex_list[part_u][i_v].key += weight;
        this->vertex_list[part_u][i_w].key += weight;
        this->vertex_list[part_v][i_w].key += weight;
        this->totalcost += weight;
    }
    this->totalcost += weight;
}

/**
 * @brief
 *
 * @param net
 * @param part
 */
auto FDKWayGainCalc::__init_gain_general_net(
    node_t net, const std::vector<uint8_t> &part) -> void {
    std::vector<uint8_t> num(this->K, 0);
    auto IdVec = std::vector<index_t>{};
    for (const auto &w : this->H.G[net]) {
        auto i_w = this->H.module_map[w];
        num[part[i_w]] += 1;
        IdVec.push_back(i_w);
    }
    auto weight = this->H.get_net_weight(net);
    for (auto k = 0U; k < this->K; ++k) {
        if (num[k] > 0) {
            this->totalcost += weight;
            if (num[k] == 1) {
                for (auto i_w : IdVec) {
                    if (part[i_w] == k) {
                        this->__modify_gain(i_w, part[i_w], weight);
                        break;
                    }
                }
            }
        } else { // num[k] == 0
            for (auto i_w : IdVec) {
                vertex_list[k][i_w].key -= weight;
            }
        }
    }
    this->totalcost -= weight;
}

auto FDKWayGainCalc::update_move_2pin_net(std::vector<uint8_t> &part,
                                          const MoveInfo &move_info)
    -> ret_2pin_info {
    auto &&[net, fromPart, toPart, v] = move_info;
    // auto &[part, extern_nets] = part_info;
    auto netCur = this->H.G[net].begin();
    node_t w = (*netCur != v) ? *netCur : *++netCur;
    auto i_w = this->H.module_map[w];
    auto part_w = part[i_w];
    auto weight = this->H.get_net_weight(net);
    auto deltaGainW = std::vector(this->K, 0);
    // auto deltaGainV = std::vector(this->K, 0);
    if (part_w == fromPart) {
        extern_nets.insert(net);
        for (auto k = 0U; k < this->K; ++k) {
            deltaGainW[k] += weight;
            this->deltaGainV[k] += weight;
        }
    } else if (part_w == toPart) {
        extern_nets.erase(net);
        for (auto k = 0U; k < this->K; ++k) {
            deltaGainW[k] -= weight;
            this->deltaGainV[k] -= weight;
        }
    }
    deltaGainW[fromPart] -= weight;
    deltaGainW[toPart] += weight;
    return std::tuple{i_w, std::move(deltaGainW)};
}

/**
 * @brief
 *
 * @param part_info
 * @param move_info
 * @return ret_info
 */
auto FDKWayGainCalc::update_move_3pin_net(std::vector<uint8_t> &part,
                                          const MoveInfo &move_info)
    -> ret_info {
    auto &&[net, fromPart, toPart, v] = move_info;
    // auto &[part, extern_nets] = part_info;
    auto IdVec = std::vector<index_t>{};
    for (const auto &w : this->H.G[net]) {
        if (w == v) {
            continue;
        }
        auto i_w = this->H.module_map[w];
        IdVec.push_back(i_w);
    }
    auto degree = IdVec.size();
    auto deltaGain = std::vector(degree, std::vector(this->K, 0));
    auto weight = this->H.get_net_weight(net);

    auto l = fromPart, u = toPart;
    auto part_w = part[IdVec[0]];
    auto part_u = part[IdVec[1]];
    action1 = [&](node_t n) -> void { extern_nets.erase(n); };
    action2 = [&](node_t n) -> void { extern_nets.insert(n); };
    auto action = std::vector{action1, action2};

    if (part_w == part_u) {
        for (auto &&i : {0, 1}) {
            if (part_w != l) {
                deltaGain[0][l] -= weight;
                deltaGain[1][l] -= weight;
                if (part_w == u) {
                    for (auto k = 0U; k < this->K; ++k) {
                        this->deltaGainV[k] -= weight;
                    }
                    action[i](net);
                }
            }
            weight = -weight;
            std::swap(l, u);
        }
        return std::tuple{std::move(IdVec), std::move(deltaGain)};
    }

    for (auto &&_ : {0, 1}) {
        if (part_w == l) {
            for (auto k = 0U; k < this->K; ++k) {
                deltaGain[0][k] += weight;
            }
        } else if (part_u == l) {
            for (auto k = 0U; k < this->K; ++k) {
                deltaGain[1][k] += weight;
            }
        } else {
            deltaGain[0][l] -= weight;
            deltaGain[1][l] -= weight;
            if (part_w == u || part_u == u) {
                for (auto k = 0U; k < this->K; ++k) {
                    this->deltaGainV[k] -= weight;
                }
            }
        }
        weight = -weight;
        std::swap(l, u);
    }

    return std::tuple{std::move(IdVec), std::move(deltaGain)};
    // return this->update_move_general_net(part, move_info);
}

/**
 * @brief
 *
 * @param part_info
 * @param move_info
 * @return ret_info
 */
auto FDKWayGainCalc::update_move_general_net(std::vector<uint8_t> &part,
                                             const MoveInfo &move_info)
    -> ret_info {
    auto &&[net, fromPart, toPart, v] = move_info;
    // auto &[part, extern_nets] = part_info;
    std::vector<uint8_t> num(this->K, 0);
    auto IdVec = std::vector<index_t>{};
    for (const auto &w : this->H.G[net]) {
        if (w == v) {
            continue;
        }
        auto i_w = this->H.module_map[w];
        num[part[i_w]] += 1;
        IdVec.push_back(i_w);
    }
    auto degree = IdVec.size();
    auto deltaGain = std::vector(degree, std::vector(this->K, 0));
    // auto deltaGainV = std::vector(this->K, 0);
    // auto m = this->H.G[net].get('weight', 1);
    auto weight = this->H.get_net_weight(net);

    auto count = 0;
    for (auto k = 0U; k < this->K; ++k) {
        if (num[k] > 0) {
            count += 1;
        }
    }

    action1 = [&](node_t n) -> void { extern_nets.erase(n); };
    action2 = [&](node_t n) -> void { extern_nets.insert(n); };
    auto action = std::vector{action1, action2};

    // if (num[fromPart] == 0) {
    //     if (num[toPart] > 0) {
    //         for (auto k = 0U; k < this->K; ++k) {
    //             this->deltaGainV[k] -= weight;
    //         }
    //         if (count == 1) {
    //             extern_nets.erase(net);
    //         }
    //     }
    // } else { // num[fromPart] > 0
    //     if (num[toPart] == 0) {
    //         for (auto k = 0U; k < this->K; ++k) {
    //             this->deltaGainV[k] += weight;
    //         }
    //         if (count == 1) {
    //             extern_nets.insert(net);
    //         }
    //     }
    // }

    auto l = fromPart, u = toPart;
    // for (auto l : {fromPart, toPart}) {
    for (auto &&i : {0, 1}) {
        if (num[l] == 0) {
            for (auto idx = 0U; idx < degree; ++idx) {
                deltaGain[idx][l] -= weight;
            }
            if (num[u] > 0) {
                for (auto k = 0U; k < this->K; ++k) {
                    this->deltaGainV[k] -= weight;
                }
                if (count == 1) {
                    action[i](net);
                }
            }
        } else if (num[l] == 1) {
            for (auto idx = 0U; idx < degree; ++idx) {
                if (part[IdVec[idx]] == l) {
                    for (auto k = 0U; k < this->K; ++k) {
                        deltaGain[idx][k] += weight;
                    }
                    break;
                }
            }
        }
        weight = -weight;
    }
    return std::tuple{std::move(IdVec), std::move(deltaGain)};
}
