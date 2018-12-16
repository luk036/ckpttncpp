#ifndef _HOME_UBUNTU_GITHUB_CKPTTNCPP_MLPARTMGR_HPP
#define _HOME_UBUNTU_GITHUB_CKPTTNCPP_MLPARTMGR_HPP 1

// **Special code for two-pin nets**
// Take a snapshot when a move make **negative** gain.
// Snapshot in the form of "interface"???
#include "FMPartMgr.hpp" // import FMPartMgr
#include "netlist.hpp"
#include <vector>

extern SimpleNetlist create_contraction_subgraph(SimpleNetlist &);

class MLPartMgr {
  private:
    double BalTol;
    std::uint8_t K;

  public:
    int totalcost;

    MLPartMgr(double BalTol, std::uint8_t K=2) 
      : BalTol{BalTol}, K{K}, totalcost{0} {}

    template <typename GainMgr, typename ConstrMgr>  
    auto run_Partition(SimpleNetlist &H, std::vector<std::uint8_t> &part) -> size_t {
        auto gainMgr = GainMgr(H, this->K);
        auto constrMgr = ConstrMgr(H, this->BalTol, this->K);
        auto partMgr = FMPartMgr(H, gainMgr, constrMgr);
        partMgr.init(part);
        auto legalcheck = partMgr.legalize(part);
        if (legalcheck == 2 && H.number_of_modules() >= 7) { // OK
            auto H2 = create_contraction_subgraph(H);
            auto part2 = std::vector<std::uint8_t>(H2.number_of_modules(), 0);
            H2.project_up(part, part2);
            auto legalcheck = this->run_Partition<GainMgr, ConstrMgr>(H2, part2);
            H2.project_down(part2, part);
            partMgr.init(part);
            if (legalcheck != 2) {
                legalcheck = partMgr.legalize(part);
            }
        }
        partMgr.optimize(part);
        this->totalcost = partMgr.totalcost;
        return legalcheck;
    }
};

#endif