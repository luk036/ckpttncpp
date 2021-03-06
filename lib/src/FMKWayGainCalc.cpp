#include <ckpttncpp/FMKWayGainCalc.hpp>
#include <ckpttncpp/FMPmrConfig.hpp>
// #include <range/v3/view/enumerate.hpp>
// #include <range/v3/view/zip.hpp>
// #include <range/v3/view/remove_if.hpp>
#include <vector>

using namespace ranges;

/**
 * @brief
 *
 * @param[in] net
 * @param[in] part
 * @param[in] vertex_list
 */
void FMKWayGainCalc::_init_gain(
    const node_t& net, gsl::span<const std::uint8_t> part)
{
    const auto degree = this->H.G.degree(net);
    if (degree < 2 || degree > FM_MAX_DEGREE) // [[unlikely]]
    {
        return; // does not provide any gain when moving
    }
    if (!special_handle_2pin_nets)
    {
        this->_init_gain_general_net(net, part);
        return;
    }
    switch (degree)
    {
        case 2:
            this->_init_gain_2pin_net(net, part);
            break;
        case 3:
            this->_init_gain_3pin_net(net, part);
            break;
        default:
            this->_init_gain_general_net(net, part);
    }
}

/**
 * @brief
 *
 * @param[in] net
 * @param[in] part
 */
void FMKWayGainCalc::_init_gain_2pin_net(
    const node_t& net, gsl::span<const std::uint8_t> part)
{
    auto netCur = this->H.G[net].begin();
    const auto w = *netCur;
    const auto v = *++netCur;
    const auto part_w = part[w];
    const auto part_v = part[v];
    const auto weight = this->H.get_net_weight(net);
    if (part_v == part_w)
    {
        // this->_modify_gain(w, part_v, -weight);
        // this->_modify_gain(v, part_v, -weight);
        this->_modify_gain_va(-weight, part_v, w, v);
    }
    else
    {
        this->totalcost += weight;
        this->vertex_list[part_v][w].data.second += weight;
        this->vertex_list[part_w][v].data.second += weight;
    }
}

/**
 * @brief
 *
 * @param[in] net
 * @param[in] part
 */
void FMKWayGainCalc::_init_gain_3pin_net(
    const node_t& net, gsl::span<const std::uint8_t> part)
{
    auto netCur = this->H.G[net].begin();
    const auto w = *netCur;
    const auto v = *++netCur;
    const auto u = *++netCur;
    const auto part_w = part[w];
    const auto part_v = part[v];
    const auto part_u = part[u];
    const auto weight = this->H.get_net_weight(net);
    auto a = w;
    auto b = v;
    auto c = u;

    if (part_u == part_v)
    {
        if (part_w == part_v)
        {
            // this->_modify_gain(u, part_v, -weight);
            // this->_modify_gain(v, part_v, -weight);
            // this->_modify_gain(w, part_v, -weight);
            this->_modify_gain_va(-weight, part_v, u, v, w);
            return;
        }
    }
    else if (part_w == part_v)
    {
        a = u, b = w, c = v;
    }
    else if (part_w == part_u)
    {
        a = v, b = u, c = w;
    }
    else
    {
        this->totalcost += 2 * weight;
        this->_modify_vertex_va(weight, part_v, u, w);
        this->_modify_vertex_va(weight, part_w, u, v);
        this->_modify_vertex_va(weight, part_u, v, w);
        return;
    }

    // for (const auto& e : {b, c})
    // {
    //     this->_modify_gain(e, part[b], -weight);
    //     this->vertex_list[part[a]][e].data.second += weight;
    // }
    this->_modify_gain_va(-weight, part[b], b, c);
    this->_modify_vertex_va(weight, part[a], b, c);
    this->_modify_vertex_va(weight, part[b], a);

    this->totalcost += weight;
}

/**
 * @brief
 *
 * @param[in] net
 * @param[in] part
 */
void FMKWayGainCalc::_init_gain_general_net(
    const node_t& net, gsl::span<const std::uint8_t> part)
{
    std::byte StackBuf[2048];
    FMPmr::monotonic_buffer_resource rsrc(StackBuf, sizeof StackBuf);
    auto num = FMPmr::vector<std::uint8_t>(this->K, 0, &rsrc);
    // auto IdVec = FMPmr::vector<node_t>(&rsrc);

    for (const auto& w : this->H.G[net])
    {
        num[part[w]] += 1;
        // IdVec.push_back(w);
    }
    const auto weight = this->H.get_net_weight(net);
    for (const auto& c : num)
    {
        if (c > 0)
        {
            this->totalcost += weight;
        }
    }
    this->totalcost -= weight;

    // for (const auto& [k, c] : views::enumerate(num))
    auto k = 0U;
    for (const auto& c : num)
    {
        if (c == 0)
        {
            for (const auto& w : this->H.G[net])
            {
                vertex_list[k][w].data.second -= weight;
            }
        }
        else if (c == 1)
        {
            for (const auto& w : this->H.G[net])
            {
                if (part[w] == k)
                {
                    this->_modify_gain(w, part[w], weight);
                    break;
                }
            }
        }
        ++k;
    }
}

/**
 * @brief
 *
 * @param[in] part
 * @param[in] move_info
 * @param[out] w
 * @return ret_2pin_info
 */
auto FMKWayGainCalc::update_move_2pin_net(gsl::span<const std::uint8_t> part,
    const MoveInfo<node_t>& move_info) -> FMKWayGainCalc::node_t
{
    // const auto& [net, v, fromPart, toPart] = move_info;
    if (part[move_info.v] != move_info.fromPart)
    {
        exit(1);
    }

    auto weight = this->H.get_net_weight(move_info.net);
    // auto deltaGainW = std::vector<int>(this->K, 0);
    auto netCur = this->H.G[move_info.net].begin();
    auto w = (*netCur != move_info.v) ? *netCur : *++netCur;
    std::fill(this->deltaGainW.begin(), this->deltaGainW.end(), 0);

    // #pragma unroll
    for (const auto& l : {move_info.fromPart, move_info.toPart})
    {
        if (part[w] == l)
        {
            // for (auto i = 0U; i != deltaGainW.size(); ++i)
            // {
            //     deltaGainW[i] += weight;
            //     deltaGainV[i] += weight;
            // }
            for (auto& dGW : deltaGainW)
            {
                dGW += weight;
            }
            for (auto& dGV : deltaGainV)
            {
                dGV += weight;
            }
        }
        deltaGainW[l] -= weight;
        weight = -weight;
    }
    return w;
}

/**
 * @brief
 *
 * @param[in] part
 * @param[in] move_info
 * @return ret_info
 */
void FMKWayGainCalc::init_IdVec(const node_t& v, const node_t& net)
{
    this->IdVec.clear();
    for (const auto& w : this->H.G[net])
    {
        if (w == v)
        {
            continue;
        }
        this->IdVec.push_back(w);
    }
}

/**
 * @brief
 *
 * @param[in] part
 * @param[in] move_info
 * @return ret_info
 */
auto FMKWayGainCalc::update_move_3pin_net(gsl::span<const std::uint8_t> part,
    const MoveInfo<node_t>& move_info) -> FMKWayGainCalc::ret_info
{
    // const auto& [net, v, fromPart, toPart] = move_info;
    // auto IdVec = std::vector<node_t> {};
    // for (const auto& w : this->H.G[move_info.net])
    // {
    //     if (w == move_info.v)
    //     {
    //         continue;
    //     }
    //     this->IdVec.push_back(w);
    // }
    const auto degree = this->IdVec.size();
    auto deltaGain =
        std::vector<std::vector<int>>(degree, std::vector<int>(this->K, 0));
    auto weight = this->H.get_net_weight(move_info.net);
    const auto part_w = part[this->IdVec[0]];
    const auto part_u = part[this->IdVec[1]];
    auto l = move_info.fromPart;
    auto u = move_info.toPart;

    if (part_w == part_u)
    {
        // #pragma unroll
        for (auto i = 0; i != 2; ++i)
        {
            if (part_w != l)
            {
                deltaGain[0][l] -= weight;
                deltaGain[1][l] -= weight;
                if (part_w == u)
                {
                    for (auto& dGV : this->deltaGainV)
                    {
                        dGV -= weight;
                    }
                }
            }
            weight = -weight;
            std::swap(l, u);
        }
        return deltaGain;
    }

    // #pragma unroll
    for (auto i = 0; i != 2; ++i)
    {
        if (part_w == l)
        {
            for (auto& dG0 : deltaGain[0])
            {
                dG0 += weight;
            }
        }
        else if (part_u == l)
        {
            for (auto& dG1 : deltaGain[1])
            {
                dG1 += weight;
            }
        }
        else
        {
            deltaGain[0][l] -= weight;
            deltaGain[1][l] -= weight;
            if (part_w == u || part_u == u)
            {
                for (auto& dGV : this->deltaGainV)
                {
                    dGV -= weight;
                }
            }
        }
        weight = -weight;
        std::swap(l, u);
    }
    return deltaGain;
    // return this->update_move_general_net(part, move_info);
}

/**
 * @brief
 *
 * @param[in] part
 * @param[in] move_info
 * @return ret_info
 */
auto FMKWayGainCalc::update_move_general_net(gsl::span<const std::uint8_t> part,
    const MoveInfo<node_t>& move_info) -> FMKWayGainCalc::ret_info
{
    // const auto& [net, v, fromPart, toPart] = move_info;
    std::byte StackBuf[FM_MAX_NUM_PARTITIONS];
    FMPmr::monotonic_buffer_resource rsrc(StackBuf, sizeof StackBuf);
    auto num = FMPmr::vector<std::uint8_t>(this->K, 0, &rsrc);

    // auto IdVec = std::vector<node_t> {};
    // for (const auto& w : this->H.G[move_info.net])
    // {
    //     if (w == move_info.v)
    //     {
    //         continue;
    //     }
    //     num[part[w]] += 1;
    //     IdVec.push_back(w);
    // }
    for (const auto& w : this->IdVec)
    {
        num[part[w]] += 1;
    }
    const auto degree = IdVec.size();
    auto deltaGain =
        std::vector<std::vector<int>>(degree, std::vector<int>(this->K, 0));
    auto weight = this->H.get_net_weight(move_info.net);

    auto l = move_info.fromPart;
    auto u = move_info.toPart;

    // #pragma unroll
    for (auto i = 0; i != 2; ++i)
    {
        if (num[l] == 0)
        {
            for (size_t index = 0U; index != degree; ++index)
            {
                deltaGain[index][l] -= weight;
            }
            if (num[u] > 0)
            {
                for (auto& dGV : this->deltaGainV)
                {
                    dGV -= weight;
                }
            }
        }
        else if (num[l] == 1)
        {
            for (size_t index = 0U; index != degree; ++index)
            {
                if (part[this->IdVec[index]] == l)
                {
                    for (auto& dG : deltaGain[index])
                    {
                        dG += weight;
                    }
                    break;
                }
            }
        }
        weight = -weight;
        std::swap(l, u);
    };
    return deltaGain;
}
