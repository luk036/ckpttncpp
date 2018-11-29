#include <ckpttncpp/FMKWayPart.hpp>
#include <ckpttncpp/FMKWayConstrMgr.hpp> // import FMKWayConstrMgr
#include <ckpttncpp/FMKWayGainMgr.hpp>   // import FMKWayGainMgr

/**
 * @brief
 *
 */
void FMKWayPartMgr::init()
{
    this->gainMgr.init(this->part);
    this->validator.init(this->part);
    // auto totalgain = 0;
    while (true) {
        auto toPart = this->validator.select_togo();
        if (this->gainMgr.is_empty(toPart)) {
            break;
        }
        auto [v, gainmax] = this->gainMgr.select_togo(toPart);
        auto fromPart = this->part[v];
        assert(fromPart != toPart);
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
 */
void FMKWayPartMgr::optimize()
{
    auto totalgain = 0;
    auto deferredsnapshot = true;
    while (true) {
        auto toPart = this->validator.select_togo();
        if (this->gainMgr.is_empty(toPart)) {
            break;
        }
        auto [v, gainmax] = this->gainMgr.select_togo(toPart);
        auto fromPart = this->part[v];
        assert(fromPart != toPart);
        auto move_info_v = MoveInfoV{fromPart, toPart, v};
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
        this->part[v] = toPart;
    }
    if (deferredsnapshot) {
        // Take a snapshot
        this->snapshot = this->part;
    }
}
