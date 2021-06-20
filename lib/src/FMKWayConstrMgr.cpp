#include <algorithm> // import std::any_of()
#include <ckpttncpp/FMKWayConstrMgr.hpp>
#include <functional> // import std::identity
#include <range/v3/algorithm/any_of.hpp>

/**
 * @brief identity function object (coming in C++20)
 *
 */
struct identity
{
    template <class T>
    constexpr auto operator()(T&& t) const noexcept -> T&&
    {
        return std::forward<T>(t);
    }
};

/*!
 * @brief
 *
 * @param[in] move_info_v
 * @return LegalCheck
 */
auto FMKWayConstrMgr::check_legal(const MoveInfoV<node_t>& move_info_v)
    -> LegalCheck
{
    const auto status = FMConstrMgr::check_legal(move_info_v);
    if (status != LegalCheck::allsatisfied)
    {
        return status;
    }
    this->illegal[move_info_v.fromPart] = false;
    this->illegal[move_info_v.toPart] = false;
    if (ranges::any_of(this->illegal, identity {}))
    {
        return LegalCheck::getbetter; // get better, but still illegal
    }
    return LegalCheck::allsatisfied; // all satisfied
}
