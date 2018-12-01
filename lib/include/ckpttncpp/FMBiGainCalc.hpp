#ifndef _HOME_UBUNTU_GITHUB_CKPTTNCPP_FMBIGAINCALC_HPP
#define _HOME_UBUNTU_GITHUB_CKPTTNCPP_FMBIGAINCALC_HPP 1

// #include "bpqueue.hpp" // import bpqueue
#include "dllist.hpp"  // import dllink
#include "netlist.hpp" // import Netlist
#include <cassert>
#include <iterator>
#include <tuple>
#include <cinttypes>

/**
 * @brief FMBiGainCalc
 *
 */
class FMBiGainCalc {
  private:
    Netlist &H;

  public:
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
    auto init(const std::vector<std::uint8_t> &part, std::vector<dllink> &vertex_list)
        -> void {
        for (auto &net : this->H.net_list) {
            this->init_gain(net, part, vertex_list);
        }
    }

    using ret_2pin_info = std::tuple<node_t, int>; 

    /**
     * @brief
     *
     * @param net
     * @param part
     * @param fromPart
     * @param v
     */
    auto update_move_2pin_net(const std::vector<std::uint8_t> &part,
                              const MoveInfo& move_info) -> ret_2pin_info;

    using ret_info = std::tuple<std::vector<node_t>, std::vector<int>>; 

    /**
     * @brief
     *
     * @param net
     * @param part
     * @param fromPart
     * @param v
     */
    auto update_move_general_net(const std::vector<std::uint8_t> &part,
                                 const MoveInfo& move_info) -> ret_info;

  private:
  
    /**
     * @brief
     *
     * @param net
     * @param part
     */
    auto init_gain(node_t &net, const std::vector<std::uint8_t> &part,
                   std::vector<dllink> &vertex_list) -> void;

    /**
     * @brief
     *
     * @param net
     * @param part
     */
    auto init_gain_2pin_net(node_t &net, const std::vector<std::uint8_t> &part,
                            std::vector<dllink> &vertex_list) -> void;

    /**
     * @brief
     *
     * @param net
     * @param part
     */
    auto init_gain_general_net(node_t &net, const std::vector<std::uint8_t> &part,
                               std::vector<dllink> &vertex_list) -> void;
};

#endif
