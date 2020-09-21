#include <ckpttncpp/FMBiGainCalc.hpp>

/**
 * @brief
 *
 * @param[in] net
 * @param[in] part
 */
void FMBiGainCalc::_init_gain( //
    const node_t& net, gsl::span<const std::uint8_t> part)
{
    const auto degree = this->H.G.degree(net);
    if (degree < 2) // [[unlikely]]
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
void FMBiGainCalc::_init_gain_2pin_net( //
    const node_t& net, gsl::span<const std::uint8_t> part)
{
    auto netCur = this->H.G[net].begin();
    const auto w = *netCur;
    const auto v = *++netCur;

    const auto weight = this->H.get_net_weight(net);
    if (part[w] != part[v])
    {
        this->totalcost += weight;
        this->_modify_gain_va(weight, w, v);
    }
    else
    {
        this->_modify_gain_va(-weight, w, v);
    }
}

/**
 * @brief
 *
 * @param[in] net
 * @param[in] part
 */
void FMBiGainCalc::_init_gain_3pin_net(
    const node_t& net, gsl::span<const std::uint8_t> part)
{
    auto netCur = this->H.G[net].begin();
    const auto w = *netCur;
    const auto v = *++netCur;
    const auto u = *++netCur;

    const auto weight = this->H.get_net_weight(net);
    if (part[u] == part[v])
    {
        if (part[w] == part[v])
        {
            this->_modify_gain_va(-weight, u, v, w);
            return;
        }
        this->_modify_gain_va(weight, w);
    }
    else if (part[w] == part[v])
    {
        this->_modify_gain(u, weight);
    }
    else
    {
        this->_modify_gain(v, weight);
    }
    this->totalcost += weight;
}

/**
 * @brief
 *
 * @param[in] net
 * @param[in] part
 */
void FMBiGainCalc::_init_gain_general_net(
    const node_t& net, gsl::span<const std::uint8_t> part)
{
    // std::uint8_t num[2] = {0, 0};
    auto num = std::array<size_t, 2> {0U, 0U};
    auto IdVec = std::vector<node_t> {};
    for (auto&& w : this->H.G[net])
    {
        num[part[w]] += 1;
        IdVec.push_back(w);
    }
    const auto weight = this->H.get_net_weight(net);

    if (num[0] > 0 && num[1] > 0)
    {
        this->totalcost += weight;
    }

    // for... (auto&& k : {0, 1})
    auto repeat = [&](auto&& k) {
        if (num[k] == 0)
        {
            for (auto&& w : IdVec)
            {
                this->_modify_gain(w, -weight);
            }
        }
        else if (num[k] == 1)
        {
            for (auto&& w : IdVec)
            {
                if (part[w] == k)
                {
                    this->_modify_gain(w, weight);
                    break;
                }
            }
        }
    };
    repeat(0);
    repeat(1);
}

/**
 * @brief
 *
 * @param[in] part
 * @param[in] move_info
 * @return ret_2pin_info
 */
FMBiGainCalc::ret_2pin_info FMBiGainCalc::update_move_2pin_net(
    gsl::span<const std::uint8_t> part, const MoveInfo& move_info)
{
    auto netCur = this->H.G[move_info.net].begin();
    const auto w = (*netCur != move_info.v) ? *netCur : *++netCur;
    const auto weight = this->H.get_net_weight(move_info.net);
    const auto delta = (part[w] == move_info.fromPart) ? weight : -weight;
    return {w, 2 * delta};
}

/**
 * @brief
 *
 * @param[in] part
 * @param[in] move_info
 * @return ret_info
 */
FMBiGainCalc::ret_info FMBiGainCalc::update_move_3pin_net(
    gsl::span<const std::uint8_t> part, const MoveInfo& move_info)
{
    // const auto& [net, v, fromPart, _] = move_info;
    auto num = std::array<size_t, 2> {0U, 0U};
    auto IdVec = std::vector<node_t> {};
    for (auto&& w : this->H.G[move_info.net])
    {
        if (w == move_info.v)
        {
            continue;
        }
        num[part[w]] += 1;
        IdVec.push_back(w);
    }
    auto deltaGain = std::vector<int> {0, 0};
    auto weight = this->H.get_net_weight(move_info.net);
    const auto part_w = part[IdVec[0]];

    if (part_w != move_info.fromPart)
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
 * @param[in] part
 * @param[in] move_info
 * @return ret_info
 */
FMBiGainCalc::ret_info FMBiGainCalc::update_move_general_net(
    gsl::span<const std::uint8_t> part, const MoveInfo& move_info)
{
    // const auto& [net, v, fromPart, toPart] = move_info;
    auto num = std::array<std::uint8_t, 2> {0, 0};
    auto IdVec = std::vector<node_t> {};
    for (auto&& w : this->H.G[move_info.net])
    {
        if (w == move_info.v)
        {
            continue;
        }
        num[part[w]] += 1;
        IdVec.push_back(w);
    }
    auto degree = IdVec.size();
    auto deltaGain = std::vector<int>(degree, 0);
    auto weight = this->H.get_net_weight(move_info.net);
    // for... (auto&& l : {move_info.fromPart, move_info.toPart})
    auto repeat = [&](auto&& l) {
        if (num[l] == 0)
        {
            for (size_t index = 0U; index != degree; ++index)
            {
                deltaGain[index] -= weight;
            }
        }
        else if (num[l] == 1)
        {
            for (size_t index = 0U; index != degree; ++index)
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
    };
    repeat(move_info.fromPart);
    repeat(move_info.toPart);

    return {std::move(IdVec), std::move(deltaGain)};
}
