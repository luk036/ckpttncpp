#ifndef _HOME_UBUNTU_GITHUB_CKPTTNCPP_FMBIGAINCALC_HPP
#define _HOME_UBUNTU_GITHUB_CKPTTNCPP_FMBIGAINCALC_HPP 1

// #include "bpqueue.hpp" // import bpqueue
#include "dllist.hpp"  // import dllink
#include "netlist.hpp" // import Netlist

class FMBiGainMgr;

/**
 * @brief FMBiGainCalc
 *
 */
class FMBiGainCalc {
    friend FMBiGainMgr;

  private:
    Netlist &H;
    size_t num_modules;
    std::vector<dllink> vertex_list;

  public:

    /**
     * @brief Construct a new FMBiGainCalc object
     *
     * @param H
     * @param K
     */
    explicit FMBiGainCalc(Netlist &H, std::uint8_t K = 2)
        : H{H}, num_modules{H.number_of_modules()}, vertex_list(num_modules) {}

    /**
     * @brief
     *
     * @param part
     */
    auto init(const std::vector<std::uint8_t> &part) -> void {
        for (auto &net : this->H.net_list) {
            this->init_gain(net, part);
        }
    }

    /**
     * @brief
     *
     * @param toPart
     * @return dllink*
     */
    auto start_ptr(std::uint8_t toPart) -> dllink * {
        return &this->vertex_list[0];
    }

    /**
     * @brief Set the key object
     *
     * @param v
     * @param key
     */
    auto set_key(node_t v, int key) -> void { this->vertex_list[v].key = key; }

    /**
     * @brief
     *
     */
    auto update_move_init() -> void {
        // nothing to do in 2-way partitioning
    }

    using ret_2pin_info = std::tuple<node_t, int>;

    /**
     * @brief
     *
     * @param part
     * @param move_info
     * @return ret_2pin_info
     */
    auto update_move_2pin_net(const std::vector<std::uint8_t> &part,
                              const MoveInfo &move_info) -> ret_2pin_info;

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
    auto init_gain_2pin_net(node_t &net, const std::vector<std::uint8_t> &part)
        -> void;

    /**
     * @brief
     *
     * @param net
     * @param part
     */
    auto init_gain_general_net(node_t &net,
                               const std::vector<std::uint8_t> &part) -> void;
};

#endif
