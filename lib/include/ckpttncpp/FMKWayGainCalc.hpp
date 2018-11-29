#ifndef _HOME_UBUNTU_GITHUB_CKPTTNCPP_FMKWayGAINCALC_HPP
#define _HOME_UBUNTU_GITHUB_CKPTTNCPP_FMKWayGAINCALC_HPP 1

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
 * @brief FMKWayGainCalc
 *
 */
struct FMKWayGainCalc {
    Netlist &H;
    size_t K;
    /**
     * @brief Construct a new FMKWayGainCalc object
     *
     * @param H
     */
    explicit FMKWayGainCalc(Netlist &H, size_t K) : H{H}, K{K} {}

    /**
     * @brief
     *
     * @param part
     */
    auto init(std::vector<size_t> &part,
              std::vector<std::vector<dllink>> &vertex_list) -> void {
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
                   std::vector<std::vector<dllink>> &vertex_list) -> void {
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
                            std::vector<std::vector<dllink>> &vertex_list)
        -> void {
        assert(this->H.G.degree(net) == 2);
        auto netCur = this->H.G[net].begin();
        auto w = *netCur;
        auto v = *++netCur;
        auto part_w = part[w];
        auto part_v = part[v];
        auto weight = this->H.get_net_weight(net);
        if (part_v == part_w) {
            for (auto k = 0u; k < this->K; ++k) {
                vertex_list[k][w].key -= weight;
                vertex_list[k][v].key -= weight;
            }
        }
        vertex_list[part_v][w].key += weight;
        vertex_list[part_w][v].key += weight;
    }

    /**
     * @brief
     *
     * @param net
     * @param part
     * @param vertex_list
     */
    auto init_gain_general_net(node_t &net, std::vector<size_t> &part,
                               std::vector<std::vector<dllink>> &vertex_list)
        -> void {
        std::vector<size_t> num(this->K, 0);
        auto IdVec = std::vector<size_t>();
        for (const auto &w : this->H.G[net]) {
            num[part[w]] += 1;
            IdVec.push_back(w);
        }

        auto weight = this->H.get_net_weight(net);

        for (auto k = 0u; k < this->K; ++k) {
            if (num[k] == 0) {
                for (auto &w : IdVec) {
                    vertex_list[k][w].key -= weight;
                }
            } else if (num[k] == 1) {
                for (auto &w : IdVec) {
                    if (part[w] == k) {
                        for (auto k2 = 0u; k2 < this->K; ++k) {
                            vertex_list[k2][w].key += weight;
                        }
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
        auto weight = this->H.get_net_weight(net);
        auto deltaGainW = std::vector<int>(this->K, 0);
        auto deltaGainV = std::vector<int>(this->K, 0);
        if (part_w == fromPart) {
            for (auto k = 0u; k < this->K; ++k) {
                deltaGainW[k] += weight;
                deltaGainV[k] += weight;
            }
        } else if (part_w == toPart) {
            for (auto k = 0u; k < this->K; ++k) {
                deltaGainW[k] -= weight;
                deltaGainV[k] -= weight;
            }
        }
        deltaGainW[fromPart] -= weight;
        deltaGainW[toPart] += weight;
        return std::tuple{w, std::move(deltaGainW), std::move(deltaGainV)};
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
        std::vector<size_t> num(this->K, 0);
        auto IdVec = std::vector<size_t>{};
        for (const auto &w : this->H.G[net]) {
            if (w == v)
                continue;
            num[part[w]] += 1;
            IdVec.push_back(w);
        }
        auto degree = std::size(IdVec);
        auto deltaGain = std::vector<std::vector<int>>(degree,
                                     std::vector<int>(this->K, 0));
        auto deltaGainV = std::vector<int>(this->K, 0);

        // auto m = this->H.G[net].get('weight', 1);
        auto weight = this->H.get_net_weight(net);
	    if (num[fromPart] == 0) {
	        if (num[toPart] > 0) {
                for (auto idx = 0u; idx < degree; ++idx) {
                    deltaGain[idx][fromPart] -= weight;
                }
	            for (auto k = 0u; k < this->K; ++k) {
		            deltaGainV[k] -= weight;
		        }
            }
        } else { // num[fromPart] > 0
	        if (num[toPart] == 0) {
                for (auto idx = 0u; idx < degree; ++idx) {
                    deltaGain[idx][toPart] += weight;
                }
	            for (auto k = 0u; k < this->K; ++k) {
		            deltaGainV[k] += weight;
		        }
	        }
	    }
        for (auto &&l : {fromPart, toPart}) {
            if (num[l] == 1) {
                for (auto idx = 0u; idx < degree; ++idx) {
                    if (part[IdVec[idx]] == l) {
                        for (auto k = 0u; k < this->K; ++k) {
                            deltaGain[idx][k] += weight;
                        }
                        break;
                    }
                }
            }
            weight = -weight;
        }
        return std::tuple{std::move(IdVec), std::move(deltaGain), std::move(deltaGainV)};
    }
};

#endif
