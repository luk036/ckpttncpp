#include "benchmark/benchmark.h"
#include <ckpttncpp/FMKWayConstrMgr.hpp> // import FMKWayConstrMgr
#include <ckpttncpp/FMKWayGainMgr.hpp>   // import FMKWayGainMgr
#include <ckpttncpp/FMPartMgr.hpp>       // import FMKWayPartMgr

extern SimpleNetlist create_test_netlist(); // import create_test_netlist
extern SimpleNetlist create_dwarf();        // import create_dwarf
extern SimpleNetlist readNetD(std::string_view netDFileName);
extern void readAre(SimpleNetlist& H, std::string_view areFileName);

/**
 * @brief Run test cases
 *
 * @param H
 * @param K
 */
void run_FMKWayPartMgr(SimpleNetlist& H, uint8_t K, bool option)
{
    auto gainMgr = FMKWayGainMgr {H, K};
    gainMgr.gainCalc.special_handle_2pin_nets = option;

    auto constrMgr = FMKWayConstrMgr {H, 0.4, K};
    auto partMgr = FMPartMgr {H, gainMgr, constrMgr};
    auto part = std::vector<uint8_t>(H.number_of_modules(), 0);

    partMgr.legalize(part);
    // auto totalcostbefore = partMgr.totalcost;
    partMgr.optimize(part);
    // CHECK(totalcostbefore >= 0);
    // CHECK(partMgr.totalcost <= totalcostbefore);
    // CHECK(partMgr.totalcost >= 0);
}

/*!
 * @brief
 *
 * @param state
 */
static void BM_with_2pin_nets(benchmark::State& state)
{
    auto H = readNetD("../../../testcases/ibm01.net");
    readAre(H, "../../../testcases/ibm01.are");

    while (state.KeepRunning())
    {
        run_FMKWayPartMgr(H, 3, true);
    }
}

// Register the function as a benchmark
BENCHMARK(BM_with_2pin_nets);

//~~~~~~~~~~~~~~~~

/*!
 * @brief Define another benchmark
 *
 * @param state
 */
static void BM_without_2pin_nets(benchmark::State& state)
{
    auto H = readNetD("../../../testcases/ibm01.net");
    readAre(H, "../../../testcases/ibm01.are");

    while (state.KeepRunning())
    {
        run_FMKWayPartMgr(H, 3, false);
    }
}
BENCHMARK(BM_without_2pin_nets);

BENCHMARK_MAIN();

/*
3: ---------------------------------------------------------------
3: Benchmark                     Time             CPU   Iterations
3: ---------------------------------------------------------------
3: BM_with_2pin_nets     463584957 ns    463577351 ns            2
3: BM_without_2pin_nets  492702676 ns    492693494 ns            2
3/4 Test #3: Bench_FMKWay_2pin_nets ...........   Passed    3.02 sec
*/