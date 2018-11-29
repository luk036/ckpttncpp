#include <ckpttncpp/FMKWayConstrMgr.hpp>
#include <ckpttncpp/netlist.hpp> // import Netlist

/**
 * @brief 
 * 
 * @param part 
 */
auto FMKWayConstrMgr::init(const std::vector<size_t> &part) -> void
{
    auto totalweight = 0;
    for (auto &v : this->H.module_list) {
        // auto weight = this->H.G.nodes[v].get('weight', 1);
        weight = this->H.get_module_weight(v);
        this->diff[part[v]] += weight;
        totalweight += weight;
    }
    auto totalweightK = totalweight * 2. / this->K;
    this->upperbound = std::round(totalweightK * this->ratio);
    this->lowerbound = std::round(totalweightK - this->upperbound);
    for (auto k = 0u; k < this->K; ++k) {
        this->illegal[k] = (this->diff[k] < this->lowerbound ||
                            this->diff[k] > this->upperbound);
    }
}

/**
 * @brief 
 * 
 * @param move_info_v 
 * @return size_t 
 */
auto FMKWayConstrMgr::check_legal(const MoveInfoV& move_info_v) -> size_t
{
    auto const& [fromPart, toPart, v] = move_info_v;
    this->weight = this->H.get_module_weight(v);
    auto diffTo = this->diff[toPart] + this->weight;
    auto diffFrom = this->diff[fromPart] - this->weight;
    if (diffTo > this->upperbound || diffFrom < this->lowerbound) {
        return 0; // not ok, don't move
    }
    if (diffFrom > this->upperbound || diffTo < this->lowerbound) {
        return 1; // get better, but still illegal
    }
    this->illegal[fromPart] = this->illegal[toPart] = false;
    for (auto b : this->illegal) {
        if (b) {
            return 1; // get better, but still illegal
        }
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
auto FMKWayConstrMgr::check_constraints(const MoveInfoV& move_info_v) -> bool 
{
    auto const& [fromPart, toPart, v] = move_info_v;
    this->weight = this->H.get_module_weight(v);
    auto diffTo = this->diff[toPart] + this->weight;
    auto diffFrom = this->diff[fromPart] - this->weight;
    return diffTo <= this->upperbound && diffFrom >= this->lowerbound;
}


/**
 * @brief 
 * 
 * @param move_info_v 
 */
auto FMKWayConstrMgr::update_move(const MoveInfoV& move_info_v) -> void {
    auto const& [fromPart, toPart, v] = move_info_v;
    this->diff[toPart] += this->weight;
    this->diff[fromPart] -= this->weight;
}
