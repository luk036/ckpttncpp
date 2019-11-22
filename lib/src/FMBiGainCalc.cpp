#include <ckpttncpp/FMBiGainCalc.hpp>

/**
 * @brief
 *
 * @param net
 * @param part
 */
void FMBiGainCalc::__init_gain( //
    node_t net, gsl::span<const uint8_t> part)
{
    auto degree = this->H.G.degree(net);
    [[unlikely]] if (degree < 2)
    {
        return; // does not provide any gain when moving
    }
    switch (degree)
    {
        case 2:
            if (special_handle_2pin_nets)
            {
                this->__init_gain_2pin_net(net, part);
            }
            else
            {
                this->__init_gain_general_net(net, part);
            }
            break;
        case 3:
            if (special_handle_2pin_nets)
            {
                this->__init_gain_3pin_net(net, part);
            }
            else
            {
                this->__init_gain_general_net(net, part);
            }
            break;
        default:
            this->__init_gain_general_net(net, part);
    }
}

/**
 * @brief
 *
 * @param net
 * @param part
 */
void FMBiGainCalc::__init_gain_2pin_net( //
    node_t net, gsl::span<const uint8_t> part)
{
    auto netCur = this->H.G[net].begin();
    auto w = *netCur;
    auto v = *++netCur;

    auto weight = this->H.get_net_weight(net);
    if (part[w] != part[v])
    {
        this->totalcost += weight;
    }
    else
    {
        weight = -weight;
    }
    this->__modify_gain(w, weight);
    this->__modify_gain(v, weight);
}

/**
 * @brief
 *
 * @param net
 * @param part
 */
void FMBiGainCalc::__init_gain_3pin_net(
    node_t net, gsl::span<const uint8_t> part)
{
    auto netCur = this->H.G[net].begin();
    auto w = *netCur;
    auto v = *++netCur;
    auto u = *++netCur;

    auto weight = this->H.get_net_weight(net);
    if (part[u] == part[v])
    {
        if (part[w] == part[v])
        {
            for (auto a : {u, v, w})
            {
                this->__modify_gain(a, -weight);
            }
            return;
        }
        this->__modify_gain(w, weight);
    }
    else if (part[w] == part[v])
    {
        this->__modify_gain(u, weight);
    }
    else
    {
        this->__modify_gain(v, weight);
    }
    this->totalcost += weight;
}

/**
 * @brief
 *
 * @param net
 * @param part
 */
void FMBiGainCalc::__init_gain_general_net(
    node_t net, gsl::span<const uint8_t> part)
{
    // uint8_t num[2] = {0, 0};
    auto num = std::array<size_t, 2> {0U, 0U};
    auto IdVec = std::vector<node_t> {};
    for (const auto& w : this->H.G[net])
    {
        num[part[w]] += 1;
        IdVec.push_back(w);
    }
    auto weight = this->H.get_net_weight(net);

    if (num[0] > 0 && num[1] > 0)
    {
        this->totalcost += weight;
    }

    for (auto k : {0, 1})
    {
        if (num[k] == 0)
        {
            for (auto w : IdVec)
            {
                this->__modify_gain(w, -weight);
            }
        }
        else if (num[k] == 1)
        {
            for (auto w : IdVec)
            {
                if (part[w] == k)
                {
                    this->__modify_gain(w, weight);
                    break;
                }
            }
        }
    }
}

/**
 * @brief
 *
 * @param part
 * @param move_info
 * @return ret_2pin_info
 */
FMBiGainCalc::ret_2pin_info FMBiGainCalc::update_move_2pin_net(
    gsl::span<const uint8_t> part, const MoveInfo& move_info)
{
    auto [net, fromPart, _, v] = move_info;

    auto netCur = this->H.G[net].begin();
    auto w = (*netCur != v) ? *netCur : *++netCur;
    auto weight = this->H.get_net_weight(net);
    auto delta = (part[w] == fromPart) ? weight : -weight;
    return {w, 2 * delta};
}

/**
 * @brief
 *
 * @param part
 * @param move_info
 * @return ret_info
 */
FMBiGainCalc::ret_info FMBiGainCalc::update_move_3pin_net(
    gsl::span<const uint8_t> part, const MoveInfo& move_info)
{
    auto [net, fromPart, _, v] = move_info;
    auto num = std::array<size_t, 2> {0U, 0U};
    auto IdVec = std::vector<node_t> {};
    for (const auto& w : this->H.G[net])
    {
        if (w == v)
        {
            continue;
        }
        num[part[w]] += 1;
        IdVec.push_back(w);
    }
    auto deltaGain = std::vector<int> {0, 0};
    auto weight = this->H.get_net_weight(net);
    auto part_w = part[IdVec[0]];

    if (part_w != fromPart)
    {
        weight = -weight;
    }
    if (part_w == part[IdVec[1]])
    {
        deltaGain[0] += weight;
        deltaGain[1] += weight;
    }
    else
    {
        deltaGain[0] += weight;
        deltaGain[1] -= weight;
    }
    return {std::move(IdVec), std::move(deltaGain)};
}

/**
 * @brief
 *
 * @param part
 * @param move_info
 * @return ret_info
 */
FMBiGainCalc::ret_info FMBiGainCalc::update_move_general_net(
    gsl::span<const uint8_t> part, const MoveInfo& move_info)
{
    auto [net, fromPart, toPart, v] = move_info;
    auto num = std::array<uint8_t, 2> {0, 0};
    auto IdVec = std::vector<node_t> {};
    for (const auto& w : this->H.G[net])
    {
        if (w == v)
        {
            continue;
        }
        num[part[w]] += 1;
        IdVec.push_back(w);
    }
    auto degree = IdVec.size();
    auto deltaGain = std::vector<int>(degree, 0);
    auto weight = this->H.get_net_weight(net);
    for (const auto& l : {fromPart, toPart})
    {
        if (num[l] == 0)
        {
            for (size_t index = 0U; index < degree; ++index)
            {
                deltaGain[index] -= weight;
            }
        }
        else if (num[l] == 1)
        {
            for (size_t index = 0U; index < degree; ++index)
            {
                auto part_w = part[IdVec[index]];
                if (part_w == l)
                {
                    deltaGain[index] += weight;
                    break;
                }
            }
        }
        weight = -weight;
    }
    return {std::move(IdVec), std::move(deltaGain)};
}
