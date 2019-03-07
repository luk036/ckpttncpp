#include <ckpttncpp/FMKWayGainCalc.hpp>

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
auto FMKWayGainCalc::init_gain(node_t net, const PartInfo &part_info) -> void {
    auto degree = this->H.G.degree(net);
    if (unlikely(degree < 2)) {
        return; // does not provide any gain when moving
    }
    auto const &[part, extern_nets] = part_info;
    switch (degree) {
    case 2:
        this->init_gain_2pin_net(net, part);
        break;
    case 3:
        this->init_gain_3pin_net(net, part);
        break;
    default:
        this->init_gain_general_net(net, part);
    }
}

/**
 * @brief
 *
 * @param net
 * @param part
 */
auto FMKWayGainCalc::init_gain_2pin_net(node_t net,
                                        const std::vector<std::uint8_t> &part)
    -> void {
    auto netCur = this->H.G[net].begin();
    auto w = *netCur;
    auto v = *++netCur;
    auto i_w = this->H.module_map[w];
    auto i_v = this->H.module_map[v];
    auto part_w = part[i_w];
    auto part_v = part[i_v];
    auto weight = this->H.get_net_weight(net);
    if (part_v == part_w) {
        for (auto i_a : {i_w, i_v}) {
            this->modify_gain(i_a, part_v, -weight);
        }
    } else {
        this->totalcost += weight;
        this->vertex_list[part_v][i_w].key += weight;
        this->vertex_list[part_w][i_v].key += weight;
    }
}

/**
 * @brief
 *
 * @param net
 * @param part
 */
auto FMKWayGainCalc::init_gain_3pin_net(node_t net,
                                        const std::vector<std::uint8_t> &part)
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
        if (part_w == part_v) {
            for (auto i_a : {i_u, i_v, i_w}) {
                this->modify_gain(i_a, part_v, -weight);
            }
            return;
        }
        this->vertex_list[part_v][i_w].key += weight;
        for (auto i_a : {i_u, i_v}) {
            this->modify_gain(i_a, part_v, -weight);
            this->vertex_list[part_w][i_a].key += weight;
        }
    } else if (part_w == part_v) {
        this->vertex_list[part_v][i_u].key += weight;
        for (auto i_a : {i_w, i_v}) {
            this->modify_gain(i_a, part_v, -weight);
            this->vertex_list[part_u][i_a].key += weight;
        }
    } else if (part_w == part_u) {
        this->vertex_list[part_w][i_v].key += weight;
        for (auto i_a : {i_w, i_u}) {
            this->modify_gain(i_a, part_w, -weight);
            this->vertex_list[part_v][i_a].key += weight;
        }
    } else {
        this->totalcost += weight;
        this->vertex_list[part_v][i_u].key += weight;
        this->vertex_list[part_w][i_u].key += weight;
        this->vertex_list[part_w][i_v].key += weight;
        this->vertex_list[part_u][i_v].key += weight;
        this->vertex_list[part_u][i_w].key += weight;
        this->vertex_list[part_v][i_w].key += weight;
    }
    this->totalcost += weight;
}

/**
 * @brief
 *
 * @param net
 * @param part
 */
auto FMKWayGainCalc::init_gain_general_net(
    node_t net, const std::vector<std::uint8_t> &part) -> void {
    std::vector<uint8_t> num(this->K, 0);
    auto IdVec = std::vector<index_t>{};
    for (auto const &w : this->H.G[net]) {
        auto i_w = this->H.module_map[w];
        num[part[i_w]] += 1;
        IdVec.push_back(i_w);
    }
    auto weight = this->H.get_net_weight(net);
    for (auto k = 0U; k < this->K; ++k) {
        if (num[k] > 0) {
            this->totalcost += weight;
        }
    }
    this->totalcost -= weight;

    for (auto k = 0U; k < this->K; ++k) {
        if (num[k] == 0) {
            for (auto i_w : IdVec) {
                vertex_list[k][i_w].key -= weight;
            }
        } else if (num[k] == 1) {
            for (auto i_w : IdVec) {
                if (part[i_w] == k) {
                    this->modify_gain(i_w, part[i_w], weight);
                    break;
                }
            }
        }
    }
}

/**
 * @brief
 *
 * @param part_info
 * @param move_info
 * @return ret_2pin_info
 */
auto FMKWayGainCalc::update_move_2pin_net(const PartInfo &part_info,
                                          const MoveInfo &move_info)
    -> ret_2pin_info {
    auto const &[net, fromPart, toPart, v] = move_info;
    auto const &[part, extern_nets] = part_info;
    auto netCur = this->H.G[net].begin();
    node_t w = (*netCur != v) ? *netCur : *++netCur;
    auto i_w = this->H.module_map[w];
    auto weight = this->H.get_net_weight(net);
    auto deltaGainW = std::vector(this->K, 0);

    for (auto l : {fromPart, toPart}) {
        if (part[i_w] == l) {
            for (auto k = 0U; k < this->K; ++k) {
                deltaGainW[k] += weight;
                this->deltaGainV[k] += weight;
            }
        }
        deltaGainW[l] -= weight;
        weight = -weight;
    }
    return std::tuple{i_w, std::move(deltaGainW)};
}

/**
 * @brief
 *
 * @param part_info
 * @param move_info
 * @return ret_info
 */
auto FMKWayGainCalc::update_move_3pin_net(const PartInfo &part_info,
                                          const MoveInfo &move_info)
    -> ret_info {
    auto const &[net, fromPart, toPart, v] = move_info;
    auto const &[part, extern_nets] = part_info;
    auto IdVec = std::vector<index_t>{};
    for (auto const &w : this->H.G[net]) {
        if (w == v) {
            continue;
        }
        auto i_w = this->H.module_map[w];
        IdVec.push_back(i_w);
    }
    auto degree = std::size(IdVec);
    auto deltaGain = std::vector(degree, std::vector(this->K, 0));
    auto weight = this->H.get_net_weight(net);
    auto part_w = part[IdVec[0]];
    auto part_u = part[IdVec[1]];
    auto l = fromPart, u = toPart;

    if (part_w == part_u) {
        for (auto &&i : {0, 1}) {
            if (part_w != l) {
                deltaGain[0][l] -= weight;
                deltaGain[1][l] -= weight;
                if (part_w == u) {
                    for (auto k = 0U; k < this->K; ++k) {
                        this->deltaGainV[k] -= weight;
                    }
                }
            }
            weight = -weight;
            std::swap(l, u);
        }
        return std::tuple{std::move(IdVec), std::move(deltaGain)};
    }

    for (auto &&i : {0, 1}) {
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
    // return this->update_move_general_net(part_info, move_info);
}

/**
 * @brief
 *
 * @param part_info
 * @param move_info
 * @return ret_info
 */
auto FMKWayGainCalc::update_move_general_net(const PartInfo &part_info,
                                             const MoveInfo &move_info)
    -> ret_info {
    auto const &[net, fromPart, toPart, v] = move_info;
    auto const &[part, extern_nets] = part_info;
    std::vector<uint8_t> num(this->K, 0);
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
    auto deltaGain = std::vector(degree, std::vector(this->K, 0));
    auto weight = this->H.get_net_weight(net);

    auto l = fromPart, u = toPart;
    for (auto &&i : {0, 1}) {
        if (num[l] == 0) {
            for (auto idx = 0U; idx < degree; ++idx) {
                deltaGain[idx][l] -= weight;
            }
            if (num[u] > 0) {
                for (auto k = 0U; k < this->K; ++k) {
                    this->deltaGainV[k] -= weight;
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
        std::swap(l, u);
    }
    return std::tuple{std::move(IdVec), std::move(deltaGain)};
}
