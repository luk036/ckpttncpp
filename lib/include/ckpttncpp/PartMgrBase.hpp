#pragma once

// **Special code for two-pin nets**
// Take a snapshot when a move make **negative** gain.
// Snapshot in the form of "interface"???
#include <cassert>
#include <cinttypes>
#include <iterator>
#include <py2cpp/py2cpp.hpp>
#include <type_traits>
#include <vector>
#include "netlist.hpp"

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
template<typename GainMgr, typename ConstrMgr,
         template<typename _GainMgr, typename _ConstrMgr> class Derived> //
class PartMgrBase
{

public:
    using GainCalc_  = typename GainMgr::GainCalc_;
    using GainMgr_   = GainMgr;
    using ConstrMgr_ = ConstrMgr;

    using Der = Derived<GainMgr, ConstrMgr>;
    Der& self = *static_cast<Der*>(this);

protected:
    SimpleNetlist& H;
    GainMgr&       gainMgr;
    ConstrMgr&     validator;
    uint8_t        K;
    // std::vector<uint8_t> snapshot;
    // std::vector<uint8_t> part;

public:
    int totalcost{0};

    /*!
     * @brief Construct a new FDPartMgr object
     *
     * @param H
     * @param gainMgr
     * @param constrMgr
     */
    PartMgrBase(SimpleNetlist& H, GainMgr& gainMgr, ConstrMgr& constrMgr)
        : H{H}, gainMgr{gainMgr}, validator{constrMgr}, K{gainMgr.K}
    {
    }

    /*!
     * @brief
     *
     * @param part
     */
    void init(std::vector<uint8_t>& part);

    /*!
     * @brief
     *
     * @param part
     */
    size_t legalize(std::vector<uint8_t>& part);

    /*!
     * @brief
     *
     * @param part
     */
    void optimize(std::vector<uint8_t>& part);

private:
    /*!
     * @brief
     *
     * @param part
     */
    void __optimize_1pass(std::vector<uint8_t>& part);
};
