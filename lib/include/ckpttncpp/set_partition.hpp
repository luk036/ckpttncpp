#pragma once

#include <type_traits>

template <int N, int K>
constexpr auto Stirling2nd()
{
    if constexpr (K >= N || K <= 1)
    {
        return std::integral_constant<int, 1> {};
    }
    else
    {
        return std::integral_constant<int,
            Stirling2nd<N - 1, K - 1>() + K * Stirling2nd<N - 1, K>()> {};
    }
}

// Set Partition
//
// A set partition of the set [n] = {1,2,3,...,n} is a collection B0,
// B1, ... Bj of disjoint subsets of [n] whose union is [n]. Each Bj
// is called a block. Below we show the partitions of [4]. The periods
// separtate the individual sets so that, for example, 1.23.4 is the
// partition {{1},{2,3},{4}}.
//   1 block:  1234
//   2 blocks: 123.4   124.3   134.2   1.234   12.34   13.24   14.23
//   3 blocks: 1.2.34  1.24.3  14.2.3  13.2.4  12.3.4
//   4 blocks: 1.2.3.4
//
// Each partition above has its blocks listed in increasing order of
// smallest element; thus block 0 contains element 1, block1 contains
// the smallest element not in block 0, and so on. A Restricted Growth
// string (or RG string) is a sring a[1..n] where a[i] is the block in
// whcih element i occurs. Restricted Growth strings are often called
// restricted growth functions. Here are the RG strings corresponding
// to the partitions shown above.
//
//   1 block:  0000
//   2 blocks: 0001, 0010, 0100, 0111, 0011, 0101, 0110
//   3 blocks: 0122, 0121, 0112, 0120, 0102,
//
// ...more
//
// Reference:
// Frank Ruskey. Simple combinatorial Gray codes constructed by
// reversing sublists. Lecture Notes in Computer Science, #762,
// 201-208. Also downloadable from
// http://webhome.cs.uvic.ca/~ruskey/Publications/SimpleGray/SimpleGray.html
//
#include <boost/coroutine2/all.hpp>
#include <cassert>
#include <cctype>
#include <cstdio>
#include <functional>
#include <utility>

class set_partition_
{
    using ret_t = std::tuple<int, int>;
    using Fun = std::function<void(ret_t)>;

  private:
    int n;
    int k;
    // int b[100 + 1]; /* maximum value of n */
    // int cnt{0};
    Fun yield;

  public:
    /**
     * @brief Construct object
     * 
     * @param n_ 
     * @param k_ 
     * @param yield_ 
     */
    set_partition_(int n_, int k_, Fun yield_)
        : n(n_)
        , k(k_)
        , yield(yield_)
    {
    }

    /**
     * @brief run
     * 
     */
    void run()
    {
        if (k % 2 == 0)
            GEN0_even(n, k);
        else
            GEN0_odd(n, k);
    }

  private:
    /**
     * @brief Move
     * 
     * @param x 
     * @param y 
     */
    void Move(int x, int y)
    {
        yield(std::tuple {x, y});
    }

    /**
     * @brief S(n,k,0) even k
     * 
     * @param n 
     * @param k 
     */
    void GEN0_even(int n, int k);

    /**
     * @brief S'(n,k,0) even k
     * 
     * @param n 
     * @param k 
     */
    void NEG0_even(int n, int k);

    /**
     * @brief S(n,k,1) even k
     * 
     * @param n 
     * @param k 
     */
    void GEN1_even(int n, int k);

    /**
     * @brief S'(n,k,1) even k
     * 
     * @param n 
     * @param k 
     */
    void NEG1_even(int n, int k);

    /**
     * @brief S(n,k,0) odd k
     * 
     * @param n 
     * @param k 
     */
    void GEN0_odd(int n, int k);

    /**
     * @brief S'(n,k,0) odd k
     * 
     * @param n 
     * @param k 
     */
    void NEG0_odd(int n, int k);

    /**
     * @brief S(n,k,1) odd k
     * 
     * @param n 
     * @param k 
     */
    void GEN1_odd(int n, int k);

    /**
     * @brief S'(n,k,1) odd k
     * 
     * @param n 
     * @param k 
     */
    void NEG1_odd(int n, int k);
};

using ret_t = std::tuple<int, int>;
using coro_t = boost::coroutines2::coroutine<ret_t>;

extern coro_t::pull_type set_partition(int n, int k);
