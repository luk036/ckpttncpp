#ifndef _HOME_UBUNTU_GITHUB_CKPTTNCPP_FMBIGAINMGR_HPP
#define _HOME_UBUNTU_GITHUB_CKPTTNCPP_FMBIGAINMGR_HPP 1

#include "dllist.hpp"  // import dllink
#include "bpqueue.hpp" // import bpqueue
#include "netlist.hpp" // import Netlist
#include <cassert>
#include <iterator>

struct FMBiGainMgr
{
    Netlist &H;
    size_t pmax;
    bpqueue gainbucket;
    size_t num[2];
    size_t num_cells;
    std::vector<dllink> vertex_list;

    explicit FMBiGainMgr(Netlist &H)
        : H{H},
          pmax{H.get_max_degree()},
          gainbucket(-pmax, pmax),
          num{0, 0},
          num_cells{H.number_of_cells()},
          vertex_list(num_cells) {}

    auto init(std::vector<size_t> &part) -> void
    {
        for (auto &net : this->H.net_list)
        {
            this->init_gain(net, part);
        }

        for (auto &v : this->H.cell_fixed)
        {
            auto i_v = this->H.cell_dict[v];
            // force to the lowest gain
            this->vertex_list[i_v].key = -this->pmax;
        }

        this->gainbucket.appendfrom(this->vertex_list);
    }

    auto init_gain(node_t &net, std::vector<size_t> &part) -> void
    {
        if (this->H.G.degree(net) == 2)
        {
            this->init_gain_2pin_net(net, part);
        }
        else if (this->H.G.degree(net) < 2)
        {           // unlikely, self-loop, etc.
            return; // does not provide any gain when move
        }
        else
        {
            this->init_gain_general_net(net, part);
        }
    }

    auto init_gain_2pin_net(node_t &net, std::vector<size_t> &part) -> void
    {
        assert(this->H.G.degree(net) == 2);
        auto netCur = this->H.G[net].begin();
        auto w = *netCur;
        auto v = *++netCur;
        auto i_w = this->H.cell_dict[w];
        auto i_v = this->H.cell_dict[v];
        auto part_w = part[i_w];
        auto part_v = part[i_v];
        auto weight = this->H.G[net].get('weight', 1);
        auto g = (part_w == part_v) ? -weight : weight;
        this->vertex_list[i_w].key += g;
        this->vertex_list[i_v].key += g;
    }

    auto init_gain_general_net(node_t &net, std::vector<size_t> &part) -> void
    {
        this->num[0] = 0;
        this->num[1] = 0;
        auto IdVec = std::vector<size_t>();
        for (const auto &w : this->H.G[net])
        {
            auto i_w = this->H.cell_dict[w];
            this->num[part[i_w]] += 1;
            IdVec.push_back(i_w);
        }

        auto weight = this->H.G[net].get('weight', 1);
        for (auto &&k : {0, 1})
        {
            if (this->num[k] == 0)
            {
                for (auto &i_w : IdVec)
                {
                    this->vertex_list[i_w].key -= weight;
                }
            }
            else if (this->num[k] == 1)
            {
                for (auto &i_w : IdVec)
                {
                    auto part_w = part[i_w];
                    if (part_w == k)
                    {
                        this->vertex_list[i_w].key += weight;
                        break;
                    }
                }
            }
        }
    }

    auto update_move(std::vector<size_t> &part, node_t &v) -> void
    {
        auto i_v = this->H.cell_dict[v];
        auto fromPart = part[i_v];

        for (auto net : this->H.G[v])
        {
            if (this->H.G.degree(net) == 2)
            {
                this->update_move_2pin_net(net, part, fromPart, v);
            }
            else if (this->H.G.degree(net) < 2)
            {          // unlikely, self-loop, etc.
                break; // does not provide any gain change when move
            }
            else
            {
                this->update_move_general_net(net, part, fromPart, v);
            }
        }

        auto gain = this->gainbucket.get_key(this->vertex_list[i_v]);
        this->gainbucket.modify_key(this->vertex_list[i_v], -2 * gain);
        part[i_v] = 1 - fromPart;
    }

    auto update_move_2pin_net(node_t &net, std::vector<size_t> &part, size_t fromPart, node_t &v) -> void
    {
        assert(this->H.G.degree(net) == 2);
        auto netCur = this->H.G[net].begin();
        node_t w = *netCur;
        if (w == v)
        {
            w = *++netCur;
        }
        auto i_w = this->H.cell_dict[w];
        auto part_w = part[i_w];
        auto weight = this->H.G[net].get('weight', 1);
        auto deltaGainW = (part_w == fromPart) ? 2 * weight : -2 * weight;
        this->gainbucket.modify_key(this->vertex_list[i_w], deltaGainW);
    }

    auto update_move_general_net(node_t &net, std::vector<size_t> &part, size_t fromPart, node_t &v) -> void
    {
        assert(this->H.G.degree(net) > 2);

        auto IdVec = std::vector<size_t>{};
        auto deltaGain = std::vector<int>{};
        for (const auto &w : this->H.G[net])
        {
            if (w == v)
            {
                continue;
            }
            IdVec.push_back(this->H.cell_dict[w]);
            deltaGain.push_back(0);
        }
        auto degree = std::size(IdVec);
        auto m = this->H.G[net].get('weight', 1);
        auto weight = (fromPart == 0) ? m : -m;
        for (auto &&k : {0, 1})
        {
            if (this->num[k] == 0)
            {
                for (auto idx = 0u; idx < degree; ++idx)
                {
                    deltaGain[idx] -= weight;
                }
            }
            else if (this->num[k] == 1)
            {
                for (auto idx = 0u; idx < degree; ++idx)
                {
                    auto part_w = part[IdVec[idx]];
                    if (part_w == k)
                    {
                        deltaGain[idx] += weight;
                        break;
                    }
                }
            }
            weight = -weight;
        }

        for (auto idx = 0u; idx < degree; ++idx)
        {
            this->gainbucket.modify_key(
                this->vertex_list[IdVec[idx]], deltaGain[idx]);
        }
    }
};

#endif