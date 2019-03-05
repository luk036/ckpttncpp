#include <ckpttncpp/FDPartMgr.hpp>
#include <ckpttncpp/netlist.hpp>
#include <deque>

template <typename GainMgr, typename ConstrMgr>
auto FDPartMgr<GainMgr, ConstrMgr>::take_snapshot(const PartInfo &part_info)
    -> Snapshot {
    auto const &[part, extern_nets] = part_info;
    auto extern_nets_ss = extern_nets.copy();
    auto extern_modules_ss = py::dict<size_t, std::uint8_t>{};
    extern_modules_ss.reserve(3 * extern_nets.size());
    for (auto net : extern_nets) {
        for (auto v : this->H.G[net]) {
            auto i_v = this->H.module_map[v];
            extern_modules_ss[v] = part[i_v];
        }
    }
    return Snapshot{std::move(extern_nets_ss), std::move(extern_modules_ss)};
}

template <typename GainMgr, typename ConstrMgr>
auto FDPartMgr<GainMgr, ConstrMgr>::restore_part_info(Snapshot &snapshot,
                                                      PartInfo &part_info)
    -> void {
    auto &[extern_nets_ss, extern_modules_ss] = snapshot;
    auto &[part, extern_nets] = part_info;
    std::fill(part.begin(), part.end(), this->K);
    for (auto const &[v, part_v] : extern_modules_ss) {
        auto i_v = this->H.module_map[v];
        if (part[i_v] < this->K) {
            continue;
        }
        part[i_v] = part_v;
        std::deque Q = {v};
        while (!Q.empty()) {
            auto v2 = Q.front();
            Q.pop_front();
            for (auto net : this->H.G[v2]) {
                if (this->H.G.degree(net) < 2) {
                    continue;
                }
                if (extern_nets_ss.contains(net)) {
                    continue;
                }
                for (auto v3 : this->H.G[net]) {
                    auto i_v3 = this->H.module_map[v3];
                    if (part[i_v3] < this->K) {
                        continue;
                    }
                    part[i_v3] = part_v;
                    Q.push_back(v3);
                }
            }
        }
    }
    extern_nets.swap(extern_nets_ss);
}

#include <ckpttncpp/FDKWayGainMgr.hpp>   // import FDKWayGainMgr
#include <ckpttncpp/FMKWayConstrMgr.hpp> // import FMKWayConstrMgr
template class FDPartMgr<FDKWayGainMgr, FMKWayConstrMgr>;

#include <ckpttncpp/FDBiGainMgr.hpp>   // import FDBiGainMgr
#include <ckpttncpp/FMBiConstrMgr.hpp> // import FMBiConstrMgr
template class FDPartMgr<FDBiGainMgr, FMBiConstrMgr>;