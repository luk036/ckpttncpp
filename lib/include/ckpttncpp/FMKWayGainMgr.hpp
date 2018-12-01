#ifndef _HOME_UBUNTU_GITHUB_CKPTTNCPP_FMKWayGAINMGR_HPP
#define _HOME_UBUNTU_GITHUB_CKPTTNCPP_FMKWayGAINMGR_HPP 1

#include "bpqueue.hpp" // import bpqueue
#include "dllist.hpp"  // import dllink
#include "netlist.hpp" // import Netlist
#include <cassert>
#include <iterator>
#include <cinttypes>

/**
 * @brief FMKWayGainMgr
 *
 */
class FMKWayGainMgr {
  private:
    Netlist &H;
    std::uint8_t K;
    // FMKWayGainCalc gainCalc;
    size_t pmax;
    // size_t num[2];
    size_t num_modules;
    dllink waitinglist;
    std::vector<std::vector<dllink>> vertex_list;
    std::vector<std::unique_ptr<bpqueue>> gainbucket;
    std::vector<int> deltaGainV;

  public:
    /**
     * @brief Construct a new FMKWayGainMgr object
     *
     * @param H
     */
    explicit FMKWayGainMgr(Netlist &H, std::uint8_t K);

    /**
     * @brief
     *
     * @param part
     */
    auto init(const std::vector<std::uint8_t> &part) -> void;

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
     * @return true
     * @return false
     */
    auto is_empty_togo(std::uint8_t toPart) const -> bool {
        return this->gainbucket[toPart]->is_empty();
    }

    /**
     * @brief 
     * 
     * @param part 
     * @return std::tuple<MoveInfoV, int> 
     */
    auto select(const std::vector<std::uint8_t> &part)
                        -> std::tuple<MoveInfoV, int> {
        auto gainmax = std::vector<int>(this->K);
        for (auto k = 0u; k < this->K; ++k) {
            gainmax[k] = this->gainbucket[k]->get_max();
        }
        auto it = std::max_element(gainmax.cbegin(), gainmax.cend());
        std::uint8_t toPart = std::distance(gainmax.cbegin(), it);
        auto &vlink = this->gainbucket[toPart]->popleft();
        this->waitinglist.append(vlink);
        node_t v = &vlink - &this->vertex_list[toPart][0];
        auto fromPart = part[v];
        auto move_info_v = MoveInfoV{fromPart, toPart, v};
        return std::tuple{std::move(move_info_v), gainmax[toPart]};
    }


    /**
     * @brief
     *
     * @return std::tuple<size_t, int>
     */
    auto select_togo(std::uint8_t toPart) -> std::tuple<size_t, int> {
        auto gainmax = this->gainbucket[toPart]->get_max();
        auto &vlink = this->gainbucket[toPart]->popleft();
        this->waitinglist.append(vlink);
        node_t v = &vlink - &this->vertex_list[toPart][0];
        return {v, gainmax};
    }

    /**
     * @brief 
     * 
     * @param part 
     * @param move_info_v 
     * @param gain 
     */
    auto update_move(const std::vector<std::uint8_t> &part,
                     const MoveInfoV& move_info_v, int gain) -> void;

  private:

    /**
     * @brief Set the key object
     *
     * @param whichPart
     * @param v
     * @param key
     */
    auto set_key(size_t whichPart, node_t v, int key) -> void {
        this->gainbucket[whichPart]->set_key(
            this->vertex_list[whichPart][v], key);
    }

    /**
     * @brief 
     * 
     * @param part 
     * @param w 
     * @param keys 
     */
    auto modify_key(const std::vector<std::uint8_t> &part,
                    node_t w, std::vector<int> &keys) -> void {
        for (auto k = 0u; k < this->K; ++k) {
            if (part[w] == k) {
                continue;
            }
            this->gainbucket[k]->modify_key(this->vertex_list[k][w], keys[k]);
        }
    }

    /**
     * @brief 
     * 
     * @param part 
     * @param move_info 
     */
    auto update_move_2pin_net(const std::vector<std::uint8_t> &part,
                              const MoveInfo& move_info) -> void;

    /**
     * @brief 
     * 
     * @param part 
     * @param move_info 
     */
    auto update_move_general_net(const std::vector<std::uint8_t> &part,
                                 const MoveInfo& move_info) -> void;
};

#endif
