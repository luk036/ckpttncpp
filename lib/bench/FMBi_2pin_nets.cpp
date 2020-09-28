#include "benchmark/benchmark.h"
#include <boost/utility/string_view.hpp>
#include <ckpttncpp/FMBiConstrMgr.hpp> // import FMBiConstrMgr
#include <ckpttncpp/FMBiGainMgr.hpp>   // import FMBiGainMgr
#include <ckpttncpp/FMPartMgr.hpp> // import FMBiPartMgr#include <string_view>

extern SimpleNetlist create_test_netlist(); // import create_test_netlist
extern SimpleNetlist create_dwarf();        // import create_dwarf
extern SimpleNetlist readNetD(boost::string_view netDFileName);
extern void readAre(SimpleNetlist& H, boost::string_view areFileName);

/**
 * @brief run FM Bi-partitioning
 *
 * @param[in] H
 * @param[in] option
 */
void run_FMBiPartMgr(const SimpleNetlist& H, bool option)
{
    auto gainMgr = FMBiGainMgr {H};
    gainMgr.gainCalc.special_handle_2pin_nets = option;

    auto constrMgr = FMBiConstrMgr {H, 0.45};
    auto partMgr =
        FMPartMgr<FMBiGainMgr, FMBiConstrMgr> {H, gainMgr, constrMgr};
    auto part = std::vector<std::uint8_t>(H.number_of_modules(), 0);
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
 * @param[in] state
 */
static void BM_with_2pin_nets(benchmark::State& state)
{
    auto H = readNetD("../../../testcases/ibm03.net");
    readAre(H, "../../../testcases/ibm03.are");

    while (state.KeepRunning())
    {
        run_FMBiPartMgr(H, true);
    }
}

// Register the function as a benchmark
BENCHMARK(BM_with_2pin_nets);

//~~~~~~~~~~~~~~~~

/*!
 * @brief Define another benchmark
 *
 * @param[in] state
 */
static void BM_without_2pin_nets(benchmark::State& state)
{
    auto H = readNetD("../../../testcases/ibm03.net");
    readAre(H, "../../../testcases/ibm03.are");

    while (state.KeepRunning())
    {
        run_FMBiPartMgr(H, false);
    }
}
BENCHMARK(BM_without_2pin_nets);

BENCHMARK_MAIN();

/*
2: ---------------------------------------------------------------
2: Benchmark                     Time             CPU   Iterations
2: ---------------------------------------------------------------
2: BM_with_2pin_nets      86441084 ns     86440541 ns            8
2: BM_without_2pin_nets   95729431 ns     95728056 ns            7
2/4 Test #2: Bench_FMBi_2pin_nets .............   Passed    1.70 sec
*/