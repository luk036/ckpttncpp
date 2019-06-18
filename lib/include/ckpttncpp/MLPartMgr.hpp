#ifndef CKPTTNCPP_MLPARTMGR_HPP
#define CKPTTNCPP_MLPARTMGR_HPP 1

// **Special code for two-pin nets**
// Take a snapshot when a move make **negative** gain.
// Snapshot in the form of "interface"???
// #include "FDPartMgr.hpp" // import FDPartMgr
#include "FMPartMgr.hpp" // import FMPartMgr
#include "netlist.hpp"
#include <iostream>
#include <vector>

extern std::unique_ptr<SimpleNetlist>
create_contraction_subgraph(SimpleNetlist &, const py::set<node_t> &);

/*!
 * @brief Multilevel Partition Manager
 *
 */
class MLPartMgr {
  private:
    double BalTol;
    uint8_t K;

  public:
    int totalcost{0};

    /*!
     * @brief Construct a new MLPartMgr object
     *
     * @param BalTol
     * @param K
     */
    explicit MLPartMgr(double BalTol, uint8_t K = 2)
        : BalTol{BalTol}, K{K} {}

    /*!
     * @brief run_Partition
     *
     * @tparam GainMgr
     * @tparam ConstrMgr
     * @param H
     * @param part
     * @param limitsize
     * @return size_t self.take_snapshot(part)
     */
    template <typename PartMgr>
    auto run_FMPartition(SimpleNetlist &H, std::vector<uint8_t> &part,
                         size_t limitsize = 7) -> size_t {
        using GainMgr = typename PartMgr::GainMgr_;
        using ConstrMgr = typename PartMgr::ConstrMgr_;

        // auto gainMgr = GainMgr{H, this->K};
        // auto constrMgr = ConstrMgr{H, this->BalTol, this->K};
        // auto partMgr = PartMgr{H, gainMgr, constrMgr};
        auto gainMgrPtr = std::make_unique<GainMgr>(H, this->K);
        auto constrMgrPtr =
            std::make_unique<ConstrMgr>(H, this->BalTol, this->K);
        auto partMgrPtr =
            std::make_unique<PartMgr>(H, *gainMgrPtr, *constrMgrPtr);

        // partMgrPtr->init(part);
        auto legalcheck = partMgrPtr->legalize(part);
        if (legalcheck != 2) {
            this->totalcost = partMgrPtr->totalcost;
            return legalcheck;
        }
        // auto &[part, extern_nets] = part_info;
        if (H.number_of_modules() >= limitsize) { // OK
            try {
                auto H2 = create_contraction_subgraph(H, py::set<node_t>{});
                if (5 * H2->number_of_modules() <= 3 * H.number_of_modules()) {
                    auto part2 =
                        std::vector<uint8_t>(H2->number_of_modules(), 0);
                    // auto extern_nets_ss = py::set<node_t>{};
                    // auto part2_info =
                    //     PartInfo{std::move(part2), std::move(extern_nets_ss)};
                    H2->projection_up(part, part2);
                    legalcheck = this->run_FMPartition<PartMgr>(*H2, part2,
                                                                limitsize);
                    if (legalcheck == 2) {
                        H2->projection_down(part2, part);
                    }
                }
            } catch (std::bad_alloc) {
                std::cerr << "Warning: Insufficient memory."
                          << " Discard one level." << '\n';
            }
        }
        partMgrPtr->optimize(part);
        assert(partMgrPtr->totalcost >= 0);
        this->totalcost = partMgrPtr->totalcost;

        return legalcheck;
    }

    /*!
     * @brief run_Partition
     *
     * @tparam GainMgr
     * @tparam ConstrMgr
     * @param H
     * @param part
     * @param limitsize
     * @return size_t self.take_snapshot(part)
     */
    template <typename PartMgr>
    auto run_Partition(SimpleNetlist &H, std::vector<uint8_t> &part,
                       size_t limitsize = 7) -> size_t {
        using GainMgr = typename PartMgr::GainMgr_;
        using ConstrMgr = typename PartMgr::ConstrMgr_;

        auto gainMgr = GainMgr{H, this->K};
        auto constrMgr = ConstrMgr{H, this->BalTol, this->K};
        auto partMgr = PartMgr{H, gainMgr, constrMgr};
        // partMgr.init(part);
        auto legalcheck = partMgr.legalize(part);
        if (legalcheck != 2) {
            this->totalcost = partMgr.totalcost;
            return legalcheck;
        }
        this->run_Partition_recur<PartMgr>(H, part, limitsize);
        return legalcheck;
    }

    /*!
     * @brief run_Partition_recur
     *
     * @tparam GainMgr
     * @tparam ConstrMgr
     * @param H
     * @param part
     * @param limitsize
     * @return size_t self.take_snapshot(part)
     */
    template <typename PartMgr>
    auto run_Partition_recur(SimpleNetlist &H, std::vector<uint8_t> &part,
                             size_t limitsize) -> void {
        if (H.number_of_modules() >= limitsize) { // OK
            try {
                // auto &[part, extern_nets] = part_info;
                auto H2 = create_contraction_subgraph(H, py::set<node_t>{});
                if (5 * H2->number_of_modules() <= 3 * H.number_of_modules()) {
                    auto part2 =
                        std::vector<uint8_t>(H2->number_of_modules(), 0);
                    // auto extern_nets_ss = py::set<node_t>{};
                    // auto part2_info =
                    //     PartInfo{std::move(part2), std::move(extern_nets_ss)};
                    H2->projection_up(part, part2);
                    this->run_Partition_recur<PartMgr>(*H2, part2,
                                                       limitsize);
                    H2->projection_down(part2, part);
                }
            } catch (std::bad_alloc) {
                std::cerr << "Warning: Insufficient memory."
                          << " Discard one level." << '\n';
            }
        }
        using GainMgr = typename PartMgr::GainMgr_;
        using ConstrMgr = typename PartMgr::ConstrMgr_;
        // auto gainMgr = GainMgr{H, this->K};
        // auto constrMgr = ConstrMgr{H, this->BalTol, this->K};
        // auto partMgr = PartMgr{H, gainMgr, constrMgr};
        auto gainMgrPtr = std::make_unique<GainMgr>(H, this->K);
        auto constrMgrPtr =
            std::make_unique<ConstrMgr>(H, this->BalTol, this->K);
        auto partMgrPtr =
            std::make_unique<PartMgr>(H, *gainMgrPtr, *constrMgrPtr);
        partMgrPtr->optimize(part);
        assert(partMgrPtr->totalcost >= 0);
        this->totalcost = partMgrPtr->totalcost;
    }

    // /*!
    //  * @brief run_FDPartition
    //  *
    //  * @tparam GainMgr
    //  * @tparam ConstrMgr
    //  * @param H
    //  * @param part_info
    //  * @param limitsize
    //  * @return size_t
    //  */
    // template <typename GainMgr, typename ConstrMgr>
    // auto run_FDPartition(SimpleNetlist &H, std::vector<uint8_t> &part,
    //                      size_t limitsize = 7) -> size_t {
    //     auto gainMgr = GainMgr(H, this->K);
    //     auto constrMgr = ConstrMgr(H, this->BalTol, this->K);
    //     auto partMgr = FDPartMgr(H, gainMgr, constrMgr);
    //     // auto &[part, extern_nets] = part_info;
    //     // partMgr.init(part);
    //     auto legalcheck = partMgr.legalize(part);
    //     if (legalcheck != 2) {
    //         return legalcheck;
    //     }
    //     if (H.number_of_modules() >= limitsize) { // OK
    //         auto H2 = create_contraction_subgraph(H, extern_nets);
    //         auto part2 = std::vector<uint8_t>(H2->number_of_modules(),
    //         0); auto extern_nets = py::set<node_t>{}; auto part2_info =
    //             PartInfo{std::move(part2), std::move(extern_nets)};
    //         H2->projection_up(part, part2);
    //         legalcheck = this->run_FDPartition<GainMgr, ConstrMgr>(
    //             *H2, part2, limitsize);
    //         if (legalcheck == 2) {
    //             H2->projection_down(part2, part);
    //         }
    //     }
    //     partMgr.optimize(part);
    //     assert(partMgr.totalcost >= 0);
    //     this->totalcost = partMgr.totalcost;
    //     return legalcheck;
    // }
};

#endif