#include <ckpttncpp/netlist.hpp>
#include <py2cpp/py2cpp.hpp>
#include <tuple>
#include <vector>

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
    auto [S, total_cost] = min_net_cover_pd(H, H.module_weight);

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
            nets.push_back(net);
        } else {
            auto netCur = H.G[net].begin();
            auto master = *netCur;
            clusters.push_back(master);
            for (auto v : H.G[net]) {
                module_up_map[v] = master;
                C.insert(v);
            }
            cluster_map[master] = net;
        }
    }

    auto modules = std::vector<node_t>();
    for (auto v : H.modules) {
        if (C.contains(v))
            continue;
        modules.push_back(v);
    }
    modules.insert(modules.end(), clusters.begin(), clusters.end());
    auto nodes = std::vector<node_t>(modules.size() + nets.size());
    nodes.insert(nodes.end(), modules.begin(), modules.end());
    nodes.insert(nodes.end(), nets.begin(), nets.end());

    graph_t g(nodes.size());
    auto G = xn::grAdaptor<graph_t>(std::move(g));

    // G.add_nodes_from(nodes);
    for (auto v : H.modules) {
        for (auto net : H.G[v]) {
            boost::add_edge(module_up_map[v], net, g);
        }
    }

    auto module_map = py::dict<node_t, size_t>{};
    for (auto [i_v, v] : py::enumerate(modules)) {
        module_map[v] = i_v;
    }

    auto net_map = py::dict<node_t, size_t>{};
    for (auto [i_net, net] : py::enumerate(nets)) {
        net_map[net] = i_net;
    }

    auto H2 = Netlist(std::move(G), modules, nets, std::move(module_map), std::move(net_map));
    H2.module_up_map = std::move(module_up_map);
    H2.cluster_map = std::move(cluster_map);

    auto module_weight = std::vector<size_t>();
    for (auto v : modules) {
        if (cluster_map.contains(v)) {
            auto net = cluster_map[v];
            auto cluster_weight = 0u;
            for (auto v2 : H.G[net]) {
                cluster_weight += H.get_module_weight(v2);
            }
            module_weight.push_back(cluster_weight);
        } else {
            module_weight.push_back(H.get_module_weight(v));
        }
    }

    H2.module_weight = module_weight;
    // H2.parent = H;
    return H2;
}
