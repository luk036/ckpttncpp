#pragma once

// **Special code for two-pin nets**
// Take a snapshot when a move make **negative** gain.
// Snapshot in the form of "interface"???
// #include "FMPartMgr.hpp" // import FMPartMgr
// #include "netlist.hpp"
#include <cassert>
#include <ckpttncpp/netlist.hpp>
#include <gsl/span>
#include <vector>
// #include <ckpttncpp/FMConstrMgr.hpp>   // import LegalCheck

// forward declare
// template <typename nodeview_t, typename nodemap_t>
// struct Netlist;
// using RngIter = decltype(py::range<int>(0, 1));
// using SimpleNetlist = Netlist<RngIter, RngIter>;

extern std::unique_ptr<SimpleNetlist> create_contraction_subgraph(
    const SimpleNetlist&, const py::set<node_t>&);

enum class LegalCheck;

/*!
 * @brief Multilevel Partition Manager
 *
 */
class MLPartMgr
{
  private:
    double BalTol;
    std::uint8_t K;

  public:
    int totalcost {};

    /*!
     * @brief Construct a new MLPartMgr object
     *
     * @param[in] BalTol
     */
    explicit MLPartMgr(double BalTol)
        : MLPartMgr(BalTol, 2)
    {
    }

    /*!
     * @brief Construct a new MLPartMgr object
     *
     * @param[in] BalTol
     * @param[in] K
     */
    MLPartMgr(double BalTol, std::uint8_t K)
        : BalTol {BalTol}
        , K {K}
    {
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
    LegalCheck run_FMPartition(
        const SimpleNetlist& H, gsl::span<std::uint8_t> part, size_t limitsize);

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
    LegalCheck run_Partition(
        const SimpleNetlist& H, gsl::span<std::uint8_t> part, size_t limitsize);

    /*!
     * @brief run_Partition
     *
     * @tparam GainMgr
     * @tparam ConstrMgr
     * @param[in] H
     * @param[in,out] part
     * @return LegalCheck
     */
    template <typename PartMgr>
    auto run_FMPartition(const SimpleNetlist& H, gsl::span<std::uint8_t> part)
        -> LegalCheck
    {
        return this->run_FMPartition<PartMgr>(H, part, 7);
    }

    /*!
     * @brief run_Partition
     *
     * @tparam GainMgr
     * @tparam ConstrMgr
     * @param[in] H
     * @param[in,out] part
     * @return LegalCheck
     */
    template <typename PartMgr>
    auto run_Partition(const SimpleNetlist& H, gsl::span<std::uint8_t> part)
        -> LegalCheck
    {
        return this->run_Partition<PartMgr>(H, part, 7);
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
    void run_Partition_recur(
        const SimpleNetlist& H, gsl::span<std::uint8_t> part, size_t limitsize);
};
