#include <ckpttncpp/FMGainMgr.hpp>

/**
 * @brief Construct a new FMGainMgr object
 *
 * @param H
 * @param K
 */
template <typename GainCalc, class Derived>
FMGainMgr<GainCalc, Derived>::FMGainMgr(const SimpleNetlist& H, uint8_t K)
    : H {H}
    , pmax {H.get_max_degree()}
    , K {K}
    , gainCalc {H, K}
{
    static_assert(
        std::is_base_of<FMGainMgr<GainCalc, Derived>, Derived>::value);
    for (auto k = 0U; k != this->K; ++k)
    {
        this->gainbucket.emplace_back(-this->pmax, this->pmax);
    }
}

/**
 * @brief
 *
 * @param part
 */
template <typename GainCalc, class Derived>
int FMGainMgr<GainCalc, Derived>::init(gsl::span<const uint8_t> part)
{
    auto totalcost = this->gainCalc.init(part);
    // this->totalcost = this->gainCalc.totalcost;
    this->waitinglist.clear();
    return totalcost;
}

/**
 * @brief
 *
 * @param part
 * @return std::tuple<MoveInfoV, int>
 */
template <typename GainCalc, class Derived>
std::tuple<MoveInfoV, int> FMGainMgr<GainCalc, Derived>::select(
    gsl::span<const uint8_t> part)
{
    auto gainmax = std::vector<int>(this->K);
    for (auto k = 0U; k != this->K; ++k)
    {
        gainmax[k] = this->gainbucket[k].get_max();
    }
    const auto it = std::max_element(gainmax.cbegin(), gainmax.cend());
    const uint8_t toPart = std::distance(gainmax.cbegin(), it);
    auto& vlink = this->gainbucket[toPart].popleft();
    this->waitinglist.append(vlink);
    // node_t v = &vlink - this->gainCalc.start_ptr(toPart);
    const node_t v = std::distance(this->gainCalc.start_ptr(toPart), &vlink);
    // auto move_info_v = MoveInfoV {part[v], toPart, v};
    return {{part[v], toPart, v}, gainmax[toPart]};
}

/**
 * @brief
 *
 * @param toPart
 * @return std::tuple<node_t, int>
 */
template <typename GainCalc, class Derived>
std::tuple<node_t, int> FMGainMgr<GainCalc, Derived>::select_togo(
    uint8_t toPart)
{
    const auto gainmax = this->gainbucket[toPart].get_max();
    auto& vlink = this->gainbucket[toPart].popleft();
    this->waitinglist.append(vlink);
    const node_t v = std::distance(this->gainCalc.start_ptr(toPart), &vlink);
    return {v, gainmax};
}

/**
 * @brief
 *
 * @param part
 * @param move_info_v
 * @param gain
 */
template <typename GainCalc, class Derived>
void FMGainMgr<GainCalc, Derived>::update_move(
    gsl::span<const uint8_t> part, const MoveInfoV& move_info_v)
{
    // std::fill_n(this->deltaGainV.begin(), this->K, 0);
    this->gainCalc.update_move_init();

    const auto& [fromPart, toPart, v] = move_info_v;

    for (node_t net : this->H.G[v])
    {
        const auto degree = this->H.G.degree(net);
        [[unlikely]] if (degree < 2)
        {
            continue; // does not provide any gain change when
                                   // moving
        }
        const auto move_info = MoveInfo {net, fromPart, toPart, v};
        switch (degree)
        {
            case 2:
                if (this->gainCalc.special_handle_2pin_nets)
                {
                    this->__update_move_2pin_net(part, move_info);
                }
                else
                {
                    this->__update_move_general_net(part, move_info);
                }
                break;
            case 3:
                if (this->gainCalc.special_handle_2pin_nets)
                {
                    this->__update_move_3pin_net(part, move_info);
                }
                else
                {
                    this->__update_move_general_net(part, move_info);
                }
                break;
            default:
                this->__update_move_general_net(part, move_info);
        }
    }
}

/**
 * @brief
 *
 * @param part
 * @param move_info
 */
template <typename GainCalc, class Derived>
void FMGainMgr<GainCalc, Derived>::__update_move_2pin_net(
    gsl::span<const uint8_t> part, const MoveInfo& move_info)
{
    const auto [w, deltaGainW] =
        this->gainCalc.update_move_2pin_net(part, move_info);
    self.modify_key(w, part[w], deltaGainW);
}

/**
 * @brief
 *
 * @param part
 * @param move_info
 */
template <typename GainCalc, class Derived>
void FMGainMgr<GainCalc, Derived>::__update_move_3pin_net(
    gsl::span<const uint8_t> part, const MoveInfo& move_info)
{
    const auto [IdVec, deltaGain] =
        this->gainCalc.update_move_3pin_net(part, move_info);
    const auto degree = IdVec.size();
    for (size_t index = 0U; index < degree; ++index)
    {
        const auto w = IdVec[index];
        self.modify_key(w, part[w], deltaGain[index]);
    }
}

/**
 * @brief
 *
 * @param part
 * @param move_info
 */
template <typename GainCalc, class Derived>
void FMGainMgr<GainCalc, Derived>::__update_move_general_net(
    gsl::span<const uint8_t> part, const MoveInfo& move_info)
{
    const auto [IdVec, deltaGain] =
        this->gainCalc.update_move_general_net(part, move_info);
    const auto degree = IdVec.size();
    for (size_t index = 0U; index < degree; ++index)
    {
        const auto w = IdVec[index];
        self.modify_key(w, part[w], deltaGain[index]);
    }
}

#include <ckpttncpp/FMBiGainMgr.hpp> // import FMBiGainMgr
template class FMGainMgr<FMBiGainCalc, FMBiGainMgr>;

#include <ckpttncpp/FMKWayGainMgr.hpp> // import FMKWayGainMgr
template class FMGainMgr<FMKWayGainCalc, FMKWayGainMgr>;
