#include <ckpttncpp/netlist.hpp>
#include <py2cpp/py2cpp.hpp>
#include <tuple>
#include <vector>

auto max_independent_net(SimpleNetlist &H, const std::vector<size_t> &weight) {
    auto visited = std::vector<bool>(H.nets.size(), false);
    auto S = py::set<node_t>{};
    auto total_cost = 0u;
    for (auto i_net = 0u; i_net < H.nets.size(); ++i_net) {
        if (visited[i_net]) {
            continue;
        }
        auto net = H.nets[i_net];
        S.insert(net);
        total_cost += H.get_net_weight(net);
        for (auto v : H.G[net]) {
            for (auto net2 : H.G[v]) {
                auto i_net2 = H.net_map[net2];
                visited[i_net2] = true;
            }
        }
    }
    return std::tuple{std::move(S), total_cost};
}

// Primal-dual algorithm for (auto minimum vertex cover problem
auto min_net_cover_pd(SimpleNetlist &H, const std::vector<size_t> &weight) {
    // auto S = py::set<node_t>{};
    auto L = std::vector<node_t>{};
    auto is_covered = py::set<node_t>{};
    auto gap = H.net_weight.empty() ? std::vector<size_t>(H.number_of_nets(), 1)
                                    : H.net_weight;
    auto total_primal_cost = 0;
    auto total_dual_cost = 0;
    auto offset = H.number_of_modules();

    for (auto v : H.modules) {
        if (is_covered.contains(v)) {
            continue;
        }
        auto s = *H.G[v].begin();
        auto min_gap = gap[H.net_map[s]];
        for (auto net : H.G[v]) {
            auto i_net = H.net_map[net];
            if (min_gap > gap[i_net]) {
                s = net;
                min_gap = gap[i_net];
            }
        }

        // is_net_cover[i_s] = True
        // S.append(i_s)
        // S.insert(s);
        L.push_back(s);
        for (auto net : H.G[v]) {
            auto i_net = H.net_map[net];
            gap[i_net] -= min_gap;
        }
        assert(gap[H.net_map[s]] == 0);
        for (auto v2 : H.G[s]) {
            is_covered.insert(v2);
        }
        total_primal_cost += H.get_net_weight(s);
        total_dual_cost += min_gap;
    }

    assert(total_primal_cost >= total_dual_cost);
    // std::unordered_set<node_t> S(L.cbegin(), L.cend());

    // auto S2 = py::set<node_t>{S.copy()};
    py::set<node_t> S(L.cbegin(), L.cend());
    // for (auto net : S2) {
    for (auto net : L) {
        auto found = false;
        for (auto v : H.G[net]) {
            auto covered = false;
            for (auto net2 : H.G[v]) {
                if (net2 == net)
                    continue;
                if (S.contains(net2)) {
                    covered = true;
                    break;
                }
            }
            if (!covered) {
                found = true;
                break;
            }
        }
        if (found)
            continue;
        total_primal_cost -= H.get_net_weight(net);
        S.erase(net);
    }

    return std::tuple{std::move(S), total_primal_cost};
}

/**
 * @brief Create a contraction subgraph object
 *
 * @param H
 * @return auto
 */
auto create_contraction_subgraph(SimpleNetlist &H) {
    auto [S, total_cost] = max_independent_net(H, H.module_weight);

    auto module_up_map = py::dict<node_t, size_t>();
    for (auto v : H.modules) {
        module_up_map[v] = v;
    }

    auto C = py::set<node_t>();
    auto nets = std::vector<node_t>();
    auto clusters = std::vector<node_t>();
    auto cluster_map = py::dict<node_t, size_t>{};
    for (auto net : H.nets) {
        if (S.contains(net)) {
            auto netCur = H.G[net].begin();
            auto master = *netCur;
            clusters.push_back(master);
            for (auto v : H.G[net]) {
                module_up_map[v] = master;
                C.insert(v);
            }
            cluster_map[master] = net;
        } else {
            nets.push_back(net);
        }
    }

    auto modules = std::vector<node_t>();
    for (auto v : H.modules) {
        if (C.contains(v))
            continue;
        modules.push_back(v);
    }
    modules.insert(modules.end(), clusters.begin(), clusters.end());
    auto nodes = std::vector<node_t>{};
    nodes.insert(nodes.end(), modules.begin(), modules.end());
    nodes.insert(nodes.end(), nets.begin(), nets.end());
    auto numModules = std::size(modules);
    auto numNets = std::size(nets);
    auto num_vertices = numModules + numNets;

    auto module_map = py::dict<node_t, size_t>{};
    for (auto [i_v, v] : py::enumerate(modules)) {
        module_map[v] = i_v;
    }

    auto net_map = py::dict<node_t, size_t>{};
    for (auto [i_net, net] : py::enumerate(nets)) {
        net_map[net] = i_net;
    }

    auto node_up_map = py::dict<node_t, node_t>{};
    for (auto v : H.modules) {
        node_up_map[v] = module_map[module_up_map[v]];
    }
    for (auto net : H.nets) {
        if (S.contains(net)) {
            continue;
        }
        node_up_map[net] = net_map[net] + numModules;
    }

    graph_t g(nodes.size());
    // G.add_nodes_from(nodes);
    for (auto v : H.modules) {
        for (auto net : H.G[v]) {
            if (S.contains(net)) {
                continue;
            }
            boost::add_edge(node_up_map[v], node_up_map[net], g);
        }
    }
    auto G = xn::grAdaptor<graph_t>(std::move(g));

    auto H2 =
        Netlist(std::move(G), py::range2(0, numModules),
                py::range2(numModules, num_vertices), py::range2(0, numModules),
                py::range2(-numModules, numNets));

    auto node_down_map = py::dict<node_t, node_t>{};
    for (auto [v1, v2] : node_up_map) {
        node_down_map[v2] = v1;
    }
    auto cluster_down_map = py::dict<node_t, node_t>{};
    for (auto [v, net] : cluster_map) {
        cluster_down_map[node_up_map[v]] = net;
    }

    auto module_weight = std::vector<size_t>();
    for (auto i_v = 0u; i_v < modules.size(); ++i_v) {
        auto v = modules[i_v];
        if (cluster_down_map.contains(v)) {
            auto net = cluster_down_map[v];
            auto cluster_weight = 0u;
            for (auto v2 : H.G[net]) {
                cluster_weight += H.get_module_weight(v2);
            }
            module_weight.push_back(cluster_weight);
        }
        else {
            auto v2 = node_down_map[v];
            module_weight.push_back(H.get_module_weight(v2));
        }
    }

    H2.node_up_map = std::move(node_up_map);
    H2.node_down_map = std::move(node_down_map);
    H2.cluster_down_map = std::move(cluster_down_map);
    H2.module_weight = module_weight;
    H2.parent = &H;
    return H2;
}
