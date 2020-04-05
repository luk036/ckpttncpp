#pragma once

// **Special code for two-pin nets**
// Take a snapshot when a move make **negative** gain.
// Snapshot in the form of "interface"???
// #include "netlist.hpp"
#include <cinttypes>
#include <gsl/span>
#include <iterator>
#include <py2cpp/py2cpp.hpp>
#include <type_traits>
#include <vector>

// forward declare
template <typename nodeview_t, typename nodemap_t>
struct Netlist;
using RngIter = decltype(py::range<int>(0, 1));
using SimpleNetlist = Netlist<RngIter, RngIter>;
enum class LegalCheck;

/*!
 * @brief Partition Manager Base
 *
 * @tparam GainMgr
 * @tparam ConstrMgr
 * @tparam Derived
 *
 * Iterative Improvement Partitioning Base Class. In this
 * partitioning method, the next solution $s'$ considered after
 * solution $s$ is dervied by first applying a sequence of
 * $t$ changes (moves) to $s$ (with $t$ dependent from
 * $s$ and from the specific heuristic method), thus obtaining a
 * sequence of solution $s,...,s_t$ and by successively
 * choosing the best among these solutions.
 *
 * In order to do that, heuristics refer to a measure of the gain (and
 * balance condition) associated to any sequence of changes performed on
 * the current solution. Moreover, the length of the sequence generated
 * is determined by evaluting a suitably defined $stopping rule$ at
 * each iteration.
 *
 * Reference:
 *   G. Ausiello et al., Complexity and Approximation: Combinatorial
 * Optimization Problems and Their Approximability Properties, Section 10.3.2.
 */
template <typename GainMgr, typename ConstrMgr,
    template <typename _GainMgr, typename _ConstrMgr> class Derived> //
class PartMgrBase
{

  public:
    using GainCalc_ = typename GainMgr::GainCalc_;
    using GainMgr_ = GainMgr;
    using ConstrMgr_ = ConstrMgr;

    using Der = Derived<GainMgr, ConstrMgr>;
    Der& self = *static_cast<Der*>(this);

  protected:
    const SimpleNetlist& H;
    GainMgr& gainMgr;
    ConstrMgr& validator;
    std::uint8_t K;
    // std::vector<std::uint8_t> snapshot;
    // std::vector<std::uint8_t> part;

  public:
    int totalcost {};

    /*!
     * @brief Construct a new FDPartMgr object
     *
     * @param[in] H
     * @param[in,out] gainMgr
     * @param[in,out] constrMgr
     */
    PartMgrBase(const SimpleNetlist& H, GainMgr& gainMgr, ConstrMgr& constrMgr)
        : H {H}
        , gainMgr {gainMgr}
        , validator {constrMgr}
        , K {gainMgr.K}
    {
    }

    /*!
     * @brief
     *
     * @param[in,out] part
     */
    void init(gsl::span<std::uint8_t> part);

    /*!
     * @brief
     *
     * @param[in,out] part
     * @return LegalCheck
     */
    LegalCheck legalize(gsl::span<std::uint8_t> part);

    /*!
     * @brief
     *
     * @param[in,out] part
     */
    void optimize(gsl::span<std::uint8_t> part);

  private:
    /*!
     * @brief
     *
     * @param[in,out] part
     */
    void _optimize_1pass(gsl::span<std::uint8_t> part);
};
