#include <ckpttncpp/FMKWayGainCalc.hpp>

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
        this->vertex_list[part_v][w].key += weight;
        this->vertex_list[part_w][v].key += weight;
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
        this->vertex_list[part_v][u].key += weight;
        this->vertex_list[part_w][u].key += weight;
        this->vertex_list[part_w][v].key += weight;
        this->vertex_list[part_u][v].key += weight;
        this->vertex_list[part_u][w].key += weight;
        this->vertex_list[part_v][w].key += weight;
        return;
    }

    // for (auto&& e : {b, c})
    // {
    //     this->_modify_gain(e, part[b], -weight);
    //     this->vertex_list[part[a]][e].key += weight;
    // }
    this->_modify_gain_va(-weight, part[b], b, c);
    this->vertex_list[part[a]][b].key += weight;
    this->vertex_list[part[a]][c].key += weight;
    this->vertex_list[part[b]][a].key += weight;

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
    auto num = std::vector<std::uint8_t>(this->K, 0);
    auto IdVec = std::vector<node_t> {};
    for (auto&& w : this->H.G[net])
    {
        num[part[w]] += 1;
        IdVec.push_back(w);
    }
    const auto weight = this->H.get_net_weight(net);
    for (auto k = 0U; k != this->K; ++k)
    {
        if (num[k] > 0)
        {
            this->totalcost += weight;
        }
    }
    this->totalcost -= weight;

    for (auto k = 0U; k != this->K; ++k)
    {
        if (num[k] == 0)
        {
            for (auto&& w : IdVec)
            {
                vertex_list[k][w].key -= weight;
            }
        }
        else if (num[k] == 1)
        {
            for (auto&& w : IdVec)
            {
                if (part[w] == k)
                {
                    this->_modify_gain(w, part[w], weight);
                    break;
                }
            }
        }
    }
}

/**
 * @brief
 *
 * @param[in] part
 * @param[in] move_info
 * @return ret_2pin_info
 */
FMKWayGainCalc::ret_2pin_info FMKWayGainCalc::update_move_2pin_net(
    gsl::span<const std::uint8_t> part, const MoveInfo& move_info)
{
    const auto& [net, v, fromPart, toPart] = move_info;
    if (part[v] != fromPart)
        exit(1);

    auto netCur = this->H.G[net].begin();
    const auto w = (*netCur != v) ? *netCur : *++netCur;
    auto weight = this->H.get_net_weight(net);
    auto deltaGainW = std::vector(this->K, 0);

    for (auto&& l : {fromPart, toPart}) // for ...
    {
        if (part[w] == l)
        {
            for (auto k = 0U; k != this->K; ++k)
            {
                deltaGainW[k] += weight;
                this->deltaGainV[k] += weight;
            }
        }
        deltaGainW[l] -= weight;
        weight = -weight;
    }
    return {w, std::move(deltaGainW)};
}

/**
 * @brief
 *
 * @param[in] part
 * @param[in] move_info
 * @return ret_info
 */
FMKWayGainCalc::ret_info FMKWayGainCalc::update_move_3pin_net(
    gsl::span<const std::uint8_t> part, const MoveInfo& move_info)
{
    const auto& [net, v, fromPart, toPart] = move_info;
    auto IdVec = std::vector<node_t> {};
    for (auto&& w : this->H.G[net])
    {
        if (w == v)
        {
            continue;
        }
        IdVec.push_back(w);
    }
    const auto degree = IdVec.size();
    auto deltaGain = std::vector(degree, std::vector(this->K, 0));
    auto weight = this->H.get_net_weight(net);
    const auto part_w = part[IdVec[0]];
    const auto part_u = part[IdVec[1]];
    auto l = fromPart;
    auto u = toPart;

    if (part_w == part_u)
    {
        for (auto i = 0; i != 2; ++i)
        {
            if (part_w != l)
            {
                deltaGain[0][l] -= weight;
                deltaGain[1][l] -= weight;
                if (part_w == u)
                {
                    for (auto k = 0U; k != this->K; ++k)
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

    for (auto i = 0; i != 2; ++i)
    {
        if (part_w == l)
        {
            for (auto k = 0U; k != this->K; ++k)
            {
                deltaGain[0][k] += weight;
            }
        }
        else if (part_u == l)
        {
            for (auto k = 0U; k != this->K; ++k)
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
                for (auto k = 0U; k != this->K; ++k)
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
 * @param[in] part
 * @param[in] move_info
 * @return ret_info
 */
FMKWayGainCalc::ret_info FMKWayGainCalc::update_move_general_net(
    gsl::span<const std::uint8_t> part, const MoveInfo& move_info)
{
    const auto& [net, v, fromPart, toPart] = move_info;

    auto num = std::vector<std::uint8_t>(this->K, 0);
    auto IdVec = std::vector<node_t> {};
    for (auto&& w : this->H.G[net])
    {
        if (w == v)
        {
            continue;
        }
        num[part[w]] += 1;
        IdVec.push_back(w);
    }
    const auto degree = IdVec.size();
    auto deltaGain = std::vector(degree, std::vector(this->K, 0));
    auto weight = this->H.get_net_weight(net);

    auto l = fromPart;
    auto u = toPart;
    for (auto i = 0; i != 2; ++i)
    {
        if (num[l] == 0)
        {
            for (size_t index = 0U; index < degree; ++index)
            {
                deltaGain[index][l] -= weight;
            }
            if (num[u] > 0)
            {
                for (auto k = 0U; k != this->K; ++k)
                {
                    this->deltaGainV[k] -= weight;
                }
            }
        }
        else if (num[l] == 1)
        {
            for (size_t index = 0U; index < degree; ++index)
            {
                if (part[IdVec[index]] == l)
                {
                    for (auto k = 0U; k != this->K; ++k)
                    {
                        deltaGain[index][k] += weight;
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
