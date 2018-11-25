#ifndef _HOME_UBUNTU_GITHUB_CKPTTNCPP_FMKWayPART_HPP
#define _HOME_UBUNTU_GITHUB_CKPTTNCPP_FMKWayPART_HPP 1

// **Special code for two-pin nets**
// Take a snapshot when a move make **negative** gain.
// Snapshot in the form of "interface"???
#include "FMKWayConstrMgr.hpp" // import FMKWayConstrMgr
#include "FMKWayGainMgr.hpp"   // import FMKWayGainMgr
#include <cassert>
#include <vector>

class FMKWayPartMgr {
  private:
    Netlist &H;
    size_t K; //> number_of_partitions
    FMKWayGainMgr &gainMgr;
    FMKWayConstrMgr &validator;
    std::vector<size_t> snapshot;
    std::vector<size_t> part;

  public:
    int totalcost;

    /**
     * @brief Construct a new FMKWayPartMgr object
     *
     * @param H
     * @param gainMgr
     * @param constrMgr
     */
    FMKWayPartMgr(Netlist &H, size_t K, FMKWayGainMgr &gainMgr,
                  FMKWayConstrMgr &constrMgr)
        : H{H}, K{K}, gainMgr{gainMgr}, validator{constrMgr}, snapshot{},
          part(this->H.number_of_cells(), 0), totalcost{0} {}

    /**
     * @brief
     *
     */
    auto init() -> void {
        this->gainMgr.init(this->part);
        this->validator.init(this->part);

        auto totalgain = 0;

        while (true) {
            auto toPart = this->validator.select_togo();
            if (this->gainMgr.is_empty(toPart)) {
                break;
            }
            auto [v, gainmax] = this->gainMgr.select_togo(toPart);
            auto fromPart = this->part[v];
            // Check if the move of v can notsatisfied, makebetter, or satisfied
            // auto weight = this->H.G.nodes[v].get('weight', 1);
            // auto weight = 10u;
            auto legalcheck = this->validator.check_legal(fromPart, toPart, v);
            if (legalcheck == 0) { // notsatisfied
                continue;
            }

            // Update v and its neigbours (even they are in waitinglist);
            // Put neigbours to bucket
            this->gainMgr.update_move(this->part, fromPart, toPart, v, gainmax);
            this->validator.update_move(fromPart, toPart, v);
            this->part[v] = toPart;
            totalgain += gainmax;

            if (legalcheck == 2) { // satisfied
                this->totalcost -= totalgain;
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
    auto optimize() -> void {
        auto totalgain = 0;
        auto deferredsnapshot = true;

        while (true) {
            auto toPart = this->validator.select_togo();
            if (this->gainMgr.is_empty(toPart)) {
                break;
            }
            auto [v, gainmax] = this->gainMgr.select_togo(toPart);
            auto fromPart = this->part[v];
            // Check if the move of v can satisfied or notsatisfied
            auto satisfiedOK =
                this->validator.check_constraints(fromPart, toPart, v);

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
            this->gainMgr.update_move(this->part, fromPart, toPart, v, gainmax);
            this->validator.update_move(fromPart, toPart, v);
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
};

#endif