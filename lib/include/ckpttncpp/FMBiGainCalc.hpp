#pragma once

#include "dllist.hpp"  // import dllink
#include "netlist.hpp" // import Netlist
#include <gsl/span>
#include <memory_resource>

// struct FMBiGainMgr;

/*!
 * @brief FMBiGainCalc
 *
 */
class FMBiGainCalc
{
    friend class FMBiGainMgr;
  public:  
    using node_t = typename SimpleNetlist::node_t;

  private:
    const SimpleNetlist& H;
    std::vector<dllink<std::pair<node_t, int16_t>>> vertex_list;
    int totalcost {0};
    uint16_t MAX_DEGREE {256};
    std::byte StackBuf[4096];
    std::pmr::monotonic_buffer_resource rsrc;

  public:
    int deltaGainW;
    std::pmr::vector<node_t> IdVec;
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
        , rsrc(StackBuf, sizeof StackBuf)
        , IdVec(&rsrc)
    {
        for (auto&& v : this->H.modules)
        {
            this->vertex_list[v].data = std::pair {v, int16_t(0)};
        }
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
            vlink.data.second = 0;
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
     * @deprecated
     * @param[in] toPart
     * @return dllink*
     * @deprecated
     */
    auto start_ptr(std::uint8_t /*toPart*/)
        -> dllink<std::pair<node_t, int16_t>>*
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

    void init_IdVec(const node_t& v, const node_t& net);

    /*!
     * @brief update move 2-pin net
     *
     * @param[in] part
     * @param[in] move_info
     * @param[out] w
     * @return int
     */
    auto update_move_2pin_net(gsl::span<const std::uint8_t> part,
        const MoveInfo<node_t>& move_info) -> node_t;

    /*!
     * @brief update move 3-pin net
     *
     * @param[in] part
     * @param[in] move_info
     * @return ret_info
     */
    auto update_move_3pin_net(gsl::span<const std::uint8_t> part,
        const MoveInfo<node_t>& move_info)
        -> std::vector<int>;

    /*!
     * @brief update move general net
     *
     * @param[in] part
     * @param[in] move_info
     * @return ret_info
     */
    auto update_move_general_net(gsl::span<const std::uint8_t> part,
        const MoveInfo<node_t>& move_info)
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
        this->vertex_list[w].data.second += weight;
    }

    /**
     * @brief
     *
     * @tparam Ts
     * @param weight
     * @param w
     */
    template <typename... Ts>
    auto _modify_gain_va(int weight, Ts... w) -> void
    {
        ((this->vertex_list[w].data.second += weight), ...);
    }

    // /**
    //  * @brief
    //  *
    //  * @tparam Ts
    //  * @param weight
    //  * @param w
    //  */
    // auto _modify_gain_va(int weight, const node_t& w1) -> void
    // {
    //     this->vertex_list[w1].data.second += weight;
    // }

    // /**
    //  * @brief
    //  *
    //  * @tparam Ts
    //  * @param weight
    //  * @param w
    //  */
    // auto _modify_gain_va(int weight, const node_t& w1, const node_t& w2) -> void
    // {
    //     this->vertex_list[w1].data.second += weight;
    //     this->vertex_list[w2].data.second += weight;
    // }

    // /**
    //  * @brief
    //  *
    //  * @tparam Ts
    //  * @param weight
    //  * @param w
    //  */
    // auto _modify_gain_va(int weight, const node_t& w1, const node_t& w2,
    //     const node_t& w3) -> void
    // {
    //     this->vertex_list[w1].data.second += weight;
    //     this->vertex_list[w2].data.second += weight;
    //     this->vertex_list[w3].data.second += weight;
    // }

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
