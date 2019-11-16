#include <ckpttncpp/bpqueue.hpp>
#include <ckpttncpp/netlist.hpp>
#include <memory>
#include <py2cpp/py2cpp.hpp>
#include <tuple>
#include <vector>

/**
 * @brief
 *
 * @param H
 * @param DontSelect
 * @return auto
 */
auto max_independent_net(SimpleNetlist& H, const std::vector<int>& /*weight*/,
    const py::set<node_t>& DontSelect)
{
    // bpqueue bpq {-int(H.get_max_net_degree()), 0};
    // auto nets = std::vector<dllink<node_t>>(H.nets.size());

    // for (size_t i_net = 0U; i_net < H.nets.size(); ++i_net)
    // {
    //     auto net = H.nets[i_net];
    //     bpq.append(nets[i_net], -H.G.degree(net));
    // }

    auto visited = py::set<node_t> {};
    for (auto net : DontSelect)
    {
        visited.insert(net);
    }

    auto S = py::set<node_t> {};
    auto total_cost = 0U;

    // while (!bpq.is_empty())
    for (auto net : H.nets)
    {
        // dllink<node_t>& item = bpq.popleft();

        if (visited.contains(net))
        {
            continue;
        }
        if (H.G.degree(net) < 2)
        {
            continue;
        }
        S.insert(net);
        total_cost += H.get_net_weight(net);
        for (auto v : H.G[net])
        {
            for (auto net2 : H.G[v])
            {
                visited.insert(net2);
            }
        }
    }
    return std::tuple {std::move(S), total_cost};
}

// /**
//  * @brief
//  *
//  * @param H
//  * @return auto
//  */
// auto min_net_cover_pd(SimpleNetlist &H, const std::vector<int> & /*weight*/)
// {
//     // auto S = py::set<node_t>{};
//     auto L = std::vector<node_t>{};
//     auto is_covered = py::set<node_t>{};
//     auto gap = H.net_weight.empty() ? std::vector<int>(H.number_of_nets(), 1)
//                                     : H.net_weight;
//     auto total_primal_cost = 0;
//     auto total_dual_cost = 0;
//     auto offset = H.number_of_modules();

//     for (auto v : H.modules) {
//         if (is_covered.contains(v)) {
//             continue;
//         }
//         auto s = *H.G[v].begin();
//         auto min_gap = gap[H.net_map[s]];
//         for (auto net : H.G[v]) {
//             auto i_net = H.net_map[net];
//             if (min_gap > gap[i_net]) {
//                 s = net;
//                 min_gap = gap[i_net];
//             }
//         }

//         // is_net_cover[i_s] = True
//         // S.append(i_s)
//         // S.insert(s);
//         L.push_back(s);
//         for (auto net : H.G[v]) {
//             auto i_net = H.net_map[net];
//             gap[i_net] -= min_gap;
//         }
//         assert(gap[H.net_map[s]] == 0);
//         for (auto v2 : H.G[s]) {
//             is_covered.insert(v2);
//         }
//         total_primal_cost += H.get_net_weight(s);
//         total_dual_cost += min_gap;
//     }

//     assert(total_primal_cost >= total_dual_cost);
//     // std::unordered_set<node_t> S(L.cbegin(), L.cend());

//     // auto S2 = py::set<node_t>{S.copy()};
//     py::set<node_t> S(L.cbegin(), L.cend());
//     // for (auto net : S2) {
//     for (auto net : L) {
//         auto found = false;
//         for (auto v : H.G[net]) {
//             auto covered = false;
//             for (auto net2 : H.G[v]) {
//                 if (net2 == net) {
//                     continue;
// }
//                 if (S.contains(net2)) {
//                     covered = true;
//                     break;
//                 }
//             }
//             if (!covered) {
//                 found = true;
//                 break;
//             }
//         }
//         if (found) {
//             continue;
// }
//         total_primal_cost -= H.get_net_weight(net);
//         S.erase(net);
//     }

//     return std::tuple{std::move(S), total_primal_cost};
// }

/**
 * @brief Create a contraction subgraph object
 *
 * @param H
 * @param DontSelect
 * @return auto
 */
auto create_contraction_subgraph(
    SimpleNetlist& H, const py::set<node_t>& DontSelect)
{
    auto [S, _] = max_independent_net(H, H.module_weight, DontSelect);

    auto module_up_map = py::dict<node_t, node_t> {};
    module_up_map.reserve(H.number_of_modules());
    for (auto v : H.modules)
    {
        module_up_map[v] = v;
    }

    auto cluster_map = py::dict<node_t, node_t> {};
    cluster_map.reserve(S.size());
    auto node_up_map = py::dict<node_t, index_t> {};
    size_t numModules;
    size_t numNets;

    auto modules = std::vector<node_t> {};
    auto nets = std::vector<node_t> {};
    nets.reserve(H.nets.size() - S.size());

    { // localize C and clusters
        auto C = py::set<node_t> {};
        auto clusters = std::vector<node_t> {};
        C.reserve(3 * S.size()); // ???
        clusters.reserve(S.size());

        for (auto net : H.nets)
        {
            if (S.contains(net))
            {
                auto netCur = H.G[net].begin();
                auto master = *netCur;
                clusters.push_back(master);
                for (auto v : H.G[net])
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
        for (auto v : H.modules)
        {
            if (C.contains(v))
            {
                continue;
            }
            modules.push_back(v);
        }
        modules.insert(modules.end(), clusters.begin(), clusters.end());
    }
    // auto nodes = std::vector<node_t>{};
    // nodes.reserve(modules.size() + nets.size());

    // nodes.insert(nodes.end(), modules.begin(), modules.end());
    // nodes.insert(nodes.end(), nets.begin(), nets.end());
    numModules = modules.size();
    numNets = nets.size();

    { // localize module_map and net_map
        auto module_map = py::dict<node_t, index_t> {};
        module_map.reserve(numModules);
        for (auto [i_v, v] : py::enumerate(modules))
        {
            module_map[v] = i_v;
        }

        auto net_map = py::dict<node_t, index_t> {};
        net_map.reserve(numNets);
        for (auto [i_net, net] : py::enumerate(nets))
        {
            net_map[net] = i_net;
        }

        node_up_map.reserve(H.number_of_modules() + nets.size());

        for (auto v : H.modules)
        {
            node_up_map[v] = module_map[module_up_map[v]];
        }
        for (auto net : nets)
        {
            node_up_map[net] = net_map[net] + numModules;
        }
    }

    auto num_vertices = numModules + numNets;
    auto R = py::range<node_t>(0, num_vertices);
    auto g = graph_t {R, R};
    // G.add_nodes_from(nodes);
    for (auto v : H.modules)
    {
        for (auto net : H.G[v])
        {
            if (S.contains(net))
            {
                continue;
            }
            g.add_edge(node_up_map[v], node_up_map[net]);
        }
    }
    // auto G = xn::grAdaptor<graph_t>(std::move(g));
    auto G = std::move(g);

    auto H2 = std::make_unique<SimpleNetlist>(std::move(G),
        py::range<int>(numModules), py::range<int>(numModules, num_vertices),
        py::range<int>(numModules));

    auto node_down_map = py::dict<index_t, node_t> {};
    node_down_map.reserve(num_vertices);
    for (auto [v1, v2] : node_up_map.items())
    {
        node_down_map[v2] = v1;
    }
    auto cluster_down_map = py::dict<index_t, node_t> {};
    cluster_down_map.reserve(cluster_map.size()); // ???
    for (auto [v, net] : cluster_map.items())
    {
        cluster_down_map[node_up_map[v]] = net;
    }

    auto module_weight = std::vector<int> {};
    module_weight.reserve(numModules);
    for (auto i_v : py::range<int>(numModules))
    {
        if (cluster_down_map.contains(i_v))
        {
            auto net = cluster_down_map[i_v];
            auto cluster_weight = 0U;
            for (auto v2 : H.G[net])
            {
                cluster_weight += H.get_module_weight(v2);
            }
            module_weight.push_back(cluster_weight);
        }
        else
        {
            auto v2 = node_down_map[i_v];
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
