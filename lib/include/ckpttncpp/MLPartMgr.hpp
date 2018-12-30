#ifndef _HOME_UBUNTU_GITHUB_CKPTTNCPP_MLPARTMGR_HPP
#define _HOME_UBUNTU_GITHUB_CKPTTNCPP_MLPARTMGR_HPP 1

// **Special code for two-pin nets**
// Take a snapshot when a move make **negative** gain.
// Snapshot in the form of "interface"???
#include "FDPartMgr.hpp" // import FDPartMgr
#include "FMPartMgr.hpp" // import FMPartMgr
#include "netlist.hpp"
#include <vector>

extern SimpleNetlist create_contraction_subgraph(SimpleNetlist &,
                                                 const py::set<node_t> &);

/**
 * @brief Multilevel Partition Manager
 * 
 */
class MLPartMgr {
  private:
    double BalTol;
    std::uint8_t K;

  public:
    int totalcost;

    /**
     * @brief Construct a new MLPartMgr object
     * 
     * @param BalTol 
     * @param K 
     */
    MLPartMgr(double BalTol, std::uint8_t K = 2)
        : BalTol{BalTol}, K{K}, totalcost{0} {}

    /**
     * @brief run_Partition
     * 
     * @tparam GainMgr 
     * @tparam ConstrMgr 
     * @param H 
     * @param part 
     * @param limitsize 
     * @return size_t 
     */
    template <typename GainMgr, typename ConstrMgr>
    auto run_Partition(SimpleNetlist &H, std::vector<std::uint8_t> &part,
                       size_t limitsize = 7) -> size_t {
        auto gainMgr = GainMgr(H, this->K);
        auto constrMgr = ConstrMgr(H, this->BalTol, this->K);
        auto partMgr = FMPartMgr(H, gainMgr, constrMgr);
        // partMgr.init(part);
        auto legalcheck = partMgr.legalize(part);
        if (legalcheck != 2) {
            return legalcheck;
        }
        if (H.number_of_modules() >= limitsize) { // OK
            auto H2 = create_contraction_subgraph(H, py::set<node_t>{});
            auto part2 = std::vector<std::uint8_t>(H2.number_of_modules(), 0);
            H2.project_up(part, part2);
            legalcheck =
                this->run_Partition<GainMgr, ConstrMgr>(H2, part2, limitsize);
            if (legalcheck == 2) {
                H2.project_down(part2, part);
            }
        }
        partMgr.optimize(part);
        assert(partMgr.totalcost >= 0);
        this->totalcost = partMgr.totalcost;
        return legalcheck;
    }

    /**
     * @brief run_FDPartition
     * 
     * @tparam GainMgr 
     * @tparam ConstrMgr 
     * @param H 
     * @param part_info 
     * @param limitsize 
     * @return size_t 
     */
    template <typename GainMgr, typename ConstrMgr>
    auto run_FDPartition(SimpleNetlist &H, PartInfo &part_info,
                         size_t limitsize = 7) -> size_t {
        auto gainMgr = GainMgr(H, this->K);
        auto constrMgr = ConstrMgr(H, this->BalTol, this->K);
        auto partMgr = FDPartMgr(H, gainMgr, constrMgr);
        auto &[part, extern_nets] = part_info;
        // partMgr.init(part);
        auto legalcheck = partMgr.legalize(part_info);
        if (legalcheck != 2) {
            return legalcheck;
        }
        if (H.number_of_modules() >= limitsize) { // OK
            auto H2 = create_contraction_subgraph(H, extern_nets);
            auto part2 = std::vector<std::uint8_t>(H2.number_of_modules(), 0);
            auto extern_nets = py::set<size_t>();
            auto part2_info =
                PartInfo{std::move(part2), std::move(extern_nets)};
            H2.projection_up(part_info, part2_info);
            legalcheck = this->run_FDPartition<GainMgr, ConstrMgr>(
                H2, part2_info, limitsize);
            if (legalcheck == 2) {
                H2.projection_down(part2_info, part_info);
            }
        }
        partMgr.optimize(part_info);
        assert(partMgr.totalcost >= 0);
        this->totalcost = partMgr.totalcost;
        return legalcheck;
    }
};

#endif