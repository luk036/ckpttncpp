#ifndef _HOME_UBUNTU_GITHUB_CKPTTNCPP_FMKWayGAINMGR_HPP
#define _HOME_UBUNTU_GITHUB_CKPTTNCPP_FMKWayGAINMGR_HPP 1

#include "bpqueue.hpp" // import bpqueue
#include "dllist.hpp"  // import dllink
#include "netlist.hpp" // import Netlist
#include <cassert>
#include <iterator>

/**
 * @brief FMKWayGainMgr
 *
 */
class FMKWayGainMgr {
  private:
    Netlist &H;
    size_t K;
    // FMKWayGainCalc gainCalc;
    size_t pmax;
    std::vector<std::unique_ptr<bpqueue>> gainbucket;
    // size_t num[2];
    size_t num_modules;
    std::vector<std::vector<dllink>> vertex_list;
    dllink waitinglist;
    std::vector<int> deltaGainV;

  public:
    /**
     * @brief Construct a new FMKWayGainMgr object
     *
     * @param H
     */
    explicit FMKWayGainMgr(Netlist &H, size_t K);

    /**
     * @brief
     *
     * @param part
     */
    auto init(std::vector<size_t> &part) -> void;

    /**
     * @brief
     *
     * @return true
     * @return false
     */
    auto is_empty(size_t toPart) const -> bool {
        return this->gainbucket[toPart]->is_empty();
    }

    /**
     * @brief
     *
     * @return std::tuple<size_t, int>
     */
    auto select_togo(size_t toPart) -> std::tuple<size_t, int> {
        auto gainmax = this->gainbucket[toPart]->get_max();
        auto &vlink = this->gainbucket[toPart]->popleft();
        this->waitinglist.append(vlink);
        return {&vlink - &this->vertex_list[toPart][0], gainmax};
    }

    /**
     * @brief 
     * 
     * @param part 
     * @param move_info_v 
     * @param gain 
     */
    auto update_move(std::vector<size_t> &part,
                     const MoveInfoV& move_info_v, int gain) -> void;

  private:

    /**
     * @brief Set the key object
     *
     * @param whichPart
     * @param v
     * @param key
     */
    auto set_key(size_t whichPart, size_t v, int key) -> void {
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
    auto modify_key(std::vector<size_t> &part,
                    size_t w, std::vector<int> &keys) -> void {
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
    auto update_move_2pin_net(std::vector<size_t> &part,
                              const MoveInfo& move_info) -> void;

    /**
     * @brief 
     * 
     * @param part 
     * @param move_info 
     */
    auto update_move_general_net(std::vector<size_t> &part,
                                 const MoveInfo& move_info) -> void;
};

#endif
