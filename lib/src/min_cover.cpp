#include <ckpttncpp/netlist.hpp>
#include <py2cpp/py2cpp.hpp>
#include <tuple>

// Primal-dual algorithm for (auto minimum vertex cover problem

auto min_net_cover_pd(Netlist& H, const std::vector<size_t>& weight) {
    auto S = py::set<node_t>{};
    auto is_covered = py::set<node_t>{};
    auto gap = H.net_weight.empty() ? std::vector<size_t>(H.number_of_nets(), 1) : H.net_weight;
    auto total_primal_cost = 0u;
    auto total_dual_cost = 0u;
    auto offset = H.number_of_modules();

    for (auto v = 0u; v < offset; ++v) {
        if (is_covered.contains(v)) {
            continue;
        }
        auto min_gap = 10000000;
        auto i_s = 0;
        for (auto net : H.G[v]) {
            auto i_net = net - offset;
            if (min_gap > gap[i_net]) {
                i_s = i_net;
                min_gap = gap[i_net];
            }
        }
        // is_net_cover[i_s] = True
        // S.append(i_s)
        S.insert(i_s);
        for (auto net : H.G[v]) {
            auto i_net = net - offset;
            gap[i_net] -= min_gap;
        }
        assert( gap[i_s] == 0 );
        for (auto v2 : H.G[i_s + offset]) {
            is_covered.insert(v2);
        }
        total_primal_cost += H.get_net_weight(i_s + offset);
        total_dual_cost += min_gap;
    }

    // for (auto net : S) {
    //     found = False
    //     leda:: node w
    //     forall_adj_nodes(w, v)) {
    //         if (not is_net_cover[w]) {
    //             found = True
    //             break

    //     if (not found) {
    //         S.del_item(it)
    //         total_primal_cost -= weight[v]
    //         is_net_cover[v] = False

    assert(total_primal_cost >= total_dual_cost);
    return std::tuple{std::move(S), total_primal_cost};
}
