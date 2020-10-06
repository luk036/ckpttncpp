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
    std::vector<dllink<int>> vertex_list;

  public:
    int totalcost {0};
    bool special_handle_2pin_nets {true};

    /*!
     * @brief Construct a new FMBiGainCalc object
     *
     * @param[in] H
     * @param[in] K
     */
    explicit FMBiGainCalc(const SimpleNetlist& H, std::uint8_t /*K*/)
        : H {H}
        , vertex_list(H.number_of_modules())
    {
    }

    /*!
     * @brief
     *
     * @param[in] part
     */
    auto init(gsl::span<const std::uint8_t> part) -> int
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
     * @param[in] toPart
     * @return dllink*
     */
    auto start_ptr(std::uint8_t /*toPart*/) -> dllink<int>*
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

    /*!
     * @brief update move 2-pin net
     *
     * @param[in] part
     * @param[in] move_info
     * @param[out] w
     * @return int
     */
    auto update_move_2pin_net(gsl::span<const std::uint8_t> part,
        const MoveInfo<node_t>& move_info, node_t& w) -> int;

    /*!
     * @brief update move 3-pin net
     *
     * @param[in] part
     * @param[in] move_info
     * @param[out] IdVec
     * @return ret_info
     */
    auto update_move_3pin_net(gsl::span<const std::uint8_t> part,
        const MoveInfo<node_t>& move_info, std::pmr::vector<node_t>& IdVec)
        -> std::vector<int>;

    /*!
     * @brief update move general net
     *
     * @param[in] part
     * @param[in] move_info
     * @param[out] IdVec
     * @return ret_info
     */
    auto update_move_general_net(gsl::span<const std::uint8_t> part,
        const MoveInfo<node_t>& move_info, std::pmr::vector<node_t>& IdVec)
        -> std::vector<int>;

  private:
    /*!
     * @brief
     *
     * @param[in] w
     * @param[in] weight
     */
    auto _modify_gain(const node_t& w, int weight) -> void
    {
        this->vertex_list[w].key += weight;
    }

    /**
     * @brief
     *
     * @tparam Ts
     * @param weight
     * @param w
     */
    // template <typename... Ts>
    // auto _modify_gain_va(int weight, Ts... w) -> void
    // {
    //     ((this->vertex_list[w].key += weight), ...);
    // }

    /**
     * @brief
     *
     * @tparam Ts
     * @param weight
     * @param w
     */
    auto _modify_gain_va(int weight, const node_t& w1) -> void
    {
        this->vertex_list[w1].key += weight;
    }

    /**
     * @brief
     *
     * @tparam Ts
     * @param weight
     * @param w
     */
    auto _modify_gain_va(int weight, const node_t& w1, const node_t& w2) -> void
    {
        this->vertex_list[w1].key += weight;
        this->vertex_list[w2].key += weight;
    }

    /**
     * @brief
     *
     * @tparam Ts
     * @param weight
     * @param w
     */
    auto _modify_gain_va(int weight, const node_t& w1, const node_t& w2, const node_t& w3) -> void
    {
        this->vertex_list[w1].key += weight;
        this->vertex_list[w2].key += weight;
        this->vertex_list[w3].key += weight;
    }

    /*!
     * @brief
     *
     * @param[in] net
     * @param[in] part
     */
    auto _init_gain(const node_t& net, gsl::span<const std::uint8_t> part)
        -> void;

    /*!
     * @brief
     *
     * @param[in] net
     * @param[in] part
     */
    auto _init_gain_2pin_net(
        const node_t& net, gsl::span<const std::uint8_t> part) -> void;

    /*!
     * @brief
     *
     * @param[in] net
     * @param[in] part
     */
    auto _init_gain_3pin_net(
        const node_t& net, gsl::span<const std::uint8_t> part) -> void;

    /*!
     * @brief
     *
     * @param[in] net
     * @param[in] part
     */
    auto _init_gain_general_net(
        const node_t& net, gsl::span<const std::uint8_t> part) -> void;
};
