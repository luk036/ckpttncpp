#ifndef CKPTTNCPP_FMGAINMGR_HPP
#define CKPTTNCPP_FMGAINMGR_HPP 1

#include "bpqueue.hpp" // import bpqueue
#include "dllist.hpp"  // import dllink
#include "netlist.hpp" // import Netlist
#include <cassert>
#include <cinttypes>
#include <iterator>
#include <memory>
#include <type_traits>

/**
 * @brief
 *
 * @tparam GainCalc
 * @tparam Derived
 */
template <typename GainCalc, class Derived> class FMGainMgr {
    Derived &self = *static_cast<Derived *>(this);
    using index_t = typename SimpleNetlist::index_t;

  protected:
    SimpleNetlist &H;
    GainCalc gainCalc;
    int pmax;
    // index_t num[2];
    dllink<index_t> waitinglist{};
    std::vector<bpqueue<int>> gainbucket;

  public:
    std::uint8_t K;
    // int totalcost;

    /**
     * @brief Construct a new FMGainMgr object
     *
     * @param H
     * @param K
     */
    explicit FMGainMgr(SimpleNetlist &H, std::uint8_t K);

    /**
     * @brief
     *
     * @param part
     */
    auto init(const std::vector<uint8_t> &part) -> int;

    /**
     * @brief
     *
     * @param toPart
     * @return true
     * @return false
     */
    [[nodiscard]] auto is_empty_togo(std::uint8_t toPart) const -> bool {
        return this->gainbucket[toPart].is_empty();
    }

    /**
     * @brief
     *
     * @return true
     * @return false
     */
    [[nodiscard]] auto is_empty() const -> bool {
        for (auto k = 0U; k < this->K; ++k) {
            if (!this->gainbucket[k].is_empty()) {
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
     * @return std::tuple<index_t, int>
     */
    auto select_togo(std::uint8_t toPart) -> std::tuple<index_t, int>;

    /**
     * @brief
     *
     * @param part
     * @param move_info_v
     */
    auto update_move(const std::vector<uint8_t> &part, const MoveInfoV &move_info_v)
        -> void;

  private:
    /**
     * @brief
     *
     * @param part
     * @param move_info
     */
    auto update_move_2pin_net(const std::vector<uint8_t> &part,
                              const MoveInfo &move_info) -> void;

    /**
     * @brief
     *
     * @param part
     * @param move_info
     */
    auto update_move_3pin_net(const std::vector<uint8_t> &part,
                              const MoveInfo &move_info) -> void;

    /**
     * @brief
     *
     * @param part
     * @param move_info
     */
    auto update_move_general_net(const std::vector<uint8_t> &part,
                                 const MoveInfo &move_info) -> void;
};

#endif
