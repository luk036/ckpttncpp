#include <ckpttncpp/FMConstrMgr.hpp>
#include <ckpttncpp/netlist.hpp> // import Netlist

/**
 * @brief
 *
 * @param part
 */
void FMConstrMgr::init(gsl::span<const uint8_t> part)
{
    std::fill_n(this->diff.begin(), this->K, 0);
    for (auto v : this->H.modules)
    {
        auto weight_v = this->H.get_module_weight(v);
        this->diff[part[v]] += weight_v;
    }
}

/**
 * @brief
 *
 * @param move_info_v
 * @return size_t
 */
size_t FMConstrMgr::check_legal(const MoveInfoV& move_info_v)
{
    auto [fromPart, toPart, v] = move_info_v;

    this->weight = this->H.get_module_weight(v);
    auto diffFrom = this->diff[fromPart] - this->weight;
    if (diffFrom < this->lowerbound)
    {
        return 0; // not ok, don't move
    }
    auto diffTo = this->diff[toPart] + this->weight;
    if (diffTo < this->lowerbound)
    {
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
bool FMConstrMgr::check_constraints(const MoveInfoV& move_info_v)
{
    auto [fromPart, toPart, v] = move_info_v;

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
void FMConstrMgr::update_move(const MoveInfoV& move_info_v)
{
    // auto [fromPart, toPart, v] = move_info_v;
    this->diff[move_info_v.toPart] += this->weight;
    this->diff[move_info_v.fromPart] -= this->weight;
}
