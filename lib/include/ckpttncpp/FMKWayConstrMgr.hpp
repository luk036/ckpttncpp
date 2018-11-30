#ifndef _HOME_UBUNTU_GITHUB_CKPTTNCPP_FMKWayCONSTRMGR_HPP
#define _HOME_UBUNTU_GITHUB_CKPTTNCPP_FMKWayCONSTRMGR_HPP 1

#include <vector>
#include <algorithm>
#include "FMConstrMgr.hpp"
// Check if (the move of v can satisfied, makebetter, or notsatisfied

class Netlist;
class MoveInfoV;

class FMKWayConstrMgr : public FMConstrMgr {
  private:
    std::vector<bool> illegal;

  public:
    /**
     * @brief Construct a new FMKWayConstrMgr object
     * 
     * @param H 
     * @param K 
     * @param ratio 
     */
    FMKWayConstrMgr(Netlist &H, double ratio, size_t K)
        : FMConstrMgr(H, ratio, K),
          illegal(K, true) {}

    /**
     * @brief 
     * 
     * @return size_t 
     */
    auto select_togo() const -> size_t {
        auto it = std::min_element(this->diff.cbegin(), this->diff.cend());
        return std::distance(this->diff.cbegin(), it);
    }

    /**
     * @brief 
     * 
     * @param part 
     */
    auto init(const std::vector<size_t> &part) -> void {
        FMConstrMgr::init(part);
        for (auto k = 0u; k < this->K; ++k) {
            this->illegal[k] = (this->diff[k] < this->lowerbound);
        }
    }

    /**
     * @brief 
     * 
     * @param move_info_v 
     * @return size_t 
     */
    auto check_legal(const MoveInfoV& move_info_v) -> size_t {
        auto status = FMConstrMgr::check_legal(move_info_v);
        if (status != 2) {
            return status;
        }
        auto const &[fromPart, toPart, v] = move_info_v;
        this->illegal[fromPart] = this->illegal[toPart] = false;
        for (auto b : this->illegal) {
            if (b) {
                return 1; // get better, but still illegal
            }
        }
        return 2; // all satisfied
    }
};

#endif