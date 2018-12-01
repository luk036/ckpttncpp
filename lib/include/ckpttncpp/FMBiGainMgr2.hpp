#ifndef _HOME_UBUNTU_GITHUB_CKPTTNCPP_FMBIGAINMGR_HPP
#define _HOME_UBUNTU_GITHUB_CKPTTNCPP_FMBIGAINMGR_HPP 1

#include "bpqueue.hpp" // import bpqueue
#include "dllist.hpp"  // import dllink
#include "netlist.hpp" // import Netlist
#include <cassert>
#include <iterator>

#include "FMBiGainCalc.hpp"

/**
 * @brief FMBiGainMgr
 *
 */
struct FMBiGainMgr {
    Netlist &H;
    FMBiGainCalc gainCalc;
    size_t pmax;
    size_t num_modules;
    dllink waitinglist;
    std::vector<dllink> vertex_list;
    std::vector<std::unique_ptr<bpqueue>> gainbucket;

    /**
     * @brief Construct a new FMBiGainMgr object
     *
     * @param H
     */
    explicit FMBiGainMgr(Netlist &H)
        : H{H},
          gainCalc{H},
          pmax{H.get_max_degree()},
          num_modules{H.number_of_modules()},
          waitinglist{},
          vertex_list(num_modules)
    {
        for (auto&& k : {0, 1}) {
            this->gainbucket.push_back(
                std::make_unique<bpqueue>(-this->pmax, this->pmax));
        }
    }

    /**
     * @brief
     *
     * @return true
     * @return false
     */
    auto is_empty_togo(size_t toPart) const -> bool {
        return this->gainbucket[toPart]->is_empty(); 
    }

    /**
     * @brief
     *
     * @return true
     * @return false
     */
    auto is_empty() const -> bool {
        for (auto&& k : {0, 1}) {
            if (this->gainbucket[k]->is_empty()) {
                return false;
            }
        }
        return true;
    }

    /**
     * @brief 
     * 
     * @param part 
     * @return std::tuple<MoveInfoV, int> 
     */
    auto select(const std::vector<size_t>& part)
                        -> std::tuple<MoveInfoV, int> {
        auto gainmax = std::vector<int>(2);
        for (auto&& k : {0, 1}) {
            gainmax[k] = this->gainbucket[k]->get_max();
        }
        size_t toPart = (gainmax[0] > gainmax[1])? 0 : 1;
        auto &vlink = this->gainbucket[toPart]->popleft();
        this->waitinglist.append(vlink);
        size_t v = &vlink - &this->vertex_list[0];
        auto fromPart = part[v];
        auto move_info_v = MoveInfoV{fromPart, toPart, v};
        return std::tuple{std::move(move_info_v), gainmax[toPart]};
    }

    auto select_togo(size_t toPart)
                        -> std::tuple<size_t, int> {
        auto gainmax = this->gainbucket[toPart]->get_max();
        auto &vlink = this->gainbucket[toPart]->popleft();
        this->waitinglist.append(vlink);
        size_t v = &vlink - &this->vertex_list[0];
        return std::tuple{v, gainmax};
    }

    /**
     * @brief
     *
     * @param part
     */
    auto init(const std::vector<size_t> &part) -> void;

    auto modify_key(const std::vector<size_t> &part,
            size_t w, int key) -> void {
        auto part_w = part[w];
        this->gainbucket[1-part_w]->modify_key(
                this->vertex_list[w], key);
    }
                    
    /**
     * @brief 
     * 
     * @param part 
     * @param move_info_v 
     * @param gain 
     */
    auto update_move(const std::vector<size_t> &part,
                     const MoveInfoV& move_info_v,
                     int gain) -> void;

    /**
     * @brief
     *
     * @param net
     * @param part
     * @param fromPart
     * @param v
     */
    auto update_move_2pin_net(const std::vector<size_t> &part,
                              const MoveInfo& move_info) -> void;

    /**
     * @brief
     *
     * @param net
     * @param part
     * @param fromPart
     * @param v
     */
    auto update_move_general_net(const std::vector<size_t> &part,
                                 const MoveInfo& move_info) -> void;
};

#endif
