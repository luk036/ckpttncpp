#ifndef CKPTTNCPP_BPQUEUE_HPP
#define CKPTTNCPP_BPQUEUE_HPP 1

#include "dllist.hpp" // import dllink
#include <cassert>
#include <vector>

// forward declare
struct bpq_iterator;

/**
 * @brief bounded priority queue
 *
 */
struct bpqueue {
    int offset;
    int high;
    int max;
    dllink sentinel;
    std::vector<dllink> bucket;

    /**
     * @brief Construct a new bpqueue object
     *
     * @param a
     * @param b
     */
    bpqueue(int a, int b)
        : offset{a - 1}, high{b - offset}, max{0}, sentinel{},
          bucket(high + 1) {
        bucket[0].append(sentinel); // sentinel
    }

    /**
     * @brief Set the key object
     *
     * @param it
     * @param gain
     */
    auto set_key(dllink &it, int gain) -> void { it.key = gain - this->offset; }

    /**
     * @brief Get the max value
     *
     * @return int -- maximum value
     */
    [[nodiscard]] auto get_max() const -> int { return this->max + this->offset; }

    /**
     * @brief whether empty
     *
     * @return true
     * @return false
     */
    [[nodiscard]] auto is_empty() const -> bool { return this->max == 0; }

    /**
     * @brief clear
     */
    auto clear() -> void {
        while (this->max > 0) {
            this->bucket[this->max].clear();
            this->max -= 1;
        }
    }

    /**
     * @brief
     *
     * @param it
     * @param k
     */
    auto append_direct(dllink &it) -> void {
        assert(it.key > this->offset);
        this->append(it, it.key);
    }

    /**
     * @brief
     *
     * @param it
     * @param k
     */
    auto append(dllink &it, int k) -> void {
        assert(k > this->offset);
        it.key = k - this->offset;
        if (this->max < it.key) {
            this->max = it.key;
        }
        this->bucket[it.key].append(it);
    }

    /**
     * @brief append from list
     *
     * @param nodes
     * @param keys
     */
    auto appendfrom(std::vector<dllink> &nodes) -> void {
        for (auto &it : nodes) {
            it.key -= this->offset;
            assert(it.key > 0);
            this->bucket[it.key].append(it);
        }
        this->max = this->high;
        while (this->bucket[this->max].is_empty()) {
            this->max -= 1;
        }
    }

    /**
     * @brief pop node with maximum key
     *
     * @return dllink&
     */
    auto popleft() -> dllink & {
        dllink &res = this->bucket[this->max].popleft();
        while (this->bucket[this->max].is_empty()) {
            this->max -= 1;
        }
        return res;
    }

    /**
     * @brief decrease key
     *
     * @param it
     * @param delta
     */
    auto decrease_key(dllink &it, int delta) -> void {
        // this->bucket[it.key].detach(it)
        it.detach();
        it.key += delta;
        assert(it.key > 0);
        assert(it.key <= this->high);
        this->bucket[it.key].append(it); // FIFO
        if (this->max < it.key) {
            this->max = it.key;
            return;
        }
        while (this->bucket[this->max].is_empty()) {
            this->max -= 1;
        }
    }

    /**
     * @brief increase key
     *
     * @param it
     * @param delta
     */
    auto increase_key(dllink &it, int delta) -> void {
        // this->bucket[it.key].detach(it)
        it.detach();
        it.key += delta;
        assert(it.key > 0);
        assert(it.key <= this->high);
        this->bucket[it.key].appendleft(it); // LIFO
        if (this->max < it.key) {
            this->max = it.key;
        }
    }

    /**
     * @brief modify key
     *
     * @param it
     * @param delta
     */
    auto modify_key(dllink &it, int delta) -> void {
        if (it.is_locked()) {
            return;
        }
        if (delta > 0) {
            this->increase_key(it, delta);
        } else if (delta < 0) {
            this->decrease_key(it, delta);
        }
    }

    /**
     * @brief detach a node from bpqueue
     *
     * @param it
     * @return auto
     */
    auto detach(dllink &it) -> void {
        // this->bucket[it.key].detach(it)
        it.detach();
        while (this->bucket[this->max].is_empty()) {
            this->max -= 1;
        }
    }

    /**
     * @brief iterator point to begin
     *
     * @return bpq_iterator
     */
    auto begin() -> bpq_iterator;

    /**
     * @brief iterator point to end
     *
     * @return bpq_iterator
     */
    auto end() -> bpq_iterator;
};

/**
 * @brief doubly linked node
 *
 */
struct bpq_iterator {
    bpqueue &bpq;
    int curkey;
    dll_iterator curitem;

    /**
     * @brief Construct a new bpq iterator object
     *
     * @param bpq
     * @param curkey
     */
    bpq_iterator(bpqueue &bpq, int curkey)
        : bpq{bpq}, curkey{curkey}, curitem{bpq.bucket[curkey].begin()} {}

    /**
     * @brief
     *
     * @return dllink&
     */
    auto curlist() -> dllink & { return this->bpq.bucket[this->curkey]; }

    /**
     * @brief
     *
     * @return bpq_iterator&
     */
    auto operator++() -> bpq_iterator & {
        ++this->curitem;
        if (this->curitem == this->curlist().end()) {
            do {
                this->curkey -= 1;
            } while (this->curlist().is_empty());
            this->curitem = this->curlist().begin();
        }
        return *this;
    }

    /**
     * @brief
     *
     * @return bpq_iterator&
     */
    auto operator*() -> dllink & { return *this->curitem; }

    /**
     * @brief eq operator
     *
     * @param rhs
     * @return true
     * @return false
     */
    auto operator==(const bpq_iterator &rhs) -> bool {
        return this->curitem == rhs.curitem;
    }

    /**
     * @brief neq operator
     *
     * @param rhs
     * @return true
     * @return false
     */
    auto operator!=(const bpq_iterator &rhs) -> bool { return !(*this == rhs); }
};

/**
 * @brief
 *
 * @return bpq_iterator
 */
inline auto bpqueue::begin() -> bpq_iterator {
    return bpq_iterator(*this, this->max);
}

/**
 * @brief
 *
 * @return bpq_iterator
 */
inline auto bpqueue::end() -> bpq_iterator { return bpq_iterator(*this, 0); }

#endif
