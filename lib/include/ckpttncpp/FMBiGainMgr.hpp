#ifndef _HOME_UBUNTU_GITHUB_CKPTTNCPP_FMBIGAINMGR_HPP
#define _HOME_UBUNTU_GITHUB_CKPTTNCPP_FMBIGAINMGR_HPP 1

#include "bpqueue.hpp" // import bpqueue
#include "dllist.hpp"  // import dllink
#include "netlist.hpp" // import Netlist
#include <cassert>
#include <iterator>

/* linux-2.6.38.8/include/linux/compiler.h */
#include <stdio.h>
#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

/**
 * @brief FMBiGainMgr
 *
 */
struct FMBiGainMgr {
    Netlist &H;
    size_t pmax;
    bpqueue gainbucket;
    // size_t num[2];
    size_t num_cells;
    std::vector<dllink> vertex_list;
    dllink waitinglist;

    /**
     * @brief Construct a new FMBiGainMgr object
     *
     * @param H
     */
    explicit FMBiGainMgr(Netlist &H)
        : H{H}, pmax{H.get_max_degree()}, gainbucket(-pmax, pmax),
          // num{0, 0},
          num_cells{H.number_of_cells()},
          vertex_list(num_cells), waitinglist{} {}

    /**
     * @brief
     *
     * @return size_t
     */
    auto popleft() -> std::tuple<size_t, int> {
        auto gainmax = this->gainbucket.get_max();
        auto &vlink =
            this->gainbucket.popleft();
        this->waitinglist.append(vlink);
        return {&vlink - &this->vertex_list[0], gainmax};
    }

    /**
     * @brief Get the vertex id object
     *
     * @param vlink
     * @return size_t
     */
    auto get_vertex_id(const dllink &vlink) const -> size_t {
        return &vlink - &vertex_list[0];
    }

    /**
     * @brief
     *
     * @param part
     */
    auto init(std::vector<size_t> &part) -> void {
        for (auto &net : this->H.net_list) {
            this->init_gain(net, part);
        }

        for (auto &v : this->H.cell_fixed) {
            // auto i_v = this->H.cell_dict[v];
            // force to the lowest gain
            this->vertex_list[v].key = -this->pmax;
        }

        this->gainbucket.appendfrom(this->vertex_list);
    }

    /**
     * @brief
     *
     * @param net
     * @param part
     */
    auto init_gain(node_t &net, std::vector<size_t> &part) -> void {
        if (this->H.G.degree(net) == 2) {
            this->init_gain_2pin_net(net, part);
        } else if (unlikely(this->H.G.degree(net) <
                            2)) { // unlikely, self-loop, etc.
            return;               // does not provide any gain when move
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
    auto init_gain_2pin_net(node_t &net, std::vector<size_t> &part) -> void {
        assert(this->H.G.degree(net) == 2);
        auto netCur = this->H.G[net].begin();
        auto w = *netCur;
        auto v = *++netCur;
        // auto i_w = this->H.cell_dict[w];
        // auto i_v = this->H.cell_dict[v];
        auto part_w = part[w];
        auto part_v = part[v];
        // auto weight = this->H.G[net].get('weight', 1);
        auto weight = 1;
        auto g = (part_w == part_v) ? -weight : weight;
        this->vertex_list[w].key += g;
        this->vertex_list[v].key += g;
    }

    /**
     * @brief
     *
     * @param net
     * @param part
     */
    auto init_gain_general_net(node_t &net, std::vector<size_t> &part) -> void {
        size_t num[2] = {0, 0};
        auto IdVec = std::vector<size_t>();
        for (const auto &w : this->H.G[net]) {
            auto i_w = this->H.cell_dict[w];
            num[part[i_w]] += 1;
            IdVec.push_back(i_w);
        }

        // auto weight = this->H.G[net].get('weight', 1);
        auto weight = 1;
        for (auto &&k : {0, 1}) {
            if (num[k] == 0) {
                for (auto &i_w : IdVec) {
                    this->vertex_list[i_w].key -= weight;
                }
            } else if (num[k] == 1) {
                for (auto &i_w : IdVec) {
                    auto part_w = part[i_w];
                    if (part_w == k) {
                        this->vertex_list[i_w].key += weight;
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
     * @param v
     */
    auto update_move(std::vector<size_t> &part, size_t fromPart, node_t v,
                     int gain) -> void {
        // auto i_v = this->H.cell_dict[v];
        // auto fromPart = part[i_v];

        for (auto net : this->H.G[v]) {
            if (this->H.G.degree(net) == 2) {
                this->update_move_2pin_net(net, part, fromPart, v);
            } else if (unlikely(this->H.G.degree(net) <
                                2)) { // unlikely, self-loop, etc.
                break; // does not provide any gain change when move
            } else {
                this->update_move_general_net(net, part, fromPart, v);
            }
        }

        // auto gain = this->gainbucket.get_key(this->vertex_list[v]);
        // this->gainbucket.modify_key(this->vertex_list[v], -2 * gain);
        this->vertex_list[v].key -= 2 * gain;
    }

    /**
     * @brief
     *
     * @param net
     * @param part
     * @param fromPart
     * @param v
     */
    auto update_move_2pin_net(node_t &net, std::vector<size_t> &part,
                              size_t fromPart, node_t v) -> void {
        assert(this->H.G.degree(net) == 2);
        auto netCur = this->H.G[net].begin();
        node_t w = (*netCur != v) ? *netCur : *++netCur;
        auto i_w = this->H.cell_dict[w];
        auto part_w = part[i_w];
        // auto weight = this->H.G[net].get('weight', 1);
        auto weight = 1;
        auto deltaGainW = (part_w == fromPart) ? 2 * weight : -2 * weight;
        this->gainbucket.modify_key(this->vertex_list[i_w], deltaGainW);
    }

    /**
     * @brief
     *
     * @param net
     * @param part
     * @param fromPart
     * @param v
     */
    auto update_move_general_net(node_t &net, std::vector<size_t> &part,
                                 size_t fromPart, node_t v) -> void {
        assert(this->H.G.degree(net) > 2);

        size_t num[2] = {0, 0};
        auto IdVec = std::vector<size_t>{};
        auto deltaGain = std::vector<int>{};
        for (const auto &w : this->H.G[net]) {
            if (w == v)
                continue;
            auto i_w = this->H.cell_dict[w];
            num[part[i_w]] += 1;
            IdVec.push_back(i_w);
            deltaGain.push_back(0);
        }
        auto degree = std::size(IdVec);
        // auto m = this->H.G[net].get('weight', 1);
        auto m = 1;
        auto weight = (fromPart == 0) ? m : -m;
        for (auto &&k : {0, 1}) {
            if (num[k] == 0) {
                for (auto idx = 0u; idx < degree; ++idx) {
                    deltaGain[idx] -= weight;
                }
            } else if (num[k] == 1) {
                for (auto idx = 0u; idx < degree; ++idx) {
                    auto part_w = part[IdVec[idx]];
                    if (part_w == k) {
                        deltaGain[idx] += weight;
                        break;
                    }
                }
            }
            weight = -weight;
        }

        for (auto idx = 0u; idx < degree; ++idx) {
            if (deltaGain[idx] == 0)
                continue;
            this->gainbucket.modify_key(this->vertex_list[IdVec[idx]],
                                        deltaGain[idx]);
        }
    }
};

#endif
