#ifndef _HOME_UBUNTU_GITHUB_CKPTTNCPP_FMKWayCONSTRMGR_HPP
#define _HOME_UBUNTU_GITHUB_CKPTTNCPP_FMKWayCONSTRMGR_HPP 1

#include <vector>
#include <algorithm>
// Check if (the move of v can satisfied, makebetter, or notsatisfied

class Netlist;
class MoveInfoV;

class FMKWayConstrMgr {
  private:
    Netlist &H;
    size_t K;
    double ratio;
    std::vector<size_t> diff;
    std::vector<bool> illegal;
    // size_t upperbound;
    size_t lowerbound;
    size_t weight; // cache value

  public:
    /**
     * @brief Construct a new FMKWayConstrMgr object
     * 
     * @param H 
     * @param K 
     * @param ratio 
     */
    FMKWayConstrMgr(Netlist &H, size_t K, double ratio)
        : H{H}, K{K}, ratio{ratio}, diff(K, 0),
          illegal(K, true), lowerbound{0}, weight{0} {}

    auto select_togo() const -> size_t {
        auto it = std::min_element(this->diff.cbegin(), this->diff.cend());
        return std::distance(this->diff.cbegin(), it);
    }

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