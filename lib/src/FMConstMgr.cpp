#include <ckpttncpp/FMConstrMgr.hpp>
#include <ckpttncpp/netlist.hpp> // import Netlist

/**
 * @brief
 *
 * @param part
 */
void FMConstrMgr::init(const std::vector<uint8_t>& part)
{
    std::fill_n(this->diff.begin(), this->K, 0);
    for (size_t i_v = 0U; i_v < this->H.number_of_modules(); ++i_v)
    {
        auto weight_v = this->H.get_module_weight_by_id(i_v);
        this->diff[part[i_v]] += weight_v;
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
    auto&& [fromPart, toPart, i_v] = move_info_v;

    this->weight = this->H.get_module_weight_by_id(i_v);
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
    auto&& [fromPart, toPart, i_v] = move_info_v;

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
void FMConstrMgr::update_move(const MoveInfoV& move_info_v)
{
    auto&& [fromPart, toPart, i_v] = move_info_v;
    this->diff[toPart] += this->weight;
    this->diff[fromPart] -= this->weight;
}
