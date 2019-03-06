#include <ckpttncpp/FMBiGainCalc.hpp>
/* linux-2.6.38.8/include/linux/compiler.h */
#include <cstdio>
#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

/**
 * @brief
 *
 * @param net
 * @param part
 */
void FMBiGainCalc::init_gain( //
    node_t net, const PartInfo &part_info) {
    auto degree = this->H.G.degree(net);
    if (unlikely(degree < 2)) {
        return; // does not provide any gain when moving
    }
    auto &[part, extern_nets] = part_info;
    if (degree == 3) {
        this->init_gain_3pin_net(net, part);
    } else if (degree == 2) {
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
void FMBiGainCalc::init_gain_2pin_net( //
    node_t net, const std::vector<std::uint8_t> &part) {
    auto netCur = this->H.G[net].begin();
    auto w = *netCur;
    auto v = *++netCur;
    auto i_w = this->H.module_map[w];
    auto i_v = this->H.module_map[v];

    auto weight = this->H.get_net_weight(net);
    if (part[i_w] != part[i_v]) {
        this->totalcost += weight;
    } else {
        weight = -weight;
    }
    this->modify_gain(i_w, weight);
    this->modify_gain(i_v, weight);
}

/**
 * @brief
 *
 * @param net
 * @param part
 */
void FMBiGainCalc::init_gain_3pin_net(
    node_t net, const std::vector<std::uint8_t> &part) {
    auto netCur = this->H.G[net].begin();
    auto w = *netCur;
    auto v = *++netCur;
    auto u = *++netCur;
    auto i_w = this->H.module_map[w];
    auto i_v = this->H.module_map[v];
    auto i_u = this->H.module_map[u];

    auto weight = this->H.get_net_weight(net);
    if (part[i_u] == part[i_v]) {
        if (part[i_w] == part[i_v]) {
            for (auto &&a : {u, v, w}) {
                this->modify_gain(a, -weight);
            }
            return;
        }
        this->modify_gain(i_w, weight);
    } else if (part[i_w] == part[i_v]) {
        this->modify_gain(u, weight);
    } else {
        this->modify_gain(i_v, weight);
    }
    this->totalcost += weight;
}

/**
 * @brief
 *
 * @param net
 * @param part
 */
void FMBiGainCalc::init_gain_general_net(
    node_t net, const std::vector<std::uint8_t> &part) {
    uint8_t num[2] = {0, 0};
    auto IdVec = std::vector<index_t>{};
    for (auto const &w : this->H.G[net]) {
        auto i_w = this->H.module_map[w];
        num[part[i_w]] += 1;
        IdVec.push_back(i_w);
    }
    auto weight = this->H.get_net_weight(net);

    if (num[0] > 0 && num[1] > 0) {
        this->totalcost += weight;
    }

    for (auto &&k : {0, 1}) {
        if (num[k] == 0) {
            for (auto i_w : IdVec) {
                this->modify_gain(i_w, -weight);
            }
        } else if (num[k] == 1) {
            for (auto i_w : IdVec) {
                if (part[i_w] == k) {
                    this->modify_gain(i_w, weight);
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
auto FMBiGainCalc::update_move_2pin_net(const PartInfo &part_info,
                                        const MoveInfo &move_info)
    -> ret_2pin_info {
    auto const &[net, fromPart, toPart, v] = move_info;
    auto const &[part, extern_nets] = part_info;
    auto netCur = this->H.G[net].begin();
    auto w = (*netCur != v) ? *netCur : *++netCur;
    auto i_w = this->H.module_map[w];
    auto weight = this->H.get_net_weight(net);
    auto delta = (part[i_w] == fromPart) ? weight : -weight;
    return std::tuple{i_w, 2 * delta};
}

/**
 * @brief
 *
 * @param part
 * @param move_info
 * @return ret_info
 */
auto FMBiGainCalc::update_move_3pin_net(const PartInfo &part_info,
                                        const MoveInfo &move_info)
    -> ret_info {
    auto const &[net, fromPart, toPart, v] = move_info;
    auto const &[part, extern_nets] = part_info;
    uint8_t num[2] = {0, 0};
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
    auto deltaGain = std::vector{0, 0};
    auto weight = this->H.get_net_weight(net);

    auto part_w = part[IdVec[0]];

    if (part_w != fromPart) {
        weight = -weight;
    }
    if (part_w == part[IdVec[1]]) {
        for (auto &&idx : {0, 1}) {
            deltaGain[idx] += weight;
        }
    } else {
        deltaGain[0] += weight;
        deltaGain[1] -= weight;
    }
    return std::tuple{std::move(IdVec), std::move(deltaGain)};
}

/**
 * @brief
 *
 * @param part
 * @param move_info
 * @return ret_info
 */
auto FMBiGainCalc::update_move_general_net(const PartInfo &part_info,
                                           const MoveInfo &move_info)
    -> ret_info {
    auto const &[net, fromPart, toPart, v] = move_info;
    auto const &[part, extern_nets] = part_info;
    uint8_t num[2] = {0, 0};
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
    for (auto &&l : {fromPart, toPart}) {
        if (num[l] == 0) {
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
    }
    return std::tuple{std::move(IdVec), std::move(deltaGain)};
}
