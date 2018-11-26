#ifndef _HOME_UBUNTU_GITHUB_CKPTTNCPP_FMBIGAINCALC_HPP
#define _HOME_UBUNTU_GITHUB_CKPTTNCPP_FMBIGAINCALC_HPP 1

// #include "bpqueue.hpp" // import bpqueue
#include "dllist.hpp"  // import dllink
#include "netlist.hpp" // import Netlist
#include <cassert>
#include <iterator>
#include <tuple>

/* linux-2.6.38.8/include/linux/compiler.h */
#include <stdio.h>
#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

/**
 * @brief FMBiGainCalc
 *
 */
struct FMBiGainCalc {
    Netlist &H;

    /**
     * @brief Construct a new FMBiGainCalc object
     *
     * @param H
     */
    explicit FMBiGainCalc(Netlist &H) : H{H} {}

    /**
     * @brief
     *
     * @param part
     */
    auto init(std::vector<size_t> &part, std::vector<dllink> &vertex_list)
        -> void {
        for (auto &net : this->H.net_list) {
            this->init_gain(net, part, vertex_list);
        }
    }

    /**
     * @brief
     *
     * @param net
     * @param part
     */
    auto init_gain(node_t &net, std::vector<size_t> &part,
                   std::vector<dllink> &vertex_list) -> void {
        if (this->H.G.degree(net) == 2) {
            this->init_gain_2pin_net(net, part, vertex_list);
        } else if (unlikely(this->H.G.degree(net) < 2)) {
            return; // does not provide any gain when move
        } else {
            this->init_gain_general_net(net, part, vertex_list);
        }
    }

    /**
     * @brief
     *
     * @param net
     * @param part
     */
    auto init_gain_2pin_net(node_t &net, std::vector<size_t> &part,
                            std::vector<dllink> &vertex_list) -> void {
        assert(this->H.G.degree(net) == 2);
        auto netCur = this->H.G[net].begin();
        auto w = *netCur;
        auto v = *++netCur;
        // auto i_w = this->H.module_dict[w];
        // auto i_v = this->H.module_dict[v];
        auto part_w = part[w];
        auto part_v = part[v];
        // auto weight = this->H.G[net].get('weight', 1);
        auto weight = this->H.get_net_weight(net);
        auto g = (part_w == part_v) ? -weight : weight;
        vertex_list[w].key += g;
        vertex_list[v].key += g;
    }

    /**
     * @brief
     *
     * @param net
     * @param part
     */
    auto init_gain_general_net(node_t &net, std::vector<size_t> &part,
                               std::vector<dllink> &vertex_list) -> void {
        size_t num[2] = {0, 0};
        auto IdVec = std::vector<size_t>();
        for (const auto &w : this->H.G[net]) {
            num[part[w]] += 1;
            IdVec.push_back(w);
        }

        auto weight = this->H.get_net_weight(net);
        for (auto &&k : {0, 1}) {
            if (num[k] == 0) {
                for (auto &w : IdVec) {
                    vertex_list[w].key -= weight;
                }
            } else if (num[k] == 1) {
                for (auto &w : IdVec) {
                    if (part[w] == k) {
                        vertex_list[w].key += weight;
                        break;
                    }
                }
            }
        }
    }

    /**
     * @brief
     *
     * @param net
     * @param part
     * @param fromPart
     * @param v
     */
    auto update_move_2pin_net(std::vector<size_t> &part,
                              const MoveInfo& move_info) {
        const auto& [net, fromPart, toPart, v] = move_info;
        assert(this->H.G.degree(net) == 2);
        auto netCur = this->H.G[net].begin();
        node_t w = (*netCur != v) ? *netCur : *++netCur;
        auto part_w = part[w];
        // auto weight = this->H.G[net].get('weight', 1);
        auto weight = this->H.get_net_weight(net);
        auto deltaGainW = (part_w == fromPart) ? 2 * weight : -2 * weight;
        return std::tuple{w, deltaGainW};
    }

    /**
     * @brief
     *
     * @param net
     * @param part
     * @param fromPart
     * @param v
     */
    auto update_move_general_net(std::vector<size_t> &part,
                                 const MoveInfo& move_info) {
        const auto& [net, fromPart, toPart, v] = move_info;
        assert(this->H.G.degree(net) > 2);

        size_t num[2] = {0, 0};
        auto IdVec = std::vector<size_t>{};
        auto deltaGain = std::vector<int>{};
        for (const auto &w : this->H.G[net]) {
            if (w == v)
                continue;
            num[part[w]] += 1;
            IdVec.push_back(w);
            deltaGain.push_back(0);
        }
        auto degree = std::size(IdVec);
        // auto m = this->H.G[net].get('weight', 1);
        auto weight = this->H.get_net_weight(net);
        // auto weight = (fromPart == 0) ? m : -m;
        for (auto &&l : {fromPart, toPart}) {
            if (num[l] == 0) {
                for (auto idx = 0u; idx < degree; ++idx) {
                    deltaGain[idx] -= weight;
                }
            } else if (num[l] == 1) {
                for (auto idx = 0u; idx < degree; ++idx) {
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
};

#endif
