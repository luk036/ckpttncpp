#ifndef _HOME_UBUNTU_GITHUB_CKPTTNCPP_FMCONSTRMGR_HPP
#define _HOME_UBUNTU_GITHUB_CKPTTNCPP_FMCONSTRMGR_HPP 1

#include <vector>
#include <algorithm>
// Check if (the move of v can satisfied, makebetter, or notsatisfied

class Netlist;
class MoveInfoV;

class FMConstrMgr {
  protected:
    Netlist &H;
    double ratio;
    size_t K;
    std::vector<size_t> diff;
    size_t lowerbound;
    size_t weight; // cache value

    /**
     * @brief Construct a new FMConstrMgr object
     * 
     * @param H 
     * @param K 
     * @param ratio 
     */
    FMConstrMgr(Netlist &H, double ratio, size_t K=2)
        : H{H}, ratio{ratio}, K{K}, diff(K, 0),
          lowerbound{0}, weight{0} {}

  public:
    /**
     * @brief 
     * 
     * @param part 
     */
    auto init(const std::vector<size_t> &part) -> void;

    /**
     * @brief 
     * 
     * @param move_info_v 
     * @return size_t 
     */
    auto check_legal(const MoveInfoV& move_info_v) -> size_t;

    /**
     * @brief 
     * 
     * @param move_info_v 
     * @return true 
     * @return false 
     */
    auto check_constraints(const MoveInfoV& move_info_v) -> bool;

    /**
     * @brief 
     * 
     * @param move_info_v 
     */
    auto update_move(const MoveInfoV& move_info_v) -> void;
};

#endif