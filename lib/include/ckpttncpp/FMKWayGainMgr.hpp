#ifndef _HOME_UBUNTU_GITHUB_CKPTTNCPP_FMKWayGAINMGR_HPP
#define _HOME_UBUNTU_GITHUB_CKPTTNCPP_FMKWayGAINMGR_HPP 1

#include "FMKWayGainCalc.hpp"
#include "bpqueue.hpp" // import bpqueue
#include "dllist.hpp"  // import dllink
#include "netlist.hpp" // import Netlist
#include <cassert>
#include <iterator>

/**
 * @brief FMKWayGainMgr
 *
 */
struct FMKWayGainMgr {
    Netlist &H;
    size_t K;
    FMKWayGainCalc gainCalc;
    size_t pmax;
    std::vector<std::unique_ptr<bpqueue>> gainbucket;
    // size_t num[2];
    size_t num_modules;
    std::vector<std::vector<dllink>> vertex_list;
    dllink waitinglist;
    std::vector<int> deltaGainV;

    /**
     * @brief Construct a new FMKWayGainMgr object
     *
     * @param H
     */
    explicit FMKWayGainMgr(Netlist &H, size_t K)
        : H{H}, K{K}, gainCalc{H, K}, pmax{H.get_max_degree()},
          // num{0, 0},
          num_modules{H.number_of_modules()}, waitinglist{}, deltaGainV(K, 0) {
        for (auto k = 0u; k < this->K; ++k) {
            this->gainbucket.push_back(
                std::make_unique<bpqueue>(-this->pmax, this->pmax));
            this->vertex_list.emplace_back(
                std::vector<dllink>(this->num_modules));
        }
    }

    /**
     * @brief
     *
     * @param part
     */
    auto init(std::vector<size_t> &part) -> void {
        this->gainCalc.init(part, this->vertex_list);

        for (auto v : this->H.module_fixed) {
            for (auto k = 0u; k < this->K; ++k) {
                this->vertex_list[k][v].key = -this->pmax;
            }
        }
        for (auto v : this->H.module_list) {
            for (auto k = 0u; k < this->K; ++k) {
                auto &vlink = this->vertex_list[k][v];
                if (part[v] == k) {
                    assert(vlink.key == 0);
                    this->gainbucket[k]->set_key(vlink, 0);
                    this->waitinglist.append(vlink);
                } else {
                    this->gainbucket[k]->append(vlink, vlink.key);
                }
            }
        }
    }

    /**
     * @brief
     *
     * @return true
     * @return false
     */
    auto is_empty(size_t toPart) const -> bool {
        return this->gainbucket[toPart]->is_empty();
    }

    /**
     * @brief
     *
     * @return std::tuple<size_t, int>
     */
    auto select_togo(size_t toPart) -> std::tuple<size_t, int> {
        auto gainmax = this->gainbucket[toPart]->get_max();
        auto &vlink = this->gainbucket[toPart]->popleft();
        this->waitinglist.append(vlink);
        return {&vlink - &this->vertex_list[toPart][0], gainmax};
    }

    /**
     * @brief Set the key object
     *
     * @param whichPart
     * @param v
     * @param key
     */
    auto set_key(size_t whichPart, size_t v, int key) -> void {
        this->gainbucket[whichPart]->set_key(
            this->vertex_list[whichPart][v], key);
    }

    /**
     * @brief
     *
     * @param part
     * @param v
     */
    auto update_move(std::vector<size_t> &part,
                     const MoveInfoV& move_info_v, int gain) -> void {
        auto [fromPart, toPart, v] = move_info_v;
	    std::fill_n(this->deltaGainV.begin(), this->K, 0);
        for (auto net : this->H.G[v]) {
            auto move_info = MoveInfo{net, fromPart, toPart, v};
            if (this->H.G.degree(net) == 2) {
                this->update_move_2pin_net(part, move_info);
            } else if (unlikely(this->H.G.degree(net) < 2)) {
                break; // does not provide any gain change when move
            } else {
                this->update_move_general_net(part, move_info);
            }
        }
        for (auto k = 0u; k < this->K; ++k) {
            if (fromPart == k || toPart == k) {
                continue;
            }
            this->gainbucket[k]->modify_key(this->vertex_list[k][v],
                                            this->deltaGainV[k]);
        }
        this->set_key(fromPart, v, -gain);
        this->set_key(toPart, v, 0); // actually don't care
    }

    auto modify_key(std::vector<size_t> &part,
                    size_t w, std::vector<int> &keys) -> void {
        for (auto k = 0u; k < this->K; ++k) {
            if (part[w] == k) {
                continue;
            }
            this->gainbucket[k]->modify_key(this->vertex_list[k][w], keys[k]);
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
                              const MoveInfo& move_info) -> void {
        auto [w, deltaGainW, deltaGainV] =
            this->gainCalc.update_move_2pin_net(part, move_info);
        this->modify_key(part, w, deltaGainW);
        for (auto k = 0u; k < this->K; ++k) {
            this->deltaGainV[k] += deltaGainV[k];
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
    auto update_move_general_net(std::vector<size_t> &part,
                                 const MoveInfo& move_info) -> void {
        auto [IdVec, deltaGain, deltaGainV] = 
	        this->gainCalc.update_move_general_net(part, move_info);
        auto degree = std::size(IdVec);
        for (auto idx = 0u; idx < degree; ++idx) {
            this->modify_key(part, IdVec[idx], deltaGain[idx]);
        }
        for (auto k = 0u; k < this->K; ++k) {
            this->deltaGainV[k] += deltaGainV[k];
        }
    }
};

#endif
