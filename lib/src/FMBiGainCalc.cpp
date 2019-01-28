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
        return; // does not provide any gain when move
    }
    auto &[part, extern_nets] = part_info;
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
void FMBiGainCalc::init_gain_2pin_net( //
    node_t net, const std::vector<std::uint8_t> &part) {
    auto netCur = this->H.G[net].begin();
    auto w = *netCur;
    auto v = *++netCur;
    // auto w = this->H.module_map[w];
    // auto v = this->H.module_map[v];
    auto part_w = part[w];
    auto part_v = part[v];
    auto weight = this->H.get_net_weight(net);
    if (part_w != part_v) {
        this->totalcost += weight;
    }
    auto g = (part_w == part_v) ? -weight : weight;
    this->modify_gain(w, g);
    this->modify_gain(v, g);
}

/**
 * @brief
 *
 * @param net
 * @param part
 */
void FMBiGainCalc::init_gain_general_net(
    node_t net, const std::vector<std::uint8_t> &part) {
    size_t num[2] = {0, 0};
    auto IdVec = std::vector<size_t>{};
    for (auto const &w : this->H.G[net]) {
        // auto w = this->H.module_map[w];
        num[part[w]] += 1;
        IdVec.push_back(w);
    }
    auto weight = this->H.get_net_weight(net);

    if (num[0] > 0 && num[1] > 0) {
        this->totalcost += weight;
    }

    for (auto &&k : {0, 1}) {
        if (num[k] == 0) {
            for (auto w : IdVec) {
                this->modify_gain(w, -weight);
            }
        } else if (num[k] == 1) {
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
auto FMBiGainCalc::update_move_2pin_net(const PartInfo &part_info,
                                        const MoveInfo &move_info)
    -> ret_2pin_info {
    auto const &[net, fromPart, toPart, v] = move_info;
    auto const &[part, extern_nets] = part_info;
    auto netCur = this->H.G[net].begin();
    node_t w = (*netCur != v) ? *netCur : *++netCur;
    // auto w = this->H.module_map[w];
    auto part_w = part[w];
    auto weight = this->H.get_net_weight(net);
    auto deltaGainW = (part_w == fromPart) ? 2 * weight : -2 * weight;
    return std::tuple{w, deltaGainW};
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
