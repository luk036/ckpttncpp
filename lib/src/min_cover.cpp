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
