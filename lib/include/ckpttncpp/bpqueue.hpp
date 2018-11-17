#ifndef _HOME_UBUNTU_GITHUB_CKPTTNCPP_BPQUEUE_HPP
#define _HOME_UBUNTU_GITHUB_CKPTTNCPP_BPQUEUE_HPP 1

#include "dllist.hpp" // import dllist, dllink
#include <vector>

// forward declare
struct bpq_iterator;

/**
 * @brief bounded priority queue
 * 
 */
struct bpqueue {
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
    bpqueue(int a, int b) :
        _offset{a - 1},
        _high{b - _offset},
        _max{0},
        _sentinel{8963},
        _bucket(_high + 1) 
    {
        _bucket[0].append(_sentinel);  // sentinel
    }

    /**
     * @brief Get the key object
     * 
     * @param it 
     * @return auto 
     */
    auto get_key(dllink& it) const {
        return it._key + this->_offset;
    }

    /**
     * @brief Get the max object
     * 
     * @return auto 
     */
    auto get_max() const {
        return this->_max + this->_offset;
    }

    /**
     * @brief 
     * 
     * @return auto 
     */
    auto is_empty() const {
        return this->_max == 0;
    }

    /**
     * @brief 
     * 
     * @return auto 
     */
    auto clear() {
        while (this->_max > 0) {
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
    auto append(dllink& it, int k) {
        auto key = k - this->_offset;
        if (this->_max < key) {
            this->_max = key;
        }
        it._key = key;
        this->_bucket[key].append(it);
    }

    /**
     * @brief 
     * 
     * @param nodes 
     * @param keys 
     * @return auto 
     */
    auto appendfrom(std::vector<dllink>& nodes,
                    const std::vector<int>& keys) {
        for (auto i = 0u; i < nodes.size(); ++i) {
            auto& it = nodes[i];
            auto key = keys[i] - this->_offset;
            it._key = key;
            this->_bucket[key].append(it);
        }
        this->_max = this->_high;
        while (this->_bucket[this->_max].is_empty()) {
            this->_max -= 1;
        }
    }

    /**
     * @brief 
     * 
     * @return dllink& 
     */
    auto popleft() -> dllink& {
        dllink& res = this->_bucket[this->_max].popleft();
        while (this->_bucket[this->_max].is_empty()) {
            this->_max -= 1;
        }
        return res;
    }

    /**
     * @brief 
     * 
     * @param it 
     * @param delta 
     * @return auto 
     */
    auto decrease_key(dllink& it, int delta) {
        // this->_bucket[it._key].detach(it)
        it.detach();
        it._key -= delta;
        this->_bucket[it._key].append(it);  // FIFO
        while (this->_bucket[this->_max].is_empty()) {
            this->_max -= 1;
        }
    }

    /**
     * @brief 
     * 
     * @param it 
     * @param delta 
     * @return auto 
     */
    auto increase_key(dllink& it, int delta) {
        // this->_bucket[it._key].detach(it)
        it.detach();
        it._key += delta;
        this->_bucket[it._key].appendleft(it);  // LIFO
        if (this->_max < it._key) {
            this->_max = it._key;
        }
    }

    /**
     * @brief 
     * 
     * @param it 
     * @param delta 
     * @return auto 
     */
    auto modify_key(dllink& it, int delta) {
        if (delta > 0) {
            this->increase_key(it, delta);
        } else if (delta < 0) {
            this->decrease_key(it, delta);
        }
    }

    /**
     * @brief 
     * 
     * @param it 
     * @return auto 
     */
    auto detach(dllink& it) {
        // this->_bucket[it._key].detach(it)
        it.detach();
        while (this->_bucket[this->_max].is_empty()) {
            this->_max -= 1;
        }
    }

    /**
     * @brief 
     * 
     * @return bpq_iterator 
     */
    auto begin() -> bpq_iterator;

    /**
     * @brief 
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
    bpqueue& _bpq;
    int _curkey;
    dll_iterator _curitem;

    /**
     * @brief Construct a new bpq iterator object
     * 
     * @param bpq 
     * @param curkey 
     */
    bpq_iterator(bpqueue &bpq, int curkey) :
        _bpq{bpq},
        _curkey{curkey},
        _curitem{_bpq._bucket[curkey].begin()} {
    }

    auto curlist() -> dllink& {
        return this->_bpq._bucket[this->_curkey];
    }

    /**
     * @brief 
     * 
     * @return bpq_iterator& 
     */
    auto operator++() -> bpq_iterator& {
        ++this->_curitem;
        if (this->_curitem == this->curlist().end()) {
            do {
                this->_curkey -= 1;
            }
            while (this->curlist().is_empty());
            this->_curitem = this->curlist().begin();
        } 
        return *this;
    }

    /**
     * @brief 
     * 
     * @return bpq_iterator& 
     */
    auto operator*() -> dllink& {
        return *this->_curitem;
    }

    /**
     * @brief 
     * 
     * @param rhs 
     * @return true 
     * @return false 
     */
    auto operator==(const bpq_iterator& rhs) -> bool {
        return this->_curitem == rhs._curitem;
    }

    /**
     * @brief 
     * 
     * @param rhs 
     * @return true 
     * @return false 
     */
    auto operator!=(const bpq_iterator& rhs) -> bool {
        return !(*this == rhs);
    }

    // auto __iter__() {
    //     this->_curkey = this->_max;
    //     this->_curitem = iter(this->_bucket[this->_curkey]);
    //     return self;
    // }

    // auto __next__() {
    //     while (this->_curkey > 0) {
    //         try {
    //             res = next(this->_curitem);
    //             return res;
    //         }
    //         catch StopIteration {
    //             this->_curkey -= 1;
    //             this->_curitem = iter(this->_bucket[this->_curkey]);
    //         }
    //     }
    //     raise StopIteration;
    // }
};


inline auto bpqueue::begin() -> bpq_iterator {
    return bpq_iterator(*this, this->_max);
}

inline auto bpqueue::end() -> bpq_iterator {
    return bpq_iterator(*this, 0);
}

#endif
