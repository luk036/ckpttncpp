#ifndef CKPTTNCPP_FMBIGAINCALC_HPP
#define CKPTTNCPP_FMBIGAINCALC_HPP 1

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
    using index_t = typename SimpleNetlist::index_t;

  private:
    SimpleNetlist &H;
    index_t num_modules{};
    std::vector<dllink<index_t>> vertex_list;

  public:
    int totalcost{0};

    /**
     * @brief Construct a new FMBiGainCalc object
     *
     * @param H
     * @param K
     */
    explicit FMBiGainCalc(SimpleNetlist &H, std::uint8_t /*K*/)
        : H{H}, vertex_list(H.number_of_modules()) {}

    /**
     * @brief
     *
     * @param part
     */
    auto init(const std::vector<uint8_t> &part) -> int {
        // for (auto &net : this->H.net_list) {
        this->totalcost = 0;
        for (auto &vlink : this->vertex_list) {
            vlink.key = 0;
        }
        // auto &[part, extern_nets] = part_info;
        for (auto net : this->H.nets) {
            this->init_gain(net, part);
        }
        return this->totalcost;
    }

    /**
     * @brief
     *
     * @param toPart
     * @return dllink*
     */
    auto start_ptr(std::uint8_t /*toPart*/) -> dllink<index_t> * {
        return &this->vertex_list[0];
    }

    /**
     * @brief
     *
     */
    auto update_move_init() -> void {
        // nothing to do in 2-way partitioning
    }

    using ret_2pin_info = std::tuple<index_t, int>;

    /**
     * @brief
     *
     * @param part
     * @param move_info
     * @return ret_2pin_info
     */
    auto update_move_2pin_net(const std::vector<uint8_t> &part,
                              const MoveInfo &move_info) -> ret_2pin_info;

    using ret_info = std::tuple<std::vector<index_t>, std::vector<int>>;

    /**
     * @brief
     *
     * @param net
     * @param part
     * @param fromPart
     * @param v
     */
    auto update_move_3pin_net(const std::vector<uint8_t> &part,
                              const MoveInfo &move_info) -> ret_info;
    /**
     * @brief
     *
     * @param net
     * @param part
     * @param fromPart
     * @param v
     */
    auto update_move_general_net(const std::vector<uint8_t> &part,
                                 const MoveInfo &move_info) -> ret_info;

  protected:
    /**
     * @brief
     *
     * @param w
     * @param weight
     */
    auto modify_gain(index_t i_w, int weight) -> void {
        this->vertex_list[i_w].key += weight;
    }

  private:
    /**
     * @brief
     *
     * @param net
     * @param part
     */
    auto init_gain(node_t net, const std::vector<uint8_t> &part) -> void;

    /**
     * @brief
     *
     * @param net
     * @param part
     */
    auto init_gain_2pin_net(node_t net, const std::vector<std::uint8_t> &part)
        -> void;

    /**
     * @brief
     *
     * @param net
     * @param part
     */
    auto init_gain_3pin_net(node_t net, const std::vector<std::uint8_t> &part)
        -> void;

    /**
     * @brief
     *
     * @param net
     * @param part
     */
    auto init_gain_general_net(node_t net,
                               const std::vector<std::uint8_t> &part) -> void;
};

#endif
