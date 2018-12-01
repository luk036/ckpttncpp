#ifndef _HOME_UBUNTU_GITHUB_CKPTTNCPP_FMBIGAINMGR_HPP
#define _HOME_UBUNTU_GITHUB_CKPTTNCPP_FMBIGAINMGR_HPP 1

#include "FMBiGainCalc.hpp"
#include "FMGainMgr.hpp"
#include "bpqueue.hpp" // import bpqueue
#include "dllist.hpp"  // import dllink
#include "netlist.hpp" // import Netlist
#include <cassert>
#include <cinttypes>
#include <iterator>

struct FMBiGainMgr;

/**
 * @brief FMBiGainMgr
 *
 */
struct FMBiGainMgr : public FMGainMgr<FMBiGainCalc, FMBiGainMgr> {
    using Base = FMGainMgr<FMBiGainCalc, FMBiGainMgr>;

    /**
     * @brief Construct a new FMBiGainMgr object
     *
     * @param H
     */
    explicit FMBiGainMgr(Netlist &H): Base{H} {}

    /**
     * @brief
     *
     * @param part
     */
    auto init(const std::vector<std::uint8_t> &part) -> void;

    auto set_key(std::uint8_t whichPart, node_t v, int key) -> void {
        this->gainbucket[whichPart]->set_key(
            this->gainCalc.vertex_list[v], key);
    }

    auto modify_key(const std::vector<std::uint8_t> &part, node_t w, int key)
        -> void {
        auto part_w = part[w];
        this->gainbucket[1 - part_w]->modify_key(this->gainCalc.vertex_list[w],
                                                 key);
    }

    /**
     * @brief
     *
     * @param part
     * @param move_info_v
     * @param gain
     */
    auto update_move_v(const std::vector<std::uint8_t> &part,
                     const MoveInfoV &move_info_v, int gain) -> void {
        // this->vertex_list[v].key -= 2 * gain;
        auto const &[fromPart, toPart, v] = move_info_v;
        this->set_key(fromPart, v, -gain);
    }
};

#endif
