#ifndef _HOME_UBUNTU_GITHUB_CKPTTNCPP_FMBIGAINMGR_HPP
#define _HOME_UBUNTU_GITHUB_CKPTTNCPP_FMBIGAINMGR_HPP 1

#include "bpqueue.hpp" // import bpqueue
#include "dllist.hpp"  // import dllink
#include "netlist.hpp" // import Netlist
#include <cassert>
#include <iterator>

/**
 * @brief FMBiGainMgr
 *
 */
struct FMBiGainMgr {
    Netlist &H;
    // FMBiGainCalc gainCalc;
    size_t pmax;
    bpqueue gainbucket;
    size_t num_modules;
    std::vector<dllink> vertex_list;
    dllink waitinglist;

    /**
     * @brief Construct a new FMBiGainMgr object
     *
     * @param H
     */
    explicit FMBiGainMgr(Netlist &H)
        : H{H},
          // gainCalc{H},
          pmax{H.get_max_degree()}, gainbucket(-pmax, pmax),
          num_modules{H.number_of_modules()},
          vertex_list(num_modules), waitinglist{} {}

    /**
     * @brief
     *
     * @return true
     * @return false
     */
    auto is_empty() const -> bool { return this->gainbucket.is_empty(); }

    /**
     * @brief
     *
     * @return std::tuple<size_t, int>
     */
    auto select() -> std::tuple<size_t, int> {
        auto gainmax = this->gainbucket.get_max();
        auto &vlink = this->gainbucket.popleft();
        this->waitinglist.append(vlink);
        return {&vlink - &this->vertex_list[0], gainmax};
    }

    /**
     * @brief
     *
     * @param part
     */
    auto init(std::vector<size_t> &part) -> void;

    /**
     * @brief 
     * 
     * @param part 
     * @param move_info_v 
     * @param gain 
     */
    auto update_move(std::vector<size_t> &part,
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
    auto update_move_2pin_net(std::vector<size_t> &part,
                              const MoveInfo& move_info) -> void;

    /**
     * @brief
     *
     * @param net
     * @param part
     * @param fromPart
     * @param v
     */
    auto update_move_general_net(std::vector<size_t> &part,
                                 const MoveInfo& move_info) -> void;
};

#endif
