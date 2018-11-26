#ifndef _HOME_UBUNTU_GITHUB_CKPTTNCPP_FMBIGAINMGR_HPP
#define _HOME_UBUNTU_GITHUB_CKPTTNCPP_FMBIGAINMGR_HPP 1

#include "FMBiGainCalc.hpp"
#include "bpqueue.hpp" // import bpqueue
#include "dllist.hpp"  // import dllink
#include "netlist.hpp" // import Netlist
#include <cassert>
#include <iterator>

/**
 * @brief FMBiGainMgr
 *
 */
struct FMBiGainMgr {
    Netlist &H;
    FMBiGainCalc gainCalc;
    size_t pmax;
    bpqueue gainbucket;
    // size_t num[2];
    size_t num_cells;
    std::vector<dllink> vertex_list;
    dllink waitinglist;

    /**
     * @brief Construct a new FMBiGainMgr object
     *
     * @param H
     */
    explicit FMBiGainMgr(Netlist &H)
        : H{H}, gainCalc{H}, pmax{H.get_max_degree()}, gainbucket(-pmax, pmax),
          // num{0, 0},
          num_cells{H.number_of_cells()},
          vertex_list(num_cells), waitinglist{} {}

    /**
     * @brief
     *
     * @return true
     * @return false
     */
    auto is_empty() const -> bool { return this->gainbucket.is_empty(); }

    /**
     * @brief
     *
     * @return std::tuple<size_t, int>
     */
    auto select() -> std::tuple<size_t, int> {
        auto gainmax = this->gainbucket.get_max();
        auto &vlink = this->gainbucket.popleft();
        this->waitinglist.append(vlink);
        return {&vlink - &this->vertex_list[0], gainmax};
    }

    /**
     * @brief Get the vertex id object
     *
     * @param vlink
     * @return size_t
     */
    auto get_vertex_id(const dllink &vlink) const -> size_t {
        return &vlink - &vertex_list[0];
    }

    /**
     * @brief
     *
     * @param part
     */
    auto init(std::vector<size_t> &part) -> void {
        this->gainCalc.init(part, this->vertex_list);

        for (auto &v : this->H.cell_fixed) {
            // auto i_v = this->H.cell_dict[v];
            // force to the lowest gain
            this->vertex_list[v].key = -this->pmax;
        }
        this->gainbucket.appendfrom(this->vertex_list);
    }

    /**
     * @brief
     *
     * @param part
     * @param v
     */
    auto update_move(std::vector<size_t> &part, const MoveInfoV& move_info_v,
                     int gain) -> void {
        auto [fromPart, toPart, v] = move_info_v;
        for (auto net : this->H.G[v]) {
            auto move_info = MoveInfo{net, fromPart, toPart, v};
            if (this->H.G.degree(net) == 2) {
                this->update_move_2pin_net(part, move_info);
            } else if (unlikely(this->H.G.degree(net) < 2)) {
                break; // does not provide any gain change when move
            } else {
                this->update_move_general_net(part, move_info);
            }
        }
        this->vertex_list[v].key -= 2 * gain;
    }

    /**
     * @brief
     *
     * @param net
     * @param part
     * @param fromPart
     * @param v
     */
    auto update_move_2pin_net(std::vector<size_t> &part,
                              const MoveInfo& move_info) -> void {
        auto [w, deltaGainW] =
            this->gainCalc.update_move_2pin_net(part, move_info);
        this->gainbucket.modify_key(this->vertex_list[w], deltaGainW);
    }

    /**
     * @brief
     *
     * @param net
     * @param part
     * @param fromPart
     * @param v
     */
    auto update_move_general_net(std::vector<size_t> &part,
                                 const MoveInfo& move_info) -> void {
        auto [IdVec, deltaGain] =
            this->gainCalc.update_move_general_net(part, move_info);
        auto degree = std::size(IdVec);
        for (auto idx = 0u; idx < degree; ++idx) {
            this->gainbucket.modify_key(this->vertex_list[IdVec[idx]],
                                        deltaGain[idx]);
        }
    }
};

#endif
