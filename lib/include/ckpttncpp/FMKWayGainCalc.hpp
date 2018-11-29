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
class FMKWayGainCalc {
  private:
    Netlist &H;
    size_t K;

  public:
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

    using ret_2pin_info = std::tuple<size_t, std::vector<int>, std::vector<int>>; 

    /**
     * @brief
     *
     * @param net
     * @param part
     * @param fromPart
     * @param v
     */
    auto update_move_2pin_net(std::vector<size_t> &part,
                              const MoveInfo& move_info) -> ret_2pin_info;

    using ret_info = std::tuple<std::vector<size_t>,
                                std::vector<std::vector<int>>,
                                std::vector<int>>; 
    /**
     * @brief
     *
     * @param net
     * @param part
     * @param fromPart
     * @param v
     */
    auto update_move_general_net(std::vector<size_t> &part,
                                 const MoveInfo& move_info) -> ret_info;

  private:

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
                            std::vector<std::vector<dllink>> &vertex_list) -> void;

    /**
     * @brief
     *
     * @param net
     * @param part
     * @param vertex_list
     */
    auto init_gain_general_net(node_t &net, std::vector<size_t> &part,
                               std::vector<std::vector<dllink>> &vertex_list) -> void;
};

#endif
