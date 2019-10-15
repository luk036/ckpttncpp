#include <ckpttncpp/set_partition.hpp>

// The lists S(n,k,0) and S(n,k,1) satisfy the following properties.
// 1. Successive RG sequences differ in exactly one position.
// 2. first(S(n,k,0)) = first(S(n,k,1)) = 0^{n-k}0123...(k-1)
// 3. last(S(n,k,0)) = 0^{n-k}12...(k-1)0
// 4. last(S(n,k,1)) = 012...(k-1)0^{n-k}
// Note that first(S'(n,k,p)) = last(S(n,k,p))

/**
 * @brief S(n,k,0) even k
 *
 * @param n
 * @param k
 */
void set_partition_::__GEN0_even(int n, int k)
{
    if (!(k > 0 && k < n))
    {
        return;
    }

    this->__GEN0_odd(n - 1, k - 1); // S(n-1, k-1, 0).(k-1)
    this->__Move(n - 1, k - 1);
    this->__GEN1_even(n - 1, k); // S(n-1, k, 1).(k-1)
    this->__Move(n, k - 2);
    this->__NEG1_even(n - 1, k); // S'(n-1, k, 1).(k-2)

    for (int i = k - 3; i >= 1; i -= 2)
    {
        this->__Move(n, i);
        this->__GEN1_even(n - 1, k); // S(n-1, k, 1).i
        this->__Move(n, i - 1);
        this->__NEG1_even(n - 1, k); // S'(n-1, k, 1).(i-1)
    }
}

/**
 * @brief S'(n,k,0) even k
 *
 * @param n
 * @param k
 */
void set_partition_::__NEG0_even(int n, int k)
{
    if (!(k > 0 && k < n))
    {
        return;
    }

    for (int i = 1; i <= k - 3; i += 2)
    {
        this->__GEN1_even(n - 1, k); // S(n-1, k, 1).(i-1)
        this->__Move(n, i);
        this->__NEG1_even(n - 1, k); // S'(n-1, k, 1).i
        this->__Move(n, i + 1);
    }

    this->__GEN1_even(n - 1, k); // S(n-1, k, 1).(k-2)
    this->__Move(n, k - 1);
    this->__NEG1_even(n - 1, k); // S(n-1, k, 1).(k-1)
    this->__Move(n - 1, 0);
    this->__NEG0_odd(n - 1, k - 1); // S(n-1, k-1, 1).(k-1)
}

/**
 * @brief S(n,k,1) even k
 *
 * @param n
 * @param k
 */
void set_partition_::__GEN1_even(int n, int k)
{
    if (!(k > 0 && k < n))
    {
        return;
    }

    this->__GEN1_odd(n - 1, k - 1);
    this->__Move(k, k - 1);
    this->__NEG1_even(n - 1, k);
    this->__Move(n, k - 2);
    this->__GEN1_even(n - 1, k);

    for (int i = k - 3; i > 0; i -= 2)
    {
        this->__Move(n, i);
        this->__NEG1_even(n - 1, k);
        this->__Move(n, i - 1);
        this->__GEN1_even(n - 1, k);
    }
}

/**
 * @brief S'(n,k,1) even k
 *
 * @param n
 * @param k
 */
void set_partition_::__NEG1_even(int n, int k)
{
    if (!(k > 0 && k < n))
    {
        return;
    }

    for (int i = 1; i <= k - 3; i += 2)
    {
        this->__NEG1_even(n - 1, k);
        this->__Move(n, i);
        this->__GEN1_even(n - 1, k);
        this->__Move(n, i + 1);
    }

    this->__NEG1_even(n - 1, k);
    this->__Move(n, k - 1);
    this->__GEN1_even(n - 1, k);
    this->__Move(k, 0);
    this->__NEG1_odd(n - 1, k - 1);
}

/**
 * @brief S(n,k,0) odd k
 *
 * @param n
 * @param k
 */
void set_partition_::__GEN0_odd(int n, int k)
{
    if (!(k > 1 && k < n))
    {
        return;
    }

    this->__GEN1_even(n - 1, k - 1);
    this->__Move(k, k - 1);
    this->__NEG1_odd(n - 1, k);

    for (int i = k - 2; i > 0; i -= 2)
    {
        this->__Move(n, i);
        this->__GEN1_odd(n - 1, k);
        this->__Move(n, i - 1);
        this->__NEG1_odd(n - 1, k);
    }
}

/**
 * @brief S'(n,k,0) odd k
 *
 * @param n
 * @param k
 */
void set_partition_::__NEG0_odd(int n, int k)
{
    if (!(k > 1 && k < n))
    {
        return;
    }

    for (int i = 1; i <= k - 2; i += 2)
    {
        this->__GEN1_odd(n - 1, k);
        this->__Move(n, i);
        this->__NEG1_odd(n - 1, k);
        this->__Move(n, i + 1);
    }

    this->__GEN1_odd(n - 1, k);
    this->__Move(k, 0);
    this->__NEG1_even(n - 1, k - 1);
}

/**
 * @brief S(n,k,1) odd k
 *
 * @param n
 * @param k
 */
void set_partition_::__GEN1_odd(int n, int k)
{
    if (!(k > 1 && k < n))
    {
        return;
    }

    this->__GEN0_even(n - 1, k - 1);
    this->__Move(n - 1, k - 1);
    this->__GEN1_odd(n - 1, k);

    for (int i = k - 2; i > 0; i -= 2)
    {
        this->__Move(n, i);
        this->__NEG1_odd(n - 1, k);
        this->__Move(n, i - 1);
        this->__GEN1_odd(n - 1, k);
    }
}

/**
 * @brief S'(n,k,1) odd k
 *
 * @param n
 * @param k
 */
void set_partition_::__NEG1_odd(int n, int k)
{
    if (!(k > 1 && k < n))
    {
        return;
    }

    for (int i = 1; i <= k - 2; i += 2)
    {
        this->__NEG1_odd(n - 1, k);
        this->__Move(n, i);
        this->__GEN1_odd(n - 1, k);
        this->__Move(n, i + 1);
    }

    this->__NEG1_odd(n - 1, k);
    this->__Move(n - 1, 0);
    this->__NEG0_even(n - 1, k - 1);
}

/**
 * @brief Set Partition
 *
 * @param n
 * @param k
 * @return coro_t::pull_type
 */
coro_t::pull_type set_partition(int n, int k)
{
    return coro_t::pull_type([n, k](coro_t::push_type& callback) {
        set_partition_ p([&callback](ret_t ch) { callback(ch); });
        p.run(n, k);
    });
}
