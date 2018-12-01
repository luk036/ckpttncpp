#include <ckpttncpp/FMKWayGainCalc.hpp>

/* linux-2.6.38.8/include/linux/compiler.h */
#include <stdio.h>
#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

/**
 * @brief 
 * 
 * @param net 
 * @param part 
 * @param vertex_list 
 */
auto FMKWayGainCalc::
init_gain(node_t &net, const std::vector<std::uint8_t> &part) -> void
{
    if (this->H.G.degree(net) == 2) {
        this->init_gain_2pin_net(net, part);
    } else if (unlikely(this->H.G.degree(net) < 2)) {
        return; // does not provide any gain when move
    } else {
        this->init_gain_general_net(net, part);
    }
}
  

auto FMKWayGainCalc::
init_gain_2pin_net(node_t &net, const std::vector<std::uint8_t> &part) -> void 
{
    assert(this->H.G.degree(net) == 2);
    auto netCur = this->H.G[net].begin();
    auto w = *netCur;
    auto v = *++netCur;
    auto part_w = part[w];
    auto part_v = part[v];
    auto weight = this->H.get_net_weight(net);
    if (part_v == part_w) {
        this->modify_gain(w, -weight);
        this->modify_gain(v, -weight);
    }
    this->vertex_list[part_v][w].key += weight;
    this->vertex_list[part_w][v].key += weight;
}


auto FMKWayGainCalc::
init_gain_general_net(node_t &net, const std::vector<std::uint8_t> &part) -> void 
{
    std::vector<size_t> num(this->K, 0);
    auto IdVec = std::vector<size_t>();
    for (const auto &w : this->H.G[net]) {
        num[part[w]] += 1;
        IdVec.push_back(w);
    }
    auto weight = this->H.get_net_weight(net);
    for (auto k = 0u; k < this->K; ++k) {
        if (num[k] == 0) {
            for (auto &w : IdVec) {
                vertex_list[k][w].key -= weight;
            }
        } else if (num[k] == 1) {
            for (auto &w : IdVec) {
                if (part[w] == k) {
                    this->modify_gain(w, weight);
                    break;
                }
            }
        }
    }
}

auto FMKWayGainCalc::
update_move_2pin_net(const std::vector<std::uint8_t> &part,
                          const MoveInfo& move_info) -> ret_2pin_info
{
    auto const &[net, fromPart, toPart, v] = move_info;
    assert(this->H.G.degree(net) == 2);
    auto netCur = this->H.G[net].begin();
    node_t w = (*netCur != v) ? *netCur : *++netCur;
    auto part_w = part[w];
    auto weight = this->H.get_net_weight(net);
    auto deltaGainW = std::vector<int>(this->K, 0);
    // auto deltaGainV = std::vector<int>(this->K, 0);
    if (part_w == fromPart) {
        for (auto k = 0u; k < this->K; ++k) {
            deltaGainW[k] += weight;
            this->deltaGainV[k] += weight;
        }
    } else if (part_w == toPart) {
        for (auto k = 0u; k < this->K; ++k) {
            deltaGainW[k] -= weight;
            this->deltaGainV[k] -= weight;
        }
    }
    deltaGainW[fromPart] -= weight;
    deltaGainW[toPart] += weight;
    return std::tuple{w, std::move(deltaGainW)};
}


auto FMKWayGainCalc::
update_move_general_net(const std::vector<std::uint8_t> &part,
                        const MoveInfo& move_info) -> ret_info
{
    auto const &[net, fromPart, toPart, v] = move_info;
    assert(this->H.G.degree(net) > 2);
    std::vector<size_t> num(this->K, 0);
    auto IdVec = std::vector<size_t>{};
    for (const auto &w : this->H.G[net]) {
        if (w == v)
            continue;
        num[part[w]] += 1;
        IdVec.push_back(w);
    }
    auto degree = std::size(IdVec);
    auto deltaGain = std::vector<std::vector<int>>(degree,
                                 std::vector<int>(this->K, 0));
    // auto deltaGainV = std::vector<int>(this->K, 0);
    // auto m = this->H.G[net].get('weight', 1);
    auto weight = this->H.get_net_weight(net);
    if (num[fromPart] == 0) {
        if (num[toPart] > 0) {
            for (auto idx = 0u; idx < degree; ++idx) {
                deltaGain[idx][fromPart] -= weight;
            }
            for (auto k = 0u; k < this->K; ++k) {
	            this->deltaGainV[k] -= weight;
	        }
        }
    } else { // num[fromPart] > 0
        if (num[toPart] == 0) {
            for (auto idx = 0u; idx < degree; ++idx) {
                deltaGain[idx][toPart] += weight;
            }
            for (auto k = 0u; k < this->K; ++k) {
	            this->deltaGainV[k] += weight;
	        }
        }
    }
    for (auto &&l : {fromPart, toPart}) {
        if (num[l] == 1) {
            for (auto idx = 0u; idx < degree; ++idx) {
                if (part[IdVec[idx]] == l) {
                    for (auto k = 0u; k < this->K; ++k) {
                        deltaGain[idx][k] += weight;
                    }
                    break;
                }
            }
        }
        weight = -weight;
    }
    return std::tuple{std::move(IdVec),
                      std::move(deltaGain)};
}
