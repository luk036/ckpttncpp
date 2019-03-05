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
    if (degree == 2) {
        this->init_gain_2pin_net(net, part);
    } else {
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
    if (part_w != part_v) {
        this->totalcost += weight;
        this->vertex_list[part_v][i_w].key += weight;
        this->vertex_list[part_w][i_v].key += weight;
    } else {
        this->modify_gain(i_w, part_w, -weight);
        this->modify_gain(i_v, part_v, -weight);
    }
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
    auto IdVec = std::vector<size_t>{};
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
    if (part[i_w] == fromPart) {
        for (auto k = 0U; k < this->K; ++k) {
            deltaGainW[k] += weight;
            this->deltaGainV[k] += weight;
        }
    } else if (part[i_w] == toPart) {
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
auto FMKWayGainCalc::update_move_general_net(const PartInfo &part_info,
                                             const MoveInfo &move_info)
    -> ret_info {
    auto const &[net, fromPart, toPart, v] = move_info;
    auto const &[part, extern_nets] = part_info;
    std::vector<uint8_t> num(this->K, 0);
    auto IdVec = std::vector<size_t>{};
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
    if (num[fromPart] == 0) {
        if (num[toPart] > 0) {
            for (auto k = 0U; k < this->K; ++k) {
                this->deltaGainV[k] -= weight;
            }
        }
    } else { // num[fromPart] > 0
        if (num[toPart] == 0) {
            for (auto k = 0U; k < this->K; ++k) {
                this->deltaGainV[k] += weight;
            }
        }
    }
    for (auto &&l : {fromPart, toPart}) {
        if (num[l] == 0) {
            for (auto idx = 0U; idx < degree; ++idx) {
                deltaGain[idx][l] -= weight;
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
