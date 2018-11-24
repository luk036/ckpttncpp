#ifndef _HOME_UBUNTU_GITHUB_CKPTTNCPP_FMBIPART_HPP
#define _HOME_UBUNTU_GITHUB_CKPTTNCPP_FMBIPART_HPP 1

// **Special code for two-pin nets**
// Take a snapshot when a move make **negative** gain.
// Snapshot in the form of "interface"???
#include "FMBiConstrMgr.hpp" // import FMBiConstrMgr
#include "FMBiGainMgr.hpp"   // import FMBiGainMgr
#include <cassert>
#include <vector>

class FMBiPartMgr {
  private:
    Netlist &H;
    FMBiGainMgr &gainMgr;
    FMBiConstrMgr &validator;
    std::vector<size_t> snapshot;
    std::vector<size_t> part;

  public:
    int totalcost;

    /**
     * @brief Construct a new FMBiPartMgr object
     *
     * @param H
     * @param gainMgr
     * @param constrMgr
     */
    FMBiPartMgr(Netlist &H, FMBiGainMgr &gainMgr, FMBiConstrMgr &constrMgr)
        : H{H}, gainMgr{gainMgr}, validator{constrMgr}, snapshot{},
          part(this->H.number_of_cells(), 0), totalcost{0} {}

    /**
     * @brief
     *
     */
    auto init() -> void {
        this->gainMgr.init(this->part);
        this->validator.init(this->part);

        auto totalgain = 0;

        while (!this->gainMgr.is_empty()) {
            // Take the gainmax with v from gainbucket
            // auto gainmax = this->gainMgr.gainbucket.get_max();
            auto [v, gainmax] = this->gainMgr.popleft();
            auto fromPart = this->part[v];
            // Check if the move of v can notsatisfied, makebetter, or satisfied
            // auto weight = this->H.G.nodes[v].get('weight', 1);
            // auto weight = 10u;
            auto legalcheck = this->validator.check_legal(fromPart, v);
            if (legalcheck == 0) { // notsatisfied
                continue;
            }

            // Update v and its neigbours (even they are in waitinglist);
            // Put neigbours to bucket
            this->gainMgr.update_move(this->part, fromPart, v, gainmax);
            this->validator.update_move(fromPart, v);
            this->part[v] = 1 - fromPart;
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

        while (!this->gainMgr.is_empty()) {
            // Take the gainmax with v from gainbucket
            // auto gainmax = this->gainMgr.gainbucket.get_max();
            auto [v, gainmax] = this->gainMgr.popleft();

            // v = this->H.cell_list[i_v];
            auto fromPart = this->part[v];
            // Check if the move of v can satisfied or notsatisfied
            // auto weight = this->H.G.nodes[v].get('weight', 1);
            // auto weight = 10u;

            auto satisfiedOK = this->validator.check_constraints(fromPart, v);

            if (!satisfiedOK)
                continue;

            if (totalgain >= 0) {
                if (totalgain + gainmax < 0) {
                    // become down turn
                    // Take a snapshot before move
                    this->snapshot = this->part;
                    deferredsnapshot = false;
                }
            } else {  // totalgain < 0;
                if (gainmax <= 0) { // ???
                    continue;
                }
            }

            // Update v and its neigbours (even they are in waitinglist);
            // Put neigbours to bucket
            this->gainMgr.update_move(this->part, fromPart, v, gainmax);
            this->validator.update_move(fromPart, v);
            totalgain += gainmax;

            if (totalgain > 0) {
                this->totalcost -= totalgain;
                totalgain = 0; // reset to zero
                deferredsnapshot = true;
            }
            this->part[v] = 1 - fromPart;
        }
        if (deferredsnapshot) {
            // Take a snapshot
            this->snapshot = this->part;
        }
    }
};

#endif