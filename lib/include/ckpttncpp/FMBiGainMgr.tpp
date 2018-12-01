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
    std::vector<dllink> vertex_list;
    dllink waitinglist;
    bpqueue gainbucket;

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
          vertex_list(num_modules), waitinglist{},
          gainbucket(-pmax, pmax) {}

    /**
     * @brief
     *
     * @return true
     * @return false
     */
    auto is_empty_togo(std::uint8_t toPart) const -> bool {
        return this->gainbucket.is_empty(); 
    }

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
     * @param part 
     * @return std::tuple<MoveInfoV, int> 
     */
    auto select(const std::vector<std::uint8_t>& part)
                        -> std::tuple<MoveInfoV, int> {
        auto gainmax = this->gainbucket.get_max();
        auto &vlink = this->gainbucket.popleft();
        this->waitinglist.append(vlink);
        node_t v = &vlink - &this->vertex_list[0];
        auto fromPart = part[v];
        auto move_info_v = MoveInfoV{fromPart, 1-fromPart, v};
        return std::tuple{std::move(move_info_v), gainmax};
    }

    auto select_togo(std::uint8_t toPart)
                        -> std::tuple<node_t, int> {
        auto gainmax = this->gainbucket.get_max();
        auto &vlink = this->gainbucket.popleft();
        this->waitinglist.append(vlink);
        node_t v = &vlink - &this->vertex_list[0];
        return std::tuple{v, gainmax};
    }

    /**
     * @brief
     *
     * @param part
     */
    auto init(const std::vector<std::uint8_t> &part) -> void;

    /**
     * @brief 
     * 
     * @param part 
     * @param move_info_v 
     * @param gain 
     */
    auto update_move(const std::vector<std::uint8_t> &part,
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
    auto update_move_2pin_net(const std::vector<std::uint8_t> &part,
                              const MoveInfo& move_info) -> void;

    /**
     * @brief
     *
     * @param net
     * @param part
     * @param fromPart
     * @param v
     */
    auto update_move_general_net(const std::vector<std::uint8_t> &part,
                                 const MoveInfo& move_info) -> void;
};

#endif
