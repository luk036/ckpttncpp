#pragma once

#include "dllist.hpp"  // import dllink
#include "netlist.hpp" // import Netlist
#include <gsl/span>

// struct FMBiGainMgr;

/*!
 * @brief FMBiGainCalc
 *
 */
class FMBiGainCalc
{
    friend class FMBiGainMgr;
    using node_t = typename SimpleNetlist::node_t;

  private:
    const SimpleNetlist& H;
    size_t num_modules {};
    std::vector<dllink<node_t>> vertex_list;

  public:
    int totalcost {0};
    bool special_handle_2pin_nets {true};

    /*!
     * @brief Construct a new FMBiGainCalc object
     *
     * @param H
     * @param K
     */
    explicit FMBiGainCalc(const SimpleNetlist& H, uint8_t /*K*/)
        : H {H}
        , vertex_list(H.number_of_modules())
    {
    }

    /*!
     * @brief
     *
     * @param part
     */
    auto init(gsl::span<const uint8_t> part) -> int
    {
        this->totalcost = 0;
        for (auto&& vlink : this->vertex_list)
        {
            vlink.key = 0;
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
     * @param toPart
     * @return dllink*
     */
    auto start_ptr(uint8_t /*toPart*/) -> dllink<node_t>*
    {
        return &this->vertex_list[0];
    }

    /*!
     * @brief update move init
     *
     */
    auto update_move_init() -> void
    {
        // nothing to do in 2-way partitioning
    }

    using ret_2pin_info = std::tuple<node_t, int>;

    /*!
     * @brief update move 2-pin net
     *
     * @param part
     * @param move_info
     * @return ret_2pin_info
     */
    auto update_move_2pin_net(gsl::span<const uint8_t> part,
        const MoveInfo& move_info) -> ret_2pin_info;

    using ret_info = std::tuple<std::vector<node_t>, std::vector<int>>;

    /*!
     * @brief update move 3-pin net
     * 
     * @param part 
     * @param move_info 
     * @return ret_info 
     */
    auto update_move_3pin_net(
        gsl::span<const uint8_t> part, const MoveInfo& move_info) -> ret_info;

    /*!
     * @brief update move general net
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
     * @param w
     * @param weight
     */
    auto _modify_gain(node_t w, int weight) -> void
    {
        this->vertex_list[w].key += weight;
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
    auto _init_gain_general_net(const node_t& net, gsl::span<const uint8_t> part)
        -> void;
};
