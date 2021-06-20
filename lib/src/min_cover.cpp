#include <ckpttncpp/bpqueue.hpp>
#include <ckpttncpp/netlist.hpp>
#include <ckpttncpp/netlist_algo.hpp>
#include <memory>
#include <py2cpp/py2cpp.hpp>
// #include <range/v3/all.hpp>
#include <range/v3/core.hpp>
#include <range/v3/numeric/accumulate.hpp>
#include <range/v3/view/enumerate.hpp>
#include <range/v3/view/remove_if.hpp>
#include <range/v3/view/transform.hpp>
#include <tuple>
#include <vector>

#include <ckpttncpp/HierNetlist.hpp>

using node_t = typename SimpleNetlist::node_t;


/**
 * @brief Create a contraction subgraph object
 *
 * @param[in] H
 * @param[in] DontSelect
 * @return auto
 */
auto create_contraction_subgraph(const SimpleNetlist& H,
    const py::set<node_t>& DontSelect) -> std::unique_ptr<SimpleHierNetlist>
{
    auto weight = py::dict<node_t, int> {};
    for (auto&& net : H.nets)
    {
        auto r_weight = H.G[net] |
            ranges::views::transform(
                [&](auto v) { return H.get_module_weight(v); });
        weight[net] = ranges::accumulate(r_weight, 0);
    }

    auto S = py::set<node_t> {};
    auto dep = DontSelect.copy();
    min_maximal_matching(H, weight, S, dep);

    auto module_up_map = py::dict<node_t, node_t> {};
    module_up_map.reserve(H.number_of_modules());
    for (auto&& v : H)
    {
        module_up_map[v] = v;
    }

    auto cluster_map = py::dict<node_t, node_t> {};
    cluster_map.reserve(S.size());
    auto node_up_map = py::dict<node_t, index_t> {};
    int numModules {};
    int numNets {};

    auto modules = std::vector<node_t> {};
    auto nets = std::vector<node_t> {};
    nets.reserve(H.nets.size() - S.size());

    { // localize C and clusters
        auto C = py::set<node_t> {};
        auto clusters = std::vector<node_t> {};
        C.reserve(3 * S.size()); // ???
        clusters.reserve(S.size());

        for (auto&& net : H.nets)
        {
            if (S.contains(net))
            {
                auto netCur = H.G[net].begin();
                auto master = *netCur;
                clusters.push_back(master);
                for (auto&& v : H.G[net])
                {
                    module_up_map[v] = master;
                    C.insert(v);
                }
                cluster_map[master] = net;
            }
            else
            {
                nets.push_back(net);
            }
        }
        modules.reserve(H.modules.size() - C.size() + clusters.size());
        for (auto&& v :
            H | ranges::views::remove_if([&](auto v) { return C.contains(v); }))
        {
            // if (C.contains(v))
            // {
            //     continue;
            // }
            modules.push_back(v);
        }
        modules.insert(modules.end(), clusters.begin(), clusters.end());
    }
    // auto nodes = std::vector<node_t>{};
    // nodes.reserve(modules.size() + nets.size());

    // nodes.insert(nodes.end(), modules.begin(), modules.end());
    // nodes.insert(nodes.end(), nets.begin(), nets.end());
    numModules = int(modules.size());
    numNets = int(nets.size());

    { // localize module_map and net_map
        auto module_map = py::dict<node_t, index_t> {};
        module_map.reserve(numModules);
        for (auto&& [i_v, v] : ranges::views::enumerate(modules))
        // auto i_v = 0U;
        // for (auto&& v : modules)
        {
            // const auto& v = modules[i_v];
            module_map[v] = index_t(i_v);
            // ++i_v;
        }

        auto net_map = py::dict<node_t, index_t> {};
        net_map.reserve(numNets);
        for (auto&& [i_net, net] : ranges::views::enumerate(nets))
        // auto i_net = 0U;
        // for (auto&& net : nets)
        {
            // const auto& net = nets[i_net];
            net_map[net] = index_t(i_net);
            // ++i_net;
        }

        node_up_map.reserve(H.number_of_modules() + nets.size());

        for (auto&& v : H)
        {
            node_up_map[v] = module_map[module_up_map[v]];
        }
        for (auto&& net : nets)
        {
            node_up_map[net] = net_map[net] + numModules;
        }
    }

    auto num_vertices = numModules + numNets;
    // auto R = py::range<node_t>(0, num_vertices);
    auto g = graph_t {num_vertices};
    // G.add_nodes_from(nodes);
    for (auto&& v : H)
    {
        for (auto&& net : H.G[v] |
                ranges::views::remove_if(
                    [&](auto net) { return S.contains(net); }))
        {
            // if (S.contains(net))
            // {
            //     continue;
            // }
            g.add_edge(node_up_map[v], node_up_map[net]);
        }
    }
    // auto G = xn::grAdaptor<graph_t>(std::move(g));
    auto G = std::move(g);

    auto H2 = std::make_unique<SimpleHierNetlist>(std::move(G),
        py::range(0, numModules), py::range(numModules, num_vertices));

    auto node_down_map = py::dict<index_t, node_t> {};
    node_down_map.reserve(num_vertices);
    // for (auto&& [v1, v2] : node_up_map.items())
    for (auto&& keyvalue : node_up_map.items())
    {
        auto&& v1 = std::get<0>(keyvalue);
        auto&& v2 = std::get<1>(keyvalue);
        node_down_map[v2] = v1;
    }
    auto cluster_down_map = py::dict<index_t, node_t> {};
    cluster_down_map.reserve(cluster_map.size()); // ???
    // for (auto&& [v, net] : cluster_map.items())
    for (auto&& keyvalue : cluster_map.items())
    {
        auto&& v = std::get<0>(keyvalue);
        auto&& net = std::get<1>(keyvalue);
        cluster_down_map[node_up_map[v]] = net;
    }

    auto module_weight = std::vector<int> {};
    module_weight.reserve(numModules);
    for (auto&& i_v : py::range(0, numModules))
    {
        if (cluster_down_map.contains(i_v))
        {
            const auto net = cluster_down_map[i_v];
            // auto cluster_weight = 0U;
            // for (auto&& v2 : H.G[net])
            // {
            //     cluster_weight += H.get_module_weight(v2);
            // }
            auto r_weight = H.G[net] |
                ranges::views::transform(
                    [&](auto v) { return H.get_module_weight(v); });
            auto cluster_weight = ranges::accumulate(r_weight, 0);

            module_weight.push_back(cluster_weight);
        }
        else
        {
            const auto v2 = node_down_map[i_v];
            module_weight.push_back(H.get_module_weight(v2));
        }
    }
    H2->node_up_map = std::move(node_up_map);
    H2->node_down_map = std::move(node_down_map);
    H2->cluster_down_map = std::move(cluster_down_map);
    H2->module_weight = std::move(module_weight);
    H2->parent = &H;
    return H2;
}
