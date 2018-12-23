#include <ckpttncpp/FDPartMgr.hpp>
#include <ckpttncpp/netlist.hpp>

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
    auto &[part, extern_nets] = part_info;
    auto totalgain = 0;
    auto deferredsnapshot = false;
    auto snapeshot = part;
    auto besttotalgain = 0;

    while (!this->gainMgr.is_empty()) {
        // Take the gainmax with v from gainbucket
        auto [move_info_v, gainmax] = this->gainMgr.select(part);
        // Check if the move of v can satisfied or notsatisfied
        auto satisfiedOK = this->validator.check_constraints(move_info_v);
        if (!satisfiedOK)
            continue;
        if (gainmax < 0) {
            // become down turn
            if (totalgain > besttotalgain) {
                // Take a snapshot before move
                snapshot = part;
                besttotalgain = totalgain;
            }
            deferredsnapshot = true;
        } else if (totalgain + gainmax > besttotalgain) {
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
        part = snapshot;
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
    while (true) {
        this->init(part_info);
        auto totalcostbefore = this->totalcost;
        this->optimize_1pass(part_info);
        assert(this->totalcost <= totalcostbefore);
        if (this->totalcost == totalcostbefore) {
            break;
        }
    }
}

#include <ckpttncpp/FMKWayConstrMgr.hpp> // import FMKWayConstrMgr
#include <ckpttncpp/FDKWayGainMgr.hpp>   // import FDKWayGainMgr
template class FDPartMgr<FDKWayGainMgr, FMKWayConstrMgr>;

#include <ckpttncpp/FMBiConstrMgr.hpp> // import FMBiConstrMgr
#include <ckpttncpp/FDBiGainMgr.hpp>  // import FDBiGainMgr
template class FDPartMgr<FDBiGainMgr, FMBiConstrMgr>;