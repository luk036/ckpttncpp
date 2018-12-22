#include <ckpttncpp/FMPartMgr.hpp>
#include <ckpttncpp/netlist.hpp>

/**
 * @brief
 *
 * @tparam FMGainMgr
 * @tparam FMConstrMgr
 * @param part
 */
template <typename FMGainMgr, typename FMConstrMgr>
auto FMPartMgr<FMGainMgr, FMConstrMgr>::init(std::vector<std::uint8_t> &part)
    -> void {
    this->gainMgr.init(part);
    this->totalcost = this->gainMgr.totalcost;
    this->validator.init(part);
}

template <typename FMGainMgr, typename FMConstrMgr>
auto FMPartMgr<FMGainMgr, FMConstrMgr>::legalize(
    std::vector<std::uint8_t> &part) -> size_t {
    size_t legalcheck = 0;
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
        this->gainMgr.update_move(part, move_info_v);
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
 * @tparam FMGainMgr
 * @tparam FMConstrMgr
 * @param part
 */
template <typename FMGainMgr, typename FMConstrMgr>
auto FMPartMgr<FMGainMgr, FMConstrMgr>::optimize_1pass(
    std::vector<std::uint8_t> &part) -> void {
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
        this->gainMgr.update_move(part, move_info_v);
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
 * @tparam FMGainMgr
 * @tparam FMConstrMgr
 * @param part
 */
template <typename FMGainMgr, typename FMConstrMgr>
auto FMPartMgr<FMGainMgr, FMConstrMgr>::optimize(
    std::vector<std::uint8_t> &part) -> void {
    while (true) {
        this->init(part);
        auto totalcostbefore = this->totalcost;
        this->optimize_1pass(part);
        assert(this->totalcost <= totalcostbefore);
        if (this->totalcost == totalcostbefore) {
            break;
        }
    }
}

#include <ckpttncpp/FMKWayConstrMgr.hpp> // import FMKWayConstrMgr
#include <ckpttncpp/FMKWayGainMgr.hpp>   // import FMKWayGainMgr
template class FMPartMgr<FMKWayGainMgr, FMKWayConstrMgr>;

#include <ckpttncpp/FMBiConstrMgr.hpp> // import FMBiConstrMgr
#include <ckpttncpp/FMBiGainMgr.hpp>  // import FMBiGainMgr
template class FMPartMgr<FMBiGainMgr, FMBiConstrMgr>;