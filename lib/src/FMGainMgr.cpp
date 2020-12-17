#include <ckpttncpp/FMGainMgr.hpp>
#include <memory_resource>
#include <vector>

using node_t = typename SimpleNetlist::node_t;

/**
 * @brief Construct a new FMGainMgr object
 *
 * @param[in] H
 * @param[in] K
 */
template <typename GainCalc, class Derived>
FMGainMgr<GainCalc, Derived>::FMGainMgr(const SimpleNetlist& H, std::uint8_t K)
    : H {H} // , pmax {H.get_max_degree()}
    , K {K}
    , gainCalc {H, K}
{
    static_assert(std::is_base_of<FMGainMgr<GainCalc, Derived>, Derived>::value,
        "base derived consistence");
    const auto pmax = int(H.get_max_degree());
    for (auto k = 0U; k != this->K; ++k)
    {
        this->gainbucket.emplace_back(bpqueue<node_t>(-pmax, pmax));
    }
}

/**
 * @brief
 *
 * @param[in] part
 */
template <typename GainCalc, class Derived>
int FMGainMgr<GainCalc, Derived>::init(gsl::span<const std::uint8_t> part)
{
    auto totalcost = this->gainCalc.init(part);
    // this->totalcost = this->gainCalc.totalcost;
    this->waitinglist.clear();
    return totalcost;
}

/**
 * @brief
 *
 * @param[in] part
 * @return std::tuple<MoveInfoV<node_t>, int>
 */
template <typename GainCalc, class Derived>
std::tuple<MoveInfoV<node_t>, int> FMGainMgr<GainCalc, Derived>::select(
    gsl::span<const std::uint8_t> part)
{
    auto gainmax = std::vector<int>(this->K);
    for (auto k = 0U; k != this->K; ++k)
    {
        gainmax[k] = this->gainbucket[k].get_max();
    }
    const auto it = std::max_element(gainmax.cbegin(), gainmax.cend());
    const auto toPart = std::uint8_t(std::distance(gainmax.cbegin(), it));
    auto& vlink = this->gainbucket[toPart].popleft();
    this->waitinglist.append(vlink);
    // node_t v = &vlink - this->gainCalc.start_ptr(toPart);
    const auto v = vlink.data.first;
    // const auto v =
    //     node_t(std::distance(this->gainCalc.start_ptr(toPart), &vlink));
    // auto move_info_v = MoveInfoV<node_t> {v, part[v], toPart};
    return {{v, part[v], toPart}, gainmax[toPart]};
}

/**
 * @brief
 *
 * @param[in] toPart
 * @return std::tuple<node_t, int>
 */
template <typename GainCalc, class Derived>
std::tuple<node_t, int> FMGainMgr<GainCalc, Derived>::select_togo(
    std::uint8_t toPart)
{
    const auto gainmax = this->gainbucket[toPart].get_max();
    auto& vlink = this->gainbucket[toPart].popleft();
    this->waitinglist.append(vlink);
    const auto v = vlink.data.first;
    // const auto v =
    //     node_t(std::distance(this->gainCalc.start_ptr(toPart), &vlink));
    return {v, gainmax};
}

/**
 * @brief
 *
 * @param[in] part
 * @param[in] move_info_v
 * @param[in] gain
 */
template <typename GainCalc, class Derived>
void FMGainMgr<GainCalc, Derived>::update_move(
    gsl::span<const std::uint8_t> part, const MoveInfoV<node_t>& move_info_v)
{
    // std::fill_n(this->deltaGainV.begin(), this->K, 0);
    this->gainCalc.update_move_init();

    // const auto& [v, fromPart, toPart] = move_info_v;
    const auto& v = move_info_v.v;
    for (const node_t& net : this->H.G[move_info_v.v])
    {
        const auto degree = this->H.G.degree(net);
        if (degree < 2) // [[unlikely]]
        {
            continue; // does not provide any gain change when
                      // moving
        }
        const auto move_info = MoveInfo<node_t> {
            net, v, move_info_v.fromPart, move_info_v.toPart};
        if (!this->gainCalc.special_handle_2pin_nets)
        {
            this->gainCalc.init_IdVec(v, net);
            this->_update_move_general_net(part, move_info);
            continue;
        }
        if (degree == 2)
        {
            this->_update_move_2pin_net(part, move_info);
        }
        else
        {
            this->gainCalc.init_IdVec(v, net);
            if (degree == 3)
            {
                this->_update_move_3pin_net(part, move_info);
            }
            else
            {
                this->_update_move_general_net(part, move_info);
            }
        }
    }
}

/**
 * @brief
 *
 * @param[in] part
 * @param[in] move_info
 */
template <typename GainCalc, class Derived>
void FMGainMgr<GainCalc, Derived>::_update_move_2pin_net(
    gsl::span<const std::uint8_t> part, const MoveInfo<node_t>& move_info)
{
    // const auto [w, deltaGainW] =
    //     this->gainCalc.update_move_2pin_net(part, move_info);
    const auto w =
        this->gainCalc.update_move_2pin_net(part, move_info);
    self.modify_key(w, part[w], this->gainCalc.deltaGainW);
}

/**
 * @brief
 *
 * @param[in] part
 * @param[in] move_info
 */
template <typename GainCalc, class Derived>
void FMGainMgr<GainCalc, Derived>::_update_move_3pin_net(
    gsl::span<const std::uint8_t> part, const MoveInfo<node_t>& move_info)
{
    // std::byte StackBuf[4096];
    // std::pmr::monotonic_buffer_resource rsrc(StackBuf, sizeof StackBuf);
    // auto IdVec = std::pmr::vector<node_t>(&rsrc);

    const auto deltaGain =
        this->gainCalc.update_move_3pin_net(part, move_info);
    const auto degree = this->gainCalc.IdVec.size();
    for (size_t index = 0U; index != degree; ++index)
    {
        const auto& w = this->gainCalc.IdVec[index];
        self.modify_key(w, part[w], deltaGain[index]);
    }
}

/**
 * @brief
 *
 * @param[in] part
 * @param[in] move_info
 */
template <typename GainCalc, class Derived>
void FMGainMgr<GainCalc, Derived>::_update_move_general_net(
    gsl::span<const std::uint8_t> part, const MoveInfo<node_t>& move_info)
{
    // std::byte StackBuf[2048];
    // std::pmr::monotonic_buffer_resource rsrc(StackBuf, sizeof StackBuf);
    // auto IdVec = std::pmr::vector<node_t>(&rsrc);

    // const auto [IdVec, deltaGain] =
    const auto deltaGain =
        this->gainCalc.update_move_general_net(part, move_info);
    // const auto& IdVec = std::get<0>(infoW);
    // const auto& deltaGain = std::get<1>(infoW);
    const auto degree = this->gainCalc.IdVec.size();
    for (size_t index = 0U; index != degree; ++index)
    {
        const auto& w = this->gainCalc.IdVec[index];
        self.modify_key(w, part[w], deltaGain[index]);
    }
}

#include <ckpttncpp/FMBiGainMgr.hpp> // import FMBiGainMgr
template class FMGainMgr<FMBiGainCalc, FMBiGainMgr>;

#include <ckpttncpp/FMKWayGainMgr.hpp> // import FMKWayGainMgr
template class FMGainMgr<FMKWayGainCalc, FMKWayGainMgr>;
