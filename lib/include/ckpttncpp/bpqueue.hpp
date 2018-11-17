#ifndef _HOME_UBUNTU_GITHUB_CKPTTNCPP_BPQUEUE_HPP
#define _HOME_UBUNTU_GITHUB_CKPTTNCPP_BPQUEUE_HPP 1

#include "dllist.hpp" // import dllink
#include <cassert>
#include <vector>

// forward declare
struct bpq_iterator;

/**
 * @brief bounded priority queue
 * 
 */
struct bpqueue
{
    int _offset;
    int _high;
    int _max;
    dllink _sentinel;
    std::vector<dllink> _bucket;

    /**
     * @brief Construct a new bpqueue object
     * 
     * @param a 
     * @param b 
     */
    bpqueue(int a, int b)
        : _offset{a - 1},
          _high{b - _offset},
          _max{0},
          _sentinel{8963},
          _bucket(_high + 1)
    {
        _bucket[0].append(_sentinel); // sentinel
    }

    /**
     * @brief Get the key value
     * 
     * @param it 
     * @return int
     */
    auto get_key(dllink &it) const -> int
    {
        return it._key + this->_offset;
    }

    /**
     * @brief Get the max value
     * 
     * @return int -- maximum value
     */
    auto get_max() const -> int
    {
        return this->_max + this->_offset;
    }

    /**
     * @brief whether empty 
     * 
     * @return true 
     * @return false 
     */
    auto is_empty() const -> bool
    {
        return this->_max == 0;
    }

    /**
     * @brief clear
     */
    auto clear() -> void
    {
        while (this->_max > 0)
        {
            this->_bucket[this->_max].clear();
            this->_max -= 1;
        }
    }

    /**
     * @brief 
     * 
     * @param it 
     * @param k 
     * @return auto 
     */
    auto append(dllink &it, int k)
    {
        auto key = k - this->_offset;
        if (this->_max < key)
        {
            this->_max = key;
        }
        it._key = key;
        this->_bucket[key].append(it);
    }

    /**
     * @brief append from list
     * 
     * @param nodes 
     * @param keys 
     * @return auto 
     */
    auto appendfrom(std::vector<dllink> &nodes)
    {
        for (auto &it : nodes)
        {
            it._key -= this->_offset;
            this->_bucket[it._key].append(it);
        }
        this->_max = this->_high;
        while (this->_bucket[this->_max].is_empty())
        {
            this->_max -= 1;
        }
    }

    /**
     * @brief pop node with maximum key
     * 
     * @return dllink& 
     */
    auto popleft() -> dllink &
    {
        dllink &res = this->_bucket[this->_max].popleft();
        while (this->_bucket[this->_max].is_empty())
        {
            this->_max -= 1;
        }
        return res;
    }

    /**
     * @brief decrease key
     * 
     * @param it 
     * @param delta 
     */
    auto decrease_key(dllink &it, int delta) -> void
    {
        // this->_bucket[it._key].detach(it)
        it.detach();
        it._key += delta;
        assert(it._key > 0);
        assert(it._key <= this->_high);
        this->_bucket[it._key].append(it); // FIFO
        while (this->_bucket[this->_max].is_empty())
        {
            this->_max -= 1;
        }
    }

    /**
     * @brief increase key
     * 
     * @param it 
     * @param delta 
     */
    auto increase_key(dllink &it, int delta) -> void
    {
        // this->_bucket[it._key].detach(it)
        it.detach();
        it._key += delta;
        assert(it._key > 0);
        assert(it._key <= this->_high);
        this->_bucket[it._key].appendleft(it); // LIFO
        if (this->_max < it._key)
        {
            this->_max = it._key;
        }
    }

    /**
     * @brief modify key
     * 
     * @param it 
     * @param delta 
     * @return auto 
     */
    auto modify_key(dllink &it, int delta)
    {
        if (delta > 0)
        {
            this->increase_key(it, delta);
        }
        else if (delta < 0)
        {
            this->decrease_key(it, delta);
        }
    }

    /**
     * @brief detach a node from bpqueue
     * 
     * @param it 
     * @return auto 
     */
    auto detach(dllink &it)
    {
        // this->_bucket[it._key].detach(it)
        it.detach();
        while (this->_bucket[this->_max].is_empty())
        {
            this->_max -= 1;
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
struct bpq_iterator
{
    bpqueue &_bpq;
    int _curkey;
    dll_iterator _curitem;

    /**
     * @brief Construct a new bpq iterator object
     * 
     * @param bpq 
     * @param curkey 
     */
    bpq_iterator(bpqueue &bpq, int curkey)
        : _bpq{bpq},
          _curkey{curkey},
          _curitem{_bpq._bucket[curkey].begin()}
    {
    }

    auto curlist() -> dllink &
    {
        return this->_bpq._bucket[this->_curkey];
    }

    /**
     * @brief 
     * 
     * @return bpq_iterator& 
     */
    auto operator++() -> bpq_iterator &
    {
        ++this->_curitem;
        if (this->_curitem == this->curlist().end())
        {
            do
            {
                this->_curkey -= 1;
            } while (this->curlist().is_empty());
            this->_curitem = this->curlist().begin();
        }
        return *this;
    }

    /**
     * @brief 
     * 
     * @return bpq_iterator& 
     */
    auto operator*() -> dllink &
    {
        return *this->_curitem;
    }

    /**
     * @brief eq operator
     * 
     * @param rhs 
     * @return true 
     * @return false 
     */
    auto operator==(const bpq_iterator &rhs) -> bool
    {
        return this->_curitem == rhs._curitem;
    }

    /**
     * @brief neq operator
     * 
     * @param rhs 
     * @return true 
     * @return false 
     */
    auto operator!=(const bpq_iterator &rhs) -> bool
    {
        return !(*this == rhs);
    }
};

inline auto bpqueue::begin() -> bpq_iterator
{
    return bpq_iterator(*this, this->_max);
}

inline auto bpqueue::end() -> bpq_iterator
{
    return bpq_iterator(*this, 0);
}

#endif
