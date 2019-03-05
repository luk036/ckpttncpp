#ifndef CKPTTNCPP_FDBIGAINCALC_HPP
#define CKPTTNCPP_FDBIGAINCALC_HPP 1

#include "dllist.hpp"  // import dllink
#include "netlist.hpp" // import Netlist

class FDBiGainMgr;

/**
 * @brief FDBiGainCalc
 *
 */
class FDBiGainCalc {
    friend FDBiGainMgr;

  private:
    SimpleNetlist &H;
    size_t num_modules{};
    std::vector<dllink<int>> vertex_list;

  public:
    int totalcost;

    /**
     * @brief Construct a new FDBiGainCalc object
     *
     * @param H
     * @param K
     */
    explicit FDBiGainCalc(SimpleNetlist &H, std::uint8_t  K = 2)
        : H{H}, vertex_list(H.number_of_modules()), totalcost{0} {}

    /**
     * @brief
     *
     * @param part_info
     * @return int
     */
    auto init(const PartInfo &part_info) -> int {
        // for (auto &net : this->H.net_list) {
        this->totalcost = 0;
        for (auto &vlink : this->vertex_list) {
            vlink.key = 0;
        }
        for (auto net : this->H.nets) {
            this->init_gain(net, part_info);
        }
        return this->totalcost;
    }

    /**
     * @brief
     *
     * @param toPart
     * @return dllink*
     */
    auto start_ptr(std::uint8_t  /*toPart*/) -> dllink<int> * {
        return &this->vertex_list[0];
    }

    /**
     * @brief
     *
     */
    auto update_move_init() -> void {
        // nothing to do in 2-way partitioning
    }

    using ret_2pin_info = std::tuple<size_t, int>;

    /**
     * @brief
     *
     * @param part_info
     * @param move_info
     * @return ret_2pin_info
     */
    auto update_move_2pin_net(PartInfo &part_info, const MoveInfo &move_info)
        -> ret_2pin_info;

    using ret_info = std::tuple<std::vector<size_t>, std::vector<int>>;

    /**
     * @brief
     *
     * @param part_info
     * @param move_info
     */
    auto update_move_general_net(PartInfo &part_info, const MoveInfo &move_info)
        -> ret_info;

  protected:
    /**
     * @brief
     *
     * @param w
     * @param weight
     */
    auto modify_gain(size_t i_w, int weight) -> void {
        this->vertex_list[i_w].key += weight;
    }

  private:
    /**
     * @brief
     *
     * @param net
     * @param part_info
     */
    auto init_gain(node_t net, const PartInfo &part_info) -> void;

    /**
     * @brief
     *
     * @param net
     * @param part
     * @param weight
     */
    auto init_gain_3pin_net(node_t net,
                            const std::vector<std::uint8_t> &part,
                            int weight) -> void;

    /**
     * @brief
     *
     * @param net
     * @param part
     * @param weight
     */
    auto init_gain_general_net(node_t net,
                               const std::vector<std::uint8_t> &part,
                               int weight) -> void;
};

#endif
