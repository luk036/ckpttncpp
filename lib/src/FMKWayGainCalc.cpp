#include <ckpttncpp/FMKWayGainCalc.hpp>

/* linux-2.6.38.8/include/linux/compiler.h */
#include <cstdio>
#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

/**
 * @brief
 *
 * @param net
 * @param part
 * @param vertex_list
 */
void FMKWayGainCalc::__init_gain(node_t net, const std::vector<uint8_t>& part)
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
void FMKWayGainCalc::__init_gain_2pin_net(
    node_t net, const std::vector<uint8_t>& part)
{
    auto netCur = this->H.G[net].begin();
    auto w = *netCur;
    auto v = *++netCur;
    auto i_w = this->H.module_map[w];
    auto i_v = this->H.module_map[v];
    auto part_w = part[i_w];
    auto part_v = part[i_v];
    auto weight = this->H.get_net_weight(net);
    if (part_v == part_w)
    {
        this->__modify_gain(i_w, part_v, -weight);
        this->__modify_gain(i_v, part_v, -weight);
    }
    else
    {
        this->totalcost += weight;
        this->vertex_list[part_v][i_w].key += weight;
        this->vertex_list[part_w][i_v].key += weight;
    }
}

/**
 * @brief
 *
 * @param net
 * @param part
 */
void FMKWayGainCalc::__init_gain_3pin_net(
    node_t net, const std::vector<uint8_t>& part)
{
    auto netCur = this->H.G[net].begin();
    auto w = *netCur;
    auto v = *++netCur;
    auto u = *++netCur;
    auto i_w = this->H.module_map[w];
    auto i_v = this->H.module_map[v];
    auto i_u = this->H.module_map[u];
    auto part_w = part[i_w];
    auto part_v = part[i_v];
    auto part_u = part[i_u];
    auto weight = this->H.get_net_weight(net);
    auto i_a = i_w;
    auto i_b = i_v;
    auto i_c = i_u;

    if (part_u == part_v)
    {
        if (part_w == part_v)
        {
            this->__modify_gain(i_u, part_v, -weight);
            this->__modify_gain(i_v, part_v, -weight);
            this->__modify_gain(i_w, part_v, -weight);
            return;
        }
    }
    else if (part_w == part_v)
    {
        i_a = i_u, i_b = i_w, i_c = i_v;
    }
    else if (part_w == part_u)
    {
        i_a = i_v, i_b = i_u, i_c = i_w;
    }
    else
    {
        this->totalcost += 2 * weight;
        this->vertex_list[part_v][i_u].key += weight;
        this->vertex_list[part_w][i_u].key += weight;
        this->vertex_list[part_w][i_v].key += weight;
        this->vertex_list[part_u][i_v].key += weight;
        this->vertex_list[part_u][i_w].key += weight;
        this->vertex_list[part_v][i_w].key += weight;
        return;
    }

    this->vertex_list[part[i_b]][i_a].key += weight;
    for (auto i_e : {i_b, i_c})
    {
        this->__modify_gain(i_e, part[i_b], -weight);
        this->vertex_list[part[i_a]][i_e].key += weight;
    }
    this->totalcost += weight;
}

/**
 * @brief
 *
 * @param net
 * @param part
 */
void FMKWayGainCalc::__init_gain_general_net(
    node_t net, const std::vector<uint8_t>& part)
{
    auto num = std::vector<uint8_t>(this->K, 0);
    auto IdVec = std::vector<index_t> {};
    for (const auto& w : this->H.G[net])
    {
        auto i_w = this->H.module_map[w];
        num[part[i_w]] += 1;
        IdVec.push_back(i_w);
    }
    auto weight = this->H.get_net_weight(net);
    for (auto k = 0U; k < this->K; ++k)
    {
        if (num[k] > 0)
        {
            this->totalcost += weight;
        }
    }
    this->totalcost -= weight;

    for (auto k = 0U; k < this->K; ++k)
    {
        if (num[k] == 0)
        {
            for (auto i_w : IdVec)
            {
                vertex_list[k][i_w].key -= weight;
            }
        }
        else if (num[k] == 1)
        {
            for (auto i_w : IdVec)
            {
                if (part[i_w] == k)
                {
                    this->__modify_gain(i_w, part[i_w], weight);
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
FMKWayGainCalc::ret_2pin_info FMKWayGainCalc::update_move_2pin_net(
    const std::vector<uint8_t>& part, const MoveInfo& move_info)
{
    auto [net, fromPart, toPart, v] = move_info;

    auto netCur = this->H.G[net].begin();
    auto w = (*netCur != v) ? *netCur : *++netCur;
    auto i_w = this->H.module_map[w];
    auto weight = this->H.get_net_weight(net);
    auto deltaGainW = std::vector(this->K, 0);

    for (auto l : {fromPart, toPart}) // for ...
    {
        if (part[i_w] == l)
        {
            for (auto k = 0U; k < this->K; ++k)
            {
                deltaGainW[k] += weight;
                this->deltaGainV[k] += weight;
            }
        }
        deltaGainW[l] -= weight;
        weight = -weight;
    }
    return {i_w, std::move(deltaGainW)};
}

/**
 * @brief
 *
 * @param part
 * @param move_info
 * @return ret_info
 */
FMKWayGainCalc::ret_info FMKWayGainCalc::update_move_3pin_net(
    const std::vector<uint8_t>& part, const MoveInfo& move_info)
{
    auto [net, fromPart, toPart, v] = move_info;

    auto IdVec = std::vector<index_t> {};
    for (const auto& w : this->H.G[net])
    {
        if (w == v)
        {
            continue;
        }
        auto i_w = this->H.module_map[w];
        IdVec.push_back(i_w);
    }
    auto degree = IdVec.size();
    auto deltaGain = std::vector(degree, std::vector(this->K, 0));
    auto weight = this->H.get_net_weight(net);
    auto part_w = part[IdVec[0]];
    auto part_u = part[IdVec[1]];
    auto l = fromPart;
    auto u = toPart;

    if (part_w == part_u)
    {
        for (auto i = 0; i < 2; ++i)
        {
            if (part_w != l)
            {
                deltaGain[0][l] -= weight;
                deltaGain[1][l] -= weight;
                if (part_w == u)
                {
                    for (auto k = 0U; k < this->K; ++k)
                    {
                        this->deltaGainV[k] -= weight;
                    }
                }
            }
            weight = -weight;
            std::swap(l, u);
        }
        return {std::move(IdVec), std::move(deltaGain)};
    }

    for (auto i = 0; i < 2; ++i)
    {
        if (part_w == l)
        {
            for (auto k = 0U; k < this->K; ++k)
            {
                deltaGain[0][k] += weight;
            }
        }
        else if (part_u == l)
        {
            for (auto k = 0U; k < this->K; ++k)
            {
                deltaGain[1][k] += weight;
            }
        }
        else
        {
            deltaGain[0][l] -= weight;
            deltaGain[1][l] -= weight;
            if (part_w == u || part_u == u)
            {
                for (auto k = 0U; k < this->K; ++k)
                {
                    this->deltaGainV[k] -= weight;
                }
            }
        }
        weight = -weight;
        std::swap(l, u);
    }

    return {std::move(IdVec), std::move(deltaGain)};
    // return this->update_move_general_net(part, move_info);
}

/**
 * @brief
 *
 * @param part
 * @param move_info
 * @return ret_info
 */
FMKWayGainCalc::ret_info FMKWayGainCalc::update_move_general_net(
    const std::vector<uint8_t>& part, const MoveInfo& move_info)
{
    auto [net, fromPart, toPart, v] = move_info;

    auto num = std::vector<uint8_t>(this->K, 0);
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
    auto deltaGain = std::vector(degree, std::vector(this->K, 0));
    auto weight = this->H.get_net_weight(net);

    auto l = fromPart;
    auto u = toPart;
    for (auto i = 0; i < 2; ++i)
    {
        if (num[l] == 0)
        {
            for (size_t idx = 0U; idx < degree; ++idx)
            {
                deltaGain[idx][l] -= weight;
            }
            if (num[u] > 0)
            {
                for (auto k = 0U; k < this->K; ++k)
                {
                    this->deltaGainV[k] -= weight;
                }
            }
        }
        else if (num[l] == 1)
        {
            for (size_t idx = 0U; idx < degree; ++idx)
            {
                if (part[IdVec[idx]] == l)
                {
                    for (auto k = 0U; k < this->K; ++k)
                    {
                        deltaGain[idx][k] += weight;
                    }
                    break;
                }
            }
        }
        weight = -weight;
        std::swap(l, u);
    }
    return {std::move(IdVec), std::move(deltaGain)};
}
