#include <ckpttncpp/FMKWayConstrMgr.hpp>
#include <algorithm>

/*!
 * @brief
 *
 * @param move_info_v
 * @return size_t
 */
size_t FMKWayConstrMgr::check_legal(const MoveInfoV& move_info_v)
{
    const auto status = FMConstrMgr::check_legal(move_info_v);
    if (status != 2)
    {
        return status;
    }
    // auto [fromPart, toPart, _] = move_info_v;
    this->illegal[move_info_v.fromPart] = false;
    this->illegal[move_info_v.toPart] = false;
    if (std::any_of(this->illegal.cbegin(),
                    this->illegal.cend(), [](bool elm) { return elm; }))
    {
        return 1; // get better, but still illegal
    }

    // for (auto&& b : this->illegal)
    // {
    //     if (b)
    //     {
    //         return 1; // get better, but still illegal
    //     }
    // }
    return 2; // all satisfied
}
