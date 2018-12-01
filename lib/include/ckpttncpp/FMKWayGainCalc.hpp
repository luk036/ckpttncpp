#ifndef _HOME_UBUNTU_GITHUB_CKPTTNCPP_FMKWayGAINCALC_HPP
#define _HOME_UBUNTU_GITHUB_CKPTTNCPP_FMKWayGAINCALC_HPP 1

// #include "bpqueue.hpp" // import bpqueue
#include "dllist.hpp"  // import dllink
#include "netlist.hpp" // import Netlist
#include <cassert>
#include <iterator>
#include <tuple>
#include <cinttypes>

/**
 * @brief FMKWayGainCalc
 *
 */
class FMKWayGainCalc {
  private:
    Netlist &H;
    std::uint8_t K;

  public:

    /**
     * @brief Construct a new FMKWayGainCalc object
     * 
     * @param H Netlist
     * @param K number of partitions
     */
    explicit FMKWayGainCalc(Netlist &H, std::uint8_t K) : H{H}, K{K} {}

    /**
     * @brief 
     * 
     * @param part 
     * @param vertex_list 
     */
    auto init(const std::vector<std::uint8_t> &part,
              std::vector<std::vector<dllink>> &vertex_list) -> void {
        for (auto &net : this->H.net_list) {
            this->init_gain(net, part, vertex_list);
        }
    }

    using ret_2pin_info = std::tuple<size_t, std::vector<int>>; 

    /**
     * @brief 
     * 
     * @param part 
     * @param move_info 
     * @return ret_2pin_info 
     */
    auto update_move_2pin_net(const std::vector<std::uint8_t> &part,
                              const MoveInfo& move_info,
                              std::vector<int>& deltaGainV) -> ret_2pin_info;

    using ret_info = std::tuple<std::vector<size_t>,
                                std::vector<std::vector<int>>>; 

    /**
     * @brief 
     * 
     * @param part 
     * @param move_info 
     * @return ret_info 
     */
    auto update_move_general_net(const std::vector<std::uint8_t> &part,
                                 const MoveInfo& move_info,
                                 std::vector<int>& deltaGainV) -> ret_info;

  private:

    /**
     * @brief
     *
     * @param net
     * @param part
     */
    auto init_gain(node_t &net, const std::vector<std::uint8_t> &part,
                   std::vector<std::vector<dllink>> &vertex_list) -> void;
  
    /**
     * @brief
     *
     * @param net
     * @param part
     */
    auto init_gain_2pin_net(node_t &net, const std::vector<std::uint8_t> &part,
                            std::vector<std::vector<dllink>> &vertex_list) -> void;

    /**
     * @brief
     *
     * @param net
     * @param part
     * @param vertex_list
     */
    auto init_gain_general_net(node_t &net, const std::vector<std::uint8_t> &part,
                               std::vector<std::vector<dllink>> &vertex_list) -> void;
};

#endif
