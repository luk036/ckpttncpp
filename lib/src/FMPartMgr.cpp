#include <ckpttncpp/FMPartMgr.hpp>

/**
 * @brief 
 * 
 * @tparam FMGainMgr 
 * @tparam FMConstrMgr 
 */
template <typename FMGainMgr, typename FMConstrMgr>
void FMPartMgr<FMGainMgr, FMConstrMgr>::init()
{
    this->gainMgr.init(this->part);
    this->validator.init(this->part);
    // auto totalgain = 0;
    while (true) {
        auto toPart = this->validator.select_togo();
        if (this->gainMgr.is_empty_togo(toPart)) {
            break;
        }
        auto [v, gainmax] = this->gainMgr.select_togo(toPart);
        auto fromPart = this->part[v];
        if (fromPart == toPart) {
            toPart = 1-fromPart;
        }
        auto move_info_v = MoveInfoV{fromPart, toPart, v};
        // Check if the move of v can notsatisfied, makebetter, or satisfied
        auto legalcheck = this->validator.check_legal(move_info_v);
        if (legalcheck == 0) { // notsatisfied
            continue;
        }
        // Update v and its neigbours (even they are in waitinglist);
        // Put neigbours to bucket
        this->gainMgr.update_move(this->part, move_info_v, gainmax);
        this->validator.update_move(move_info_v);
        this->part[v] = toPart;
        // totalgain += gainmax;
        this->totalcost -= gainmax;
        if (legalcheck == 2) { // satisfied
            // this->totalcost -= totalgain;
            // totalgain = 0; // reset to zero
            break;
        }
    }
    // assert(!this->gainMgr.gainbucket.is_empty());
}

/**
 * @brief 
 * 
 * @tparam FMGainMgr 
 * @tparam FMConstrMgr 
 */
template <typename FMGainMgr, typename FMConstrMgr>
void FMPartMgr<FMGainMgr, FMConstrMgr>::optimize()
{
    auto totalgain = 0;
    auto deferredsnapshot = true;
    while (!this->gainMgr.is_empty()) {
        // Take the gainmax with v from gainbucket
        auto [move_info_v, gainmax] = this->gainMgr.select(part);
        // Check if the move of v can satisfied or notsatisfied
        auto satisfiedOK =
            this->validator.check_constraints(move_info_v);
        if (!satisfiedOK)
            continue;
        if (totalgain >= 0) {
            if (totalgain + gainmax < 0) {
                // become down turn
                // Take a snapshot before move
                this->snapshot = this->part;
                deferredsnapshot = false;
            }
        }
        else {  // totalgain < 0;
            if (gainmax <= 0) { // ???
                continue;
            }
        }
        // Update v and its neigbours (even they are in waitinglist);
        // Put neigbours to bucket
        this->gainMgr.update_move(this->part, move_info_v, gainmax);
        this->validator.update_move(move_info_v);
        totalgain += gainmax;
        if (totalgain > 0) {
            this->totalcost -= totalgain;
            totalgain = 0; // reset to zero
            deferredsnapshot = true;
        }
        auto const &[fromPart, toPart, v] = move_info_v;
        this->part[v] = toPart;
    }
    if (deferredsnapshot) {
        // Take a snapshot
        this->snapshot = this->part;
    }
}

#include <ckpttncpp/FMKWayConstrMgr.hpp> // import FMKWayConstrMgr
#include <ckpttncpp/FMKWayGainMgr.hpp>   // import FMKWayGainMgr

template void FMPartMgr<FMKWayGainMgr, FMKWayConstrMgr>::init();
template void FMPartMgr<FMKWayGainMgr, FMKWayConstrMgr>::optimize();

#include <ckpttncpp/FMBiConstrMgr.hpp> // import FMBiConstrMgr
#include <ckpttncpp/FMBiGainMgr.hpp>   // import FMBiGainMgr

template void FMPartMgr<FMBiGainMgr, FMBiConstrMgr>::init();
template void FMPartMgr<FMBiGainMgr, FMBiConstrMgr>::optimize();
