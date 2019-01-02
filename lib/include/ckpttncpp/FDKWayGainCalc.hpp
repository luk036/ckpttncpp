#ifndef _HOME_UBUNTU_GITHUB_CKPTTNCPP_FDKWayGAINCALC_HPP
#define _HOME_UBUNTU_GITHUB_CKPTTNCPP_FDKWayGAINCALC_HPP 1

// #include "bpqueue.hpp" // import bpqueue
#include "dllist.hpp"  // import dllink
#include "netlist.hpp" // import Netlist
#include "robin.hpp"   // import robin

class FDKWayGainMgr;

/**
 * @brief FDKWayGainCalc
 *
 */
class FDKWayGainCalc {
    friend FDKWayGainMgr;

  private:
    SimpleNetlist &H;
    std::uint8_t K;
    robin RR;
    size_t num_modules;
    std::vector<std::vector<dllink>> vertex_list;
    std::vector<int> deltaGainV;

  public:
    int totalcost;

    /**
     * @brief Construct a new FDKWayGainCalc object
     *
     * @param H Netlist
     * @param K number of partitions
     */
    FDKWayGainCalc(SimpleNetlist &H, std::uint8_t K)
        : H{H}, K{K}, RR{K}, num_modules{H.number_of_modules()},
          deltaGainV(K, 0), totalcost{0} {
        for (auto k = 0u; k < this->K; ++k) {
            this->vertex_list.emplace_back(
                std::vector<dllink>(this->num_modules));
        }
    }

    /**
     * @brief
     *
     * @param toPart
     * @return dllink*
     */
    auto start_ptr(std::uint8_t toPart) -> dllink * {
        return &this->vertex_list[toPart][0];
    }

    /**
     * @brief
     *
     * @param part_info
     */
    auto init(const PartInfo &part_info) -> int {
        this->totalcost = 0;
        for (auto k = 0u; k < this->K; ++k) {
            for (auto &vlink : this->vertex_list[k]) {
                vlink.key = 0;
            }
        }
        for (auto net : this->H.nets) {
            this->init_gain(net, part_info);
        }

        return this->totalcost;
    }

    /**
     * @brief
     *
     * @param v
     * @param part_v
     * @param weight
     */
    auto modify_gain(node_t v, std::uint8_t part_v, int weight) -> void {
        for (auto &&k : this->RR.exclude(part_v)) {
            this->vertex_list[k][v].key += weight;
        }
    }

    /**
     * @brief
     *
     */
    auto update_move_init() -> void {
        std::fill_n(this->deltaGainV.begin(), this->K, 0);
    }

    using ret_2pin_info = std::tuple<size_t, std::vector<int>>;

    /**
     * @brief
     *
     * @param part_info
     * @param move_info
     * @return ret_2pin_info
     */
    auto update_move_2pin_net(PartInfo &part_info, const MoveInfo &move_info)
        -> ret_2pin_info;

    using ret_info =
        std::tuple<std::vector<size_t>, std::vector<std::vector<int>>>;

    /**
     * @brief
     *
     * @param part_info
     * @param move_info
     * @return ret_info
     */
    auto update_move_general_net(PartInfo &part_info, const MoveInfo &move_info)
        -> ret_info;

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
     */
    auto init_gain_2pin_net(node_t net, const std::vector<std::uint8_t> &part)
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
