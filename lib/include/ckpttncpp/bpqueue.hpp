#pragma once

#include "dllist.hpp" // import dllink
#include <cassert>
#include <gsl/span>
#include <tuple>
#include <type_traits>
#include <vector>

// Forward declaration for begin() end()
template <typename T, typename Int>
class bpq_iterator;

/*!
 * @brief bounded priority queue
 *
 * Bounded Priority Queue with integer keys in [a..b].
 * Implemented by array (bucket) of doubly-linked lists.
 * Efficient if key is bounded by a small integer value.
 *
 * Note that this class does not own the PQ nodes. This feature
 * makes the nodes sharable between doubly linked list class and
 * this class. In the FM algorithm, the node either attached to
 * the gain buckets (PQ) or in the waitinglist (doubly linked list),
 * but not in both of them in the same time.
 *
 * Another improvement is to make the array size one element bigger
 * i.e. (b - a + 2). The extra dummy array element (which is called
 * sentinel) is used to reduce the boundary checking during updating.
 *
 * All the member functions assume that the keys are within the bound.
 *
 * @TODO: support std::pmr
 */
template <typename T, typename Int = int16_t> //
class bpqueue
{
    friend bpq_iterator<T, Int>;
    using value_type = T;
    using key_type = Int;
    using Item = dllink<std::pair<T, Int>>;

  private:
    static Item sentinel; /*!< sentinel */

    std::vector<Item> bucket; //!< bucket, array of lists
    Int max {};               //!< max value
    Int offset;               //!< a - 1
    Int high;                 //!< b - a + 1

  public:
    /*!
     * @brief Construct a new bpqueue object
     *
     * @param[in] a lower bound
     * @param[in] b upper bound
     */
    bpqueue(Int a, Int b)
        : bucket(b - a + 2)
        , offset(a - 1)
        , high(b - offset)
    {
        assert(a <= b);
        static_assert(
            std::is_integral<Int>::value, "bucket's key must be an integer");
        bucket[0].append(this->sentinel); // sentinel
    }

    bpqueue(const bpqueue&) = delete;                    // don't copy
    auto operator=(const bpqueue&) -> bpqueue& = delete; // don't assign
    bpqueue(bpqueue&&) noexcept = default;
    auto operator=(bpqueue&&) noexcept -> bpqueue& = default; // don't assign
    ~bpqueue() = default;

    /*!
     * @brief Set the key object
     *
     * @param[out] it   the item
     * @param[in] gain the key of it
     */
    auto set_key(Item& it, Int gain) -> void
    {
        it.data.second = gain - this->offset;
    }

    /*!
     * @brief Get the max value
     *
     * @return T maximum value
     */
    auto get_max() const -> Int
    {
        return this->max + this->offset;
    }

    /*!
     * @brief whether empty
     *
     * @return true
     * @return false
     */
    [[nodiscard]] auto is_empty() const -> bool
    {
        return this->max == 0;
    }

    /*!
     * @brief clear reset the PQ
     */
    auto clear() -> void
    {
        while (this->max > 0)
        {
            this->bucket[this->max].clear();
            this->max -= 1;
        }
    }

    /*!
     * @brief append item with internal key
     *
     * @param[in,out] it the item
     */
    auto append_direct(Item& it) -> void
    {
        assert(it.data.second > this->offset);
        this->append(it, it.data.second);
    }

    /*!
     * @brief append item with external key
     *
     * @param[in,out] it the item
     * @param[in] k  the key
     */
    auto append(Item& it, Int k) -> void
    {
        assert(k > this->offset);
        it.data.second = k - this->offset;
        if (this->max < it.data.second)
        {
            this->max = it.data.second;
        }
        this->bucket[it.data.second].append(it);
    }

    /*!
     * @brief append from list
     *
     * @param[in,out] nodes
     */
    auto appendfrom(gsl::span<Item> nodes) -> void
    {
        for (auto&& it : nodes)
        {
            it.data.second -= this->offset;
            assert(it.data.second > 0);
            this->bucket[it.data.second].append(it);
        }
        this->max = this->high;
        while (this->bucket[this->max].is_empty())
        {
            this->max -= 1;
        }
    }

    /*!
     * @brief pop node with the highest key
     *
     * @return dllink&
     */
    auto popleft() -> Item&
    {
        auto& res = this->bucket[this->max].popleft();
        while (this->bucket[this->max].is_empty())
        {
            this->max -= 1;
        }
        return res;
    }

    /*!
     * @brief decrease key by delta
     *
     * @param[in,out] it the item
     * @param[in] delta the change of the key
     *
     * Note that the order of items with same key will not be preserved.
     * For FM algorithm, this is a prefered behavior.
     */
    auto decrease_key(Item& it, Int delta) -> void
    {
        // this->bucket[it.data.second].detach(it)
        it.detach();
        it.data.second += delta;
        assert(it.data.second > 0);
        assert(it.data.second <= this->high);
        this->bucket[it.data.second].append(it); // FIFO
        if (this->max < it.data.second)
        {
            this->max = it.data.second;
            return;
        }
        while (this->bucket[this->max].is_empty())
        {
            this->max -= 1;
        }
    }

    /*!
     * @brief increase key by delta
     *
     * @param[in,out] it the item
     * @param[in] delta the change of the key
     *
     * Note that the order of items with same key will not be preserved.
     * For FM algorithm, this is a prefered behavior.
     */
    auto increase_key(Item& it, Int delta) -> void
    {
        // this->bucket[it.data.second].detach(it)
        it.detach();
        it.data.second += delta;
        assert(it.data.second > 0);
        assert(it.data.second <= this->high);
        this->bucket[it.data.second].appendleft(it); // LIFO
        if (this->max < it.data.second)
        {
            this->max = it.data.second;
        }
    }

    /*!
     * @brief modify key by delta
     *
     * @param[in,out] it the item
     * @param[in] delta the change of the key
     *
     * Note that the order of items with same key will not be preserved.
     * For FM algorithm, this is a prefered behavior.
     */
    auto modify_key(Item& it, Int delta) -> void
    {
        if (it.is_locked())
        {
            return;
        }
        if (delta > 0)
        {
            this->increase_key(it, delta);
        }
        else if (delta < 0)
        {
            this->decrease_key(it, delta);
        }
    }

    /*!
     * @brief detach the item from bpqueue
     *
     * @param[in,out] it the item
     */
    auto detach(Item& it) -> void
    {
        // this->bucket[it.data.second].detach(it)
        it.detach();
        while (this->bucket[this->max].is_empty())
        {
            this->max -= 1;
        }
    }

    /*!
     * @brief iterator point to begin
     *
     * @return bpq_iterator
     */
    auto begin() -> bpq_iterator<T, Int>;

    /*!
     * @brief iterator point to end
     *
     * @return bpq_iterator
     */
    auto end() -> bpq_iterator<T, Int>;

    auto& items()
    {
        return *this;
    }

    const auto& items() const
    {
        return *this;
    }

    // using coro_t = boost::coroutines2::coroutine<dllink<T>&>;
    // using pull_t = typename coro_t::pull_type;

    // /**
    //  * @brief item generator
    //  *
    //  * @return pull_t
    //  */
    // auto items() -> pull_t
    // {
    //     auto func = [&](typename coro_t::push_type& yield) {
    //         auto curkey = this->max;
    //         while (curkey > 0)
    //         {
    //             for (auto&& item : this->bucket[curkey].items())
    //             {
    //                 yield(item);
    //             }
    //             curkey -= 1;
    //         }
    //     };
    //     return pull_t(func);
    // }
};

/*!
 * @brief Bounded Priority Queue Iterator
 *
 * Bounded Priority Queue Iterator. Traverse the queue in descending
 * order. Detaching queue items may invalidate the iterator because
 * the iterator makes a copy of current key.
 */
template <typename T, typename Int = int16_t>
class bpq_iterator
{
    using value_type = T;
    using key_type = Int;
    using Item = dllink<std::pair<T, Int>>;

  private:
    bpqueue<T, Int>& bpq; /*!< the priority queue */
    Int curkey;           /*!< the current key value */
    dll_iterator<std::pair<T, Int>>
        curitem; /*!< list iterator pointed to the current item.
                  */

    /*!
     * @brief get the reference of the current list
     *
     * @return dllink&
     */
    auto curlist() -> Item&
    {
        return this->bpq.bucket[this->curkey];
    }

  public:
    /*!
     * @brief Construct a new bpq iterator object
     *
     * @param[in] bpq
     * @param[in] curkey
     */
    bpq_iterator(bpqueue<T, Int>& bpq, Int curkey)
        : bpq {bpq}
        , curkey {curkey}
        , curitem {bpq.bucket[curkey].begin()}
    {
    }

    /*!
     * @brief move to the next item
     *
     * @return bpq_iterator&
     */
    auto operator++() -> bpq_iterator&
    {
        ++this->curitem;
        while (this->curitem == this->curlist().end())
        {
            do
            {
                this->curkey -= 1;
            } while (this->curlist().is_empty());
            this->curitem = this->curlist().begin();
        }
        return *this;
    }

    /*!
     * @brief get the reference of the current item
     *
     * @return bpq_iterator&
     */
    auto operator*() -> Item&
    {
        return *this->curitem;
    }

    /*!
     * @brief eq operator
     *
     * @param[in] rhs
     * @return true
     * @return false
     */
    friend auto operator==(const bpq_iterator& lhs, const bpq_iterator& rhs)
        -> bool
    {
        return lhs.curitem == rhs.curitem;
    }

    /*!
     * @brief neq operator
     *
     * @param[in] rhs
     * @return true
     * @return false
     */
    friend auto operator!=(const bpq_iterator& lhs, const bpq_iterator& rhs)
        -> bool
    {
        return !(lhs == rhs);
    }
};

/*!
 * @brief
 *
 * @return bpq_iterator
 */
template <typename T, typename Int>
inline auto bpqueue<T, Int>::begin() -> bpq_iterator<T, Int>
{
    return {*this, this->max};
}

/*!
 * @brief
 *
 * @return bpq_iterator
 */
template <typename T, typename Int>
inline auto bpqueue<T, Int>::end() -> bpq_iterator<T, Int>
{
    return {*this, 0};
}
