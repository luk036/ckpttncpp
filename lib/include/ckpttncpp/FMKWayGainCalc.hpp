#pragma once

#include "dllist.hpp"  // import dllink
#include "netlist.hpp" // import Netlist
#include "robin.hpp"   // import robin
#include <gsl/span>

// class FMKWayGainMgr;

/*!
 * @brief FMKWayGainCalc
 *
 */
class FMKWayGainCalc
{
    friend class FMKWayGainMgr;
    using node_t = typename SimpleNetlist::node_t;

  private:
    const SimpleNetlist& H;
    uint8_t K;
    robin<uint8_t> RR;
    size_t num_modules;
    std::vector<std::vector<dllink<node_t>>> vertex_list;
    std::vector<int> deltaGainV;

  public:
    int totalcost {0};
    bool special_handle_2pin_nets {true};

    /*!
     * @brief Construct a new FMKWayGainCalc object
     *
     * @param H Netlist
     * @param K number of partitions
     */
    FMKWayGainCalc(const SimpleNetlist& H, uint8_t K)
        : H {H}
        , K {K}
        , RR {K}
        , num_modules {H.number_of_modules()}
        , deltaGainV(K, 0)
    {
        for (auto k = 0U; k != this->K; ++k)
        {
            this->vertex_list.emplace_back(
                std::vector<dllink<node_t>>(this->num_modules));
        }
    }

    /*!
     * @brief
     *
     * @param toPart
     * @return dllink*
     */
    auto start_ptr(uint8_t toPart) -> dllink<node_t>*
    {
        return &this->vertex_list[toPart][0];
    }

    /*!
     * @brief
     *
     * @param part
     */
    auto init(gsl::span<const uint8_t> part) -> int
    {
        this->totalcost = 0;
        for (auto k = 0U; k != this->K; ++k)
        {
            for (auto&& vlink : this->vertex_list[k])
            {
                vlink.key = 0;
            }
        }
        for (auto&& net : this->H.nets)
        {
            this->_init_gain(net, part);
        }
        return this->totalcost;
    }

    /*!
     * @brief
     *
     */
    auto update_move_init() -> void
    {
        std::fill_n(this->deltaGainV.begin(), this->K, 0);
    }

    using ret_2pin_info = std::tuple<node_t, std::vector<int>>;

    /*!
     * @brief
     *
     * @param part
     * @param move_info
     * @return ret_2pin_info
     */
    auto update_move_2pin_net(gsl::span<const uint8_t> part,
        const MoveInfo& move_info) -> ret_2pin_info;

    using ret_info =
        std::tuple<std::vector<node_t>, std::vector<std::vector<int>>>;

    /*!
     * @brief
     *
     * @param part
     * @param move_info
     * @return ret_info
     */
    auto update_move_3pin_net(
        gsl::span<const uint8_t> part, const MoveInfo& move_info) -> ret_info;

    /*!
     * @brief
     *
     * @param part
     * @param move_info
     * @return ret_info
     */
    auto update_move_general_net(
        gsl::span<const uint8_t> part, const MoveInfo& move_info) -> ret_info;

  private:
    /*!
     * @brief
     *
     * @param v
     * @param weight
     */
    auto _modify_gain(node_t v, uint8_t part_v, int weight) -> void
    {
        for (auto&& k : this->RR.exclude(part_v))
        {
            this->vertex_list[k][v].key += weight;
        }
    }

    /*!
     * @brief
     *
     * @param net
     * @param part
     */
    auto _init_gain(const node_t& net, gsl::span<const uint8_t> part) -> void;

    /*!
     * @brief
     *
     * @param net
     * @param part
     */
    auto _init_gain_2pin_net(const node_t& net, gsl::span<const uint8_t> part)
        -> void;

    /*!
     * @brief
     *
     * @param net
     * @param part
     */
    auto _init_gain_3pin_net(const node_t& net, gsl::span<const uint8_t> part)
        -> void;

    /*!
     * @brief
     *
     * @param net
     * @param part
     */
    auto _init_gain_general_net(
        const node_t& net, gsl::span<const uint8_t> part) -> void;
};
