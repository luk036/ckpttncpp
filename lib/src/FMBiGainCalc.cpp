#include <ckpttncpp/FMBiGainCalc.hpp>
/* linux-2.6.38.8/include/linux/compiler.h */
#include <cstdio>
#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

/**
 * @brief
 *
 * @param net
 * @param part
 */
void FMBiGainCalc::__init_gain( //
    node_t net, const std::vector<uint8_t>& part)
{
    auto degree = this->H.G.degree(net);
    if (unlikely(degree < 2))
    {
        return; // does not provide any gain when moving
    }
    switch (degree)
    {
        case 2:
            this->__init_gain_2pin_net(net, part);
            break;
        case 3:
            this->__init_gain_3pin_net(net, part);
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
    node_t net, const std::vector<uint8_t>& part)
{
    auto netCur = this->H.G[net].begin();
    auto w = *netCur;
    auto v = *++netCur;
    auto i_w = this->H.module_map[w];
    auto i_v = this->H.module_map[v];

    auto weight = this->H.get_net_weight(net);
    if (part[i_w] != part[i_v])
    {
        this->totalcost += weight;
    }
    else
    {
        weight = -weight;
    }
    this->__modify_gain(i_w, weight);
    this->__modify_gain(i_v, weight);
}

/**
 * @brief
 *
 * @param net
 * @param part
 */
void FMBiGainCalc::__init_gain_3pin_net(
    node_t net, const std::vector<uint8_t>& part)
{
    auto netCur = this->H.G[net].begin();
    auto w = *netCur;
    auto v = *++netCur;
    auto u = *++netCur;
    auto i_w = this->H.module_map[w];
    auto i_v = this->H.module_map[v];
    auto i_u = this->H.module_map[u];

    auto weight = this->H.get_net_weight(net);
    if (part[i_u] == part[i_v])
    {
        if (part[i_w] == part[i_v])
        {
            for (auto i_a : {i_u, i_v, i_w})
            {
                this->__modify_gain(i_a, -weight);
            }
            return;
        }
        this->__modify_gain(i_w, weight);
    }
    else if (part[i_w] == part[i_v])
    {
        this->__modify_gain(i_u, weight);
    }
    else
    {
        this->__modify_gain(i_v, weight);
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
    node_t net, const std::vector<uint8_t>& part)
{
    // uint8_t num[2] = {0, 0};
    auto num = std::array<size_t, 2> {0U, 0U};
    auto IdVec = std::vector<index_t> {};
    for (const auto& w : this->H.G[net])
    {
        auto i_w = this->H.module_map[w];
        num[part[i_w]] += 1;
        IdVec.push_back(i_w);
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
            for (auto i_w : IdVec)
            {
                this->__modify_gain(i_w, -weight);
            }
        }
        else if (num[k] == 1)
        {
            for (auto i_w : IdVec)
            {
                if (part[i_w] == k)
                {
                    this->__modify_gain(i_w, weight);
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
    const std::vector<uint8_t>& part, const MoveInfo& move_info)
{
    auto [net, fromPart, _, v] = move_info;

    auto netCur = this->H.G[net].begin();
    auto w = (*netCur != v) ? *netCur : *++netCur;
    auto i_w = this->H.module_map[w];
    auto weight = this->H.get_net_weight(net);
    auto delta = (part[i_w] == fromPart) ? weight : -weight;
    return std::tuple {i_w, 2 * delta};
}

/**
 * @brief
 *
 * @param part
 * @param move_info
 * @return ret_info
 */
FMBiGainCalc::ret_info FMBiGainCalc::update_move_3pin_net(
    const std::vector<uint8_t>& part, const MoveInfo& move_info)
{
    auto [net, fromPart, _, v] = move_info;

    // uint8_t num[2] = {0, 0};
    auto num = std::array<size_t, 2> {0U, 0U};
    auto IdVec = std::vector<index_t> {};
    for (const auto& w : this->H.G[net])
    {
        if (w == v)
        {
            continue;
        }
        auto i_w = this->H.module_map[w];
        num[part[i_w]] += 1;
        IdVec.push_back(i_w);
    }
    // auto degree = IdVec.size();
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
    return std::tuple {std::move(IdVec), std::move(deltaGain)};
}

/**
 * @brief
 *
 * @param part
 * @param move_info
 * @return ret_info
 */
FMBiGainCalc::ret_info FMBiGainCalc::update_move_general_net(
    const std::vector<uint8_t>& part, const MoveInfo& move_info)
{
    auto [net, fromPart, toPart, v] = move_info;
    // uint8_t num[2] = {0, 0};
    auto num = std::array<uint8_t, 2> {0, 0};
    auto IdVec = std::vector<index_t> {};
    for (const auto& w : this->H.G[net])
    {
        if (w == v)
        {
            continue;
        }
        auto i_w = this->H.module_map[w];
        num[part[i_w]] += 1;
        IdVec.push_back(i_w);
    }
    auto degree = IdVec.size();
    auto deltaGain = std::vector<int>(degree, 0);
    auto weight = this->H.get_net_weight(net);
    for (const auto& l : {fromPart, toPart})
    {
        if (num[l] == 0)
        {
            for (size_t idx = 0U; idx < degree; ++idx)
            {
                deltaGain[idx] -= weight;
            }
        }
        else if (num[l] == 1)
        {
            for (size_t idx = 0U; idx < degree; ++idx)
            {
                auto part_w = part[IdVec[idx]];
                if (part_w == l)
                {
                    deltaGain[idx] += weight;
                    break;
                }
            }
        }
        weight = -weight;
    }
    return std::tuple {std::move(IdVec), std::move(deltaGain)};
}
