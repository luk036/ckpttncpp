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
    std::uint8_t K;
    robin<std::uint8_t> RR;
    // size_t num_modules;
    std::vector<std::vector<dllink<node_t>>> vertex_list;
    std::vector<int> deltaGainV;
    int totalcost {0};

  public:
    bool special_handle_2pin_nets {true}; // @TODO should be template parameter

    /*!
     * @brief Construct a new FMKWayGainCalc object
     *
     * @param[in] H Netlist
     * @param[in] K number of partitions
     */
    FMKWayGainCalc(const SimpleNetlist& H, std::uint8_t K)
        : H {H}
        , K {K}
        , RR {K}
        , deltaGainV(K, 0)
    {
        for (auto k = 0U; k != this->K; ++k)
        {
            this->vertex_list.emplace_back(
                std::vector<dllink<node_t>>(H.number_of_modules()));

            for (auto&& v : this->H.modules)
            {
                this->vertex_list[k][v].data = v;
            }
        }
    }

    /*!
     * @brief
     *
     * @param[in] toPart
     * @return dllink*
     */
    auto start_ptr(uint8_t toPart) -> dllink<node_t>*
    {
        return &this->vertex_list[toPart][0];
    }

    /*!
     * @brief
     *
     * @param[in] part
     */
    auto init(gsl::span<const std::uint8_t> part) -> int
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

    /*!
     * @brief
     *
     * @param[in] part
     * @param[in] move_info
     * @param[out] w
     * @return std::vector<int>
     */
    auto update_move_2pin_net(gsl::span<const std::uint8_t> part,
        const MoveInfo<node_t>& move_info, node_t& w) -> std::vector<int>;

    using ret_info = std::vector<std::vector<int>>;

    /*!
     * @brief
     *
     * @param[in] part
     * @param[in] move_info
     * @param[out] IdVec
     * @return ret_info
     */
    auto update_move_3pin_net(gsl::span<const std::uint8_t> part,
        const MoveInfo<node_t>& move_info, std::pmr::vector<node_t>& IdVec)
        -> ret_info;

    /*!
     * @brief
     *
     * @param[in] part
     * @param[in] move_info
     * @param[out] IdVec
     * @return ret_info
     */
    auto update_move_general_net(gsl::span<const std::uint8_t> part,
        const MoveInfo<node_t>& move_info, std::pmr::vector<node_t>& IdVec)
        -> ret_info;

  private:
    /*!
     * @brief
     *
     * @param[in] v
     * @param[in] part_v
     * @param[in] weight
     */
    auto _modify_gain(const node_t& v, std::uint8_t part_v, int weight) -> void
    {
        for (auto&& k : this->RR.exclude(part_v))
        {
            this->vertex_list[k][v].key += weight;
        }
    }

    /**
     * @brief
     *
     * @tparam Ts
     * @param weight
     * @param part_v
     * @param v
     */
    // template <typename... Ts>
    // auto _modify_vertex_va(int weight, std::uint8_t k, Ts... v) -> void
    // {
    //     ((this->vertex_list[k][v].key += weight), ...);
    // }

    /**
     * @brief
     *
     * @tparam Ts
     * @param weight
     * @param part_v
     * @param v
     */
    auto _modify_vertex_va(int weight, std::uint8_t k, const node_t& v1) -> void
    {
        this->vertex_list[k][v1].key += weight;
    }

    /**
     * @brief
     *
     * @tparam Ts
     * @param weight
     * @param part_v
     * @param v
     */
    auto _modify_vertex_va(
        int weight, std::uint8_t k, const node_t& v1, const node_t& v2) -> void
    {
        this->vertex_list[k][v1].key += weight;
        this->vertex_list[k][v2].key += weight;
    }

    /**
     * @brief
     *
     * @tparam Ts
     * @param weight
     * @param part_v
     * @param v
     */
    auto _modify_vertex_va(int weight, std::uint8_t k, const node_t& v1,
        const node_t& v2, const node_t& v3) -> void
    {
        this->vertex_list[k][v1].key += weight;
        this->vertex_list[k][v2].key += weight;
        this->vertex_list[k][v3].key += weight;
    }

    /**
     * @brief
     *
     * @tparam Ts
     * @param weight
     * @param part_v
     * @param v
     */
    template <typename... Ts>
    auto _modify_gain_va(int weight, std::uint8_t part_v, Ts... v) -> void
    {
        for (auto&& k : this->RR.exclude(part_v))
        {
            _modify_vertex_va(weight, k, v...);
        }
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
