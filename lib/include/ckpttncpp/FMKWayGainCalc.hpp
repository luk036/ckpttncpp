#ifndef _HOME_UBUNTU_GITHUB_CKPTTNCPP_FMKWayGAINCALC_HPP
#define _HOME_UBUNTU_GITHUB_CKPTTNCPP_FMKWayGAINCALC_HPP 1

// #include "bpqueue.hpp" // import bpqueue
#include "dllist.hpp"  // import dllink
#include "netlist.hpp" // import Netlist

/**
 * @brief FMKWayGainCalc
 *
 */
class FMKWayGainCalc {
  private:
    Netlist &H;
    std::uint8_t K;
    size_t num_modules;

  public:
    std::vector<std::vector<dllink>> vertex_list;
    std::vector<int> deltaGainV;

    /**
     * @brief Construct a new FMKWayGainCalc object
     *
     * @param H Netlist
     * @param K number of partitions
     */
    FMKWayGainCalc(Netlist &H, std::uint8_t K)
        : H{H}, K{K}, num_modules{H.number_of_modules()}, deltaGainV(K, 0) 
    {
        for (auto k = 0u; k < this->K; ++k) {
            this->vertex_list.emplace_back(
                std::vector<dllink>(this->num_modules));
        }
    }

    auto start_ptr(std::uint8_t toPart) -> dllink* {
        return &this->vertex_list[toPart][0];
    }

    /**
     * @brief
     *
     * @param part
     * @param vertex_list
     */
    auto init(const std::vector<std::uint8_t> &part) -> void {
        for (auto &net : this->H.net_list) {
            this->init_gain(net, part);
        }
    }

    /**
     * @brief Set the key object
     * 
     * @param v 
     * @param key 
     */
    auto set_key(node_t v, int key) -> void {
        for (auto k = 0u; k < this->K; ++k) {
            this->vertex_list[k][v].key = key;
        }
    }

    /**
     * @brief 
     * 
     * @param v 
     * @param weight 
     */
    auto modify_gain(node_t v, int weight) -> void {
        for (auto k = 0u; k < this->K; ++k) {
            this->vertex_list[k][v].key += weight;
        }
    }

    auto update_move_init() -> void {
        std::fill_n(this->deltaGainV.begin(), this->K, 0);
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
                              const MoveInfo &move_info) -> ret_2pin_info;

    using ret_info =
        std::tuple<std::vector<size_t>, std::vector<std::vector<int>>>;

    /**
     * @brief
     *
     * @param part
     * @param move_info
     * @return ret_info
     */
    auto update_move_general_net(const std::vector<std::uint8_t> &part,
                                 const MoveInfo &move_info) -> ret_info;

  private:
    /**
     * @brief
     *
     * @param net
     * @param part
     */
    auto init_gain(node_t &net, const std::vector<std::uint8_t> &part) -> void;

    /**
     * @brief
     *
     * @param net
     * @param part
     */
    auto init_gain_2pin_net(node_t &net, const std::vector<std::uint8_t> &part) -> void;

    /**
     * @brief
     *
     * @param net
     * @param part
     * @param vertex_list
     */
    auto init_gain_general_net(node_t &net,
                               const std::vector<std::uint8_t> &part) -> void;
};

#endif
