#include <ckpttncpp/FMConstrMgr.hpp>
#include <ckpttncpp/netlist.hpp> // import Netlist

/**
 * @brief
 *
 * @param part
 */
auto FMConstrMgr::init(const std::vector<std::uint8_t> &part) -> void {
    auto totalweight = 0;
    for (auto &v : this->H.module_list) {
        // auto weight = this->H.G.nodes[v].get('weight', 1);
        weight = this->H.get_module_weight(v);
        this->diff[part[v]] += weight;
        totalweight += weight;
    }
    auto totalweightK = totalweight * (2. / this->K);
    this->lowerbound = std::round(totalweightK * this->ratio);
}

/**
 * @brief
 *
 * @param move_info_v
 * @return size_t
 */
auto FMConstrMgr::check_legal(const MoveInfoV &move_info_v) -> size_t {
    auto const &[fromPart, toPart, v] = move_info_v;
    this->weight = this->H.get_module_weight(v);
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
    auto const &[fromPart, toPart, v] = move_info_v;
    this->weight = this->H.get_module_weight(v);
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
    auto const &[fromPart, toPart, v] = move_info_v;
    this->diff[toPart] += this->weight;
    this->diff[fromPart] -= this->weight;
}
