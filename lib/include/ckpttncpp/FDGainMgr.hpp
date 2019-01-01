#ifndef _HOME_UBUNTU_GITHUB_CKPTTNCPP_FDGAINMGR_HPP
#define _HOME_UBUNTU_GITHUB_CKPTTNCPP_FDGAINMGR_HPP 1

#include "bpqueue.hpp" // import bpqueue
#include "dllist.hpp"  // import dllink
#include "netlist.hpp" // import Netlist
#include <cassert>
#include <cinttypes>
#include <iterator>
#include <type_traits>

/**
 * @brief
 *
 * @tparam GainCalc
 * @tparam Derived
 */
template <typename GainCalc, class Derived> class FDGainMgr {

    Derived &self = *static_cast<Derived *>(this);

  protected:
    SimpleNetlist &H;

  public:
    GainCalc gainCalc;

  protected:
    size_t pmax;
    // size_t num[2];
    dllink waitinglist;
    std::vector<std::unique_ptr<bpqueue>> gainbucket;

  public:
    std::uint8_t K;
    // int totalcost;

    /**
     * @brief Construct a new FDGainMgr object
     *
     * @param H
     * @param K
     */
    explicit FDGainMgr(SimpleNetlist &H, std::uint8_t K = 2);

    /**
     * @brief
     *
     * @param part_info
     */
    auto init(const PartInfo &part_info) -> int;

    /**
     * @brief Get the pmax object
     *
     * @return size_t
     */
    auto get_pmax() const -> size_t { return this->pmax; }

    /**
     * @brief
     *
     * @param toPart
     * @return true
     * @return false
     */
    auto is_empty_togo(std::uint8_t toPart) const -> bool {
        return this->gainbucket[toPart]->is_empty();
    }

    /**
     * @brief
     *
     * @return true
     * @return false
     */
    auto is_empty() const -> bool {
        for (auto k = 0u; k < this->K; ++k) {
            if (!this->gainbucket[k]->is_empty()) {
                return false;
            }
        }
        return true;
    }

    /**
     * @brief
     *
     * @param part
     * @return std::tuple<MoveInfoV, int>
     */
    auto select(const std::vector<std::uint8_t> &part)
        -> std::tuple<MoveInfoV, int>;

    /**
     * @brief
     *
     * @param toPart
     * @return std::tuple<size_t, int>
     */
    auto select_togo(std::uint8_t toPart) -> std::tuple<node_t, int>;

    /**
     * @brief
     *
     * @param part
     * @param move_info_v
     */
    auto update_move(PartInfo &part_info, const MoveInfoV &move_info_v) -> void;

  private:
    /**
     * @brief
     *
     * @param part_info
     * @param move_info
     */
    auto update_move_2pin_net(PartInfo &part_info, const MoveInfo &move_info)
        -> void;

    /**
     * @brief
     *
     * @param part_info
     * @param move_info
     */
    auto update_move_general_net(PartInfo &part_info, const MoveInfo &move_info)
        -> void;
};

#endif
