#include <ckpttncpp/FDPartMgr.hpp>
#include <ckpttncpp/netlist.hpp>
#include <deque>

template <typename GainMgr, typename ConstrMgr>
auto FDPartMgr<GainMgr, ConstrMgr>::take_snapshot(const PartInfo &part_info)
    -> Snapshot {
    auto const &[part, extern_nets] = part_info;
    auto extern_nets_ss = extern_nets.copy();
    auto extern_modules_ss = py::dict<size_t, std::uint8_t>();
    for (auto net : extern_nets) {
        for (auto v : this->H.G[net]) {
            extern_modules_ss[v] = part[v];
        }
    }
    return Snapshot{std::move(extern_nets_ss), std::move(extern_modules_ss)};
}

template <typename GainMgr, typename ConstrMgr>
auto FDPartMgr<GainMgr, ConstrMgr>::restore_part_info(Snapshot &snapshot)
    -> PartInfo {
    auto &[extern_nets_ss, extern_modules_ss] = snapshot;
    auto part = std::vector<std::uint8_t>(this->H.number_of_modules(), this->K);
    auto Q = std::deque<node_t>();
    // (v for v, _ in extern_modules_ss.items())
    for (auto const &[v, part_v] : extern_modules_ss) {
        Q.push_back(v);
    }
    while (!Q.empty()) {
        auto v = Q.front();
        Q.pop_front();
        if (part[v] < this->K) {
            continue;
        }
        std::uint8_t part_v = extern_modules_ss[v];
        part[v] = part_v;
        auto Q2 = std::deque<node_t>();
        Q2.push_back(v);
        while (!Q2.empty()) {
            auto v2 = Q2.front();
            Q2.pop_front();
            // if (part[v2] < this->K) {
            //     continue;
            for (auto net : this->H.G[v2]) {
                if (extern_nets_ss.contains(net)) {
                    continue;
                }
                if (this->H.G.degree(net) < 2) {
                    continue;
                }
                for (auto v3 : this->H.G[net]) {
                    if (part[v3] < this->K) {
                        continue;
                    }
                    part[v3] = part_v;
                    Q2.push_back(v3);
                }
            }
        }
    }
    py::set<node_t> extern_nets{};
    extern_nets.swap(extern_nets_ss);
    return PartInfo{std::move(part), std::move(extern_nets)};
}

#include <ckpttncpp/FDKWayGainMgr.hpp>   // import FDKWayGainMgr
#include <ckpttncpp/FMKWayConstrMgr.hpp> // import FMKWayConstrMgr
template class FDPartMgr<FDKWayGainMgr, FMKWayConstrMgr>;

#include <ckpttncpp/FDBiGainMgr.hpp>   // import FDBiGainMgr
#include <ckpttncpp/FMBiConstrMgr.hpp> // import FMBiConstrMgr
template class FDPartMgr<FDBiGainMgr, FMBiConstrMgr>;