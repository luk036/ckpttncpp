#include <ckpttncpp/FMConstrMgr.hpp> // import LegalCheck
#include <ckpttncpp/MLPartMgr.hpp>
#include <iostream>

/*!
 * @brief run_Partition
 *
 * @tparam GainMgr
 * @tparam ConstrMgr
 * @param[in] H
 * @param[in,out] part
 * @param[in] limitsize
 * @return LegalCheck
 */
template <typename PartMgr>
LegalCheck MLPartMgr::run_FMPartition(
    const SimpleNetlist& H, gsl::span<std::uint8_t> part, size_t limitsize)
{
    using GainMgr = typename PartMgr::GainMgr_;
    using ConstrMgr = typename PartMgr::ConstrMgr_;
    // auto gainMgr = GainMgr{H, this->K};
    // auto constrMgr = ConstrMgr{H, this->BalTol, this->K};
    // auto partMgr = PartMgr{H, gainMgr, constrMgr};
    const auto gainMgrPtr = std::make_unique<GainMgr>(H, this->K);
    const auto constrMgrPtr =
        std::make_unique<ConstrMgr>(H, this->BalTol, this->K);
    const auto partMgrPtr =
        std::make_unique<PartMgr>(H, *gainMgrPtr, *constrMgrPtr);
    // partMgrPtr->init(part);
    auto legalcheck = partMgrPtr->legalize(part);
    if (legalcheck != LegalCheck::allsatisfied)
    {
        this->totalcost = partMgrPtr->totalcost;
        return legalcheck;
    }
    if (H.number_of_modules() >= limitsize)
    { // OK
        try
        {
            const auto H2 = create_contraction_subgraph(H, py::set<node_t> {});
            if (5 * H2->number_of_modules() <= 3 * H.number_of_modules())
            {
                auto part2 =
                    std::vector<std::uint8_t>(H2->number_of_modules(), 0);
                // auto extern_nets_ss = py::set<node_t>{};
                // auto part2_info =
                //     PartInfo{std::move(part2),
                //     std::move(extern_nets_ss)};
                H2->projection_up(part, part2);
                legalcheck =
                    this->run_FMPartition<PartMgr>(*H2, part2, limitsize);
                if (legalcheck == LegalCheck::allsatisfied)
                {
                    H2->projection_down(part2, part);
                }
            }
        }
        catch (const std::bad_alloc&)
        {
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
 * @param[in] H
 * @param[in,out] part
 * @param[in] limitsize
 * @return LegalCheck
 */
template <typename PartMgr>
LegalCheck MLPartMgr::run_Partition(
    const SimpleNetlist& H, gsl::span<std::uint8_t> part, size_t limitsize)
{
    using GainMgr = typename PartMgr::GainMgr_;
    using ConstrMgr = typename PartMgr::ConstrMgr_;

    auto gainMgr = GainMgr {H, this->K};
    auto constrMgr = ConstrMgr {H, this->BalTol, this->K};
    auto partMgr = PartMgr {H, gainMgr, constrMgr};
    // partMgr.init(part);
    auto legalcheck = partMgr.legalize(part);
    if (legalcheck != LegalCheck::allsatisfied)
    {
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
 * @param[in] H
 * @param[in,out] part
 * @param[in] limitsize
 * @return size_t self.take_snapshot(part)
 */
template <typename PartMgr>
void MLPartMgr::run_Partition_recur(
    const SimpleNetlist& H, gsl::span<std::uint8_t> part, size_t limitsize)
{
    if (H.number_of_modules() >= limitsize)
    { // OK
        try
        {
            const auto H2 = create_contraction_subgraph(H, py::set<node_t> {});
            if (5 * H2->number_of_modules() <= 3 * H.number_of_modules())
            {
                auto part2 =
                    std::vector<std::uint8_t>(H2->number_of_modules(), 0);
                // auto extern_nets_ss = py::set<node_t>{};
                // auto part2_info =
                //     PartInfo{std::move(part2),
                //     std::move(extern_nets_ss)};
                H2->projection_up(part, part2);
                this->run_Partition_recur<PartMgr>(*H2, part2, limitsize);
                H2->projection_down(part2, part);
            }
        }
        catch (const std::bad_alloc&)
        {
            std::cerr << "Warning: Insufficient memory."
                      << " Discard one level." << '\n';
        }
    }
    using GainMgr = typename PartMgr::GainMgr_;
    using ConstrMgr = typename PartMgr::ConstrMgr_;
    // auto gainMgr = GainMgr{H, this->K};
    // auto constrMgr = ConstrMgr{H, this->BalTol, this->K};
    // auto partMgr = PartMgr{H, gainMgr, constrMgr};
    const auto gainMgrPtr = std::make_unique<GainMgr>(H, this->K);
    const auto constrMgrPtr =
        std::make_unique<ConstrMgr>(H, this->BalTol, this->K);
    const auto partMgrPtr =
        std::make_unique<PartMgr>(H, *gainMgrPtr, *constrMgrPtr);
    partMgrPtr->optimize(part);
    assert(partMgrPtr->totalcost >= 0);
    this->totalcost = partMgrPtr->totalcost;
}

#include <ckpttncpp/FMPartMgr.hpp> // import FMBiPartMgr

#include <ckpttncpp/FMBiConstrMgr.hpp> // import FMBiConstrMgr
#include <ckpttncpp/FMBiGainMgr.hpp>   // import FMBiGainMgr

template LegalCheck
MLPartMgr::run_FMPartition<FMPartMgr<FMBiGainMgr, FMBiConstrMgr>>(
    const SimpleNetlist& H, gsl::span<std::uint8_t> part, size_t limitsize);

#include <ckpttncpp/FMKWayConstrMgr.hpp> // import FMKWayConstrMgr
#include <ckpttncpp/FMKWayGainMgr.hpp>   // import FMKWayGainMgr

template LegalCheck
MLPartMgr::run_FMPartition<FMPartMgr<FMKWayGainMgr, FMKWayConstrMgr>>(
    const SimpleNetlist& H, gsl::span<std::uint8_t> part, size_t limitsize);
