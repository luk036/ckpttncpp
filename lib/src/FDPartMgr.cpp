#include <ckpttncpp/FDPartMgr.hpp>
#include <ckpttncpp/netlist.hpp>
#include <deque>

/**
 * @brief
 *
 * @tparam FDGainMgr
 * @tparam FDConstrMgr
 * @param part
 */
template <typename FDGainMgr, typename FDConstrMgr>
auto FDPartMgr<FDGainMgr, FDConstrMgr>::init(PartInfo &part_info)
    -> void {
    this->totalcost = this->gainMgr.init(part_info);
    // this->totalcost = this->gainMgr.totalcost;
    auto const &[part, extern_nets] = part_info;
    this->validator.init(part);
}

template <typename FDGainMgr, typename FDConstrMgr>
auto FDPartMgr<FDGainMgr, FDConstrMgr>::legalize(
    PartInfo &part_info) -> size_t {
    size_t legalcheck = 0;
    this->init(part_info);
    auto &[part, extern_nets] = part_info;
    while (true) {
        auto toPart = this->validator.select_togo();
        if (this->gainMgr.is_empty_togo(toPart)) {
            break;
        }
        auto [v, gainmax] = this->gainMgr.select_togo(toPart);
        auto fromPart = part[v];
        // assert(v == v);
        assert(fromPart != toPart);
        auto move_info_v = MoveInfoV{fromPart, toPart, v};
        // Check if the move of v can notsatisfied, makebetter, or satisfied
        legalcheck = this->validator.check_legal(move_info_v);
        if (legalcheck == 0) { // notsatisfied
            continue;
        }
        // Update v and its neigbours (even they are in waitinglist);
        // Put neigbours to bucket
        this->gainMgr.update_move(part_info, move_info_v);
        this->gainMgr.update_move_v(part, move_info_v, gainmax);
        this->validator.update_move(move_info_v);
        part[v] = toPart;
        // totalgain += gainmax;
        this->totalcost -= gainmax;
        assert(this->totalcost >= 0);
        if (legalcheck == 2) { // satisfied
            // this->totalcost -= totalgain;
            // totalgain = 0; // reset to zero
            break;
        }
    }
    return legalcheck;
}

/**
 * @brief
 *
 * @tparam FDGainMgr
 * @tparam FDConstrMgr
 * @param part
 */
template <typename FDGainMgr, typename FDConstrMgr>
auto FDPartMgr<FDGainMgr, FDConstrMgr>::optimize_1pass(
    PartInfo &part_info) -> void {
    auto totalgain = 0;
    auto deferredsnapshot = false;
    // auto snapshot = part;
    auto snapshot = Snapshot{};
    auto besttotalgain = -1;
    auto &[part, extern_nets] = part_info;

    while (!this->gainMgr.is_empty()) {
        // Take the gainmax with v from gainbucket
        auto [move_info_v, gainmax] = this->gainMgr.select(part);
        // Check if the move of v can satisfied or notsatisfied
        auto satisfiedOK = this->validator.check_constraints(move_info_v);
        if (!satisfiedOK)
            continue;
        if (gainmax < 0) {
            // become down turn
            if (!deferredsnapshot || totalgain > besttotalgain) {
                // Take a snapshot before move
                // snapshot = part;
                snapshot = this->take_snapshot(part_info);
                besttotalgain = totalgain;
            }
            deferredsnapshot = true;
        } else if (totalgain + gainmax > besttotalgain) {
            besttotalgain = totalgain + gainmax;
            deferredsnapshot = false;
        }
        // Update v and its neigbours (even they are in waitinglist);
        // Put neigbours to bucket
        this->gainMgr.update_move(part_info, move_info_v);
        this->gainMgr.update_move_v(part, move_info_v, 2*this->gainMgr.get_pmax());
        this->validator.update_move(move_info_v);
        totalgain += gainmax;
        auto const &[fromPart, toPart, v] = move_info_v;
        part[v] = toPart;
    }
    if (deferredsnapshot) {
        // restore the previous best solution
        // part = snapshot;
        part_info = this->restore_part_info(snapshot);
        totalgain = besttotalgain;
    }
    this->totalcost -= totalgain;
}

/**
 * @brief
 *
 * @tparam FDGainMgr
 * @tparam FDConstrMgr
 * @param part
 */
template <typename FDGainMgr, typename FDConstrMgr>
auto FDPartMgr<FDGainMgr, FDConstrMgr>::optimize(
    PartInfo &part_info) -> void {
    this->init(part_info);
    auto totalcostafter = this->totalcost;
    while (true) {
        this->init(part_info);
        auto totalcostbefore = this->totalcost;
        assert(totalcostafter == totalcostbefore);
        this->optimize_1pass(part_info);
        assert(this->totalcost <= totalcostbefore);
        if (this->totalcost == totalcostbefore) {
            break;
        }
        totalcostafter = this->totalcost;
    }
}

template <typename FDGainMgr, typename FDConstrMgr>
auto FDPartMgr<FDGainMgr, FDConstrMgr>::take_snapshot(
    const PartInfo &part_info) -> Snapshot {
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

template <typename FDGainMgr, typename FDConstrMgr>
auto FDPartMgr<FDGainMgr, FDConstrMgr>::restore_part_info(
    Snapshot &snapshot) -> PartInfo 
{
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
    auto extern_nets = extern_nets_ss.copy();
    return PartInfo{std::move(part), std::move(extern_nets)};
}

#include <ckpttncpp/FMKWayConstrMgr.hpp> // import FMKWayConstrMgr
#include <ckpttncpp/FDKWayGainMgr.hpp>   // import FDKWayGainMgr
template class FDPartMgr<FDKWayGainMgr, FMKWayConstrMgr>;

#include <ckpttncpp/FMBiConstrMgr.hpp> // import FMBiConstrMgr
#include <ckpttncpp/FDBiGainMgr.hpp>  // import FDBiGainMgr
template class FDPartMgr<FDBiGainMgr, FMBiConstrMgr>;