#include <ckpttncpp/netlist.hpp>
#include <py2cpp/py2cpp.hpp>

template <typename nodeview_t, typename nodemap_t>
void Netlist<nodeview_t, nodemap_t>::projection_up(
    const std::vector<uint8_t>& part, std::vector<uint8_t>& part_up)
{
    auto& H = *this->parent;
    for (auto v : H.modules)
    {
        part_up[this->node_up_map[v]] = part[v];
    }
}

template <typename nodeview_t, typename nodemap_t>
void Netlist<nodeview_t, nodemap_t>::projection_down(
    const std::vector<uint8_t>& part, std::vector<uint8_t>& part_down)
{
    auto& H = *this->parent;
    for (auto v : this->modules)
    {
        if (this->cluster_down_map.contains(v))
        {
            auto net = this->cluster_down_map[v];
            for (auto v2 : H.G[net])
            {
                part_down[v2] = part[v];
            }
        }
        else
        {
            auto v2 = this->node_down_map[v];
            part_down[v2] = part[v];
        }
    }
    // if (extern_nets.empty()) {
    //     return;
    // }
    // extern_nets_down.clear();
    // extern_nets_down.reserve(extern_nets.size());
    // for (auto net : extern_nets) {
    //     extern_nets_down.insert(this->node_down_map[net]);
    // }
}

template struct Netlist<RngIter, RngIter>;
