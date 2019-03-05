#include <ckpttncpp/FMConstrMgr.hpp>
#include <ckpttncpp/netlist.hpp> // import Netlist

/**
 * @brief
 *
 * @param part
 */
auto FMConstrMgr::init(const std::vector<std::uint8_t> &part) -> void {
    std::fill_n(this->diff.begin(), this->K, 0);
    for (auto i_v = 0U; i_v < this->H.number_of_modules(); ++i_v) {
        auto weight = this->H.get_module_weight_by_id(i_v);
        this->diff[part[i_v]] += weight;
    }
}

/**
 * @brief
 *
 * @param move_info_v
 * @return size_t
 */
auto FMConstrMgr::check_legal(const MoveInfoV &move_info_v) -> size_t {
    auto const &[fromPart, toPart, i_v] = move_info_v;
    this->weight = this->H.get_module_weight_by_id(i_v);
    auto diffFrom = this->diff[fromPart] - this->weight;
    if (diffFrom < this->lowerbound) {
        return 0; // not ok, don't move
    }
    auto diffTo = this->diff[toPart] + this->weight;
    if (diffTo < this->lowerbound) {
        return 1; // get better, but still illegal
    }
    return 2; // all satisfied
}

/**
 * @brief
 *
 * @param move_info_v
 * @return true
 * @return false
 */
auto FMConstrMgr::check_constraints(const MoveInfoV &move_info_v) -> bool {
    auto const &[fromPart, toPart, i_v] = move_info_v;
    this->weight = this->H.get_module_weight_by_id(i_v);
    // auto diffTo = this->diff[toPart] + this->weight;
    auto diffFrom = this->diff[fromPart] - this->weight;
    return diffFrom >= this->lowerbound;
}

/**
 * @brief
 *
 * @param move_info_v
 */
auto FMConstrMgr::update_move(const MoveInfoV &move_info_v) -> void {
    auto const &[fromPart, toPart, i_v] = move_info_v;
    this->diff[toPart] += this->weight;
    this->diff[fromPart] -= this->weight;
}
