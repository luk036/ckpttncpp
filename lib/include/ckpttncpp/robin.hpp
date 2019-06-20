#ifndef CKPTTNCPP_ROBIN_HPP
#define CKPTTNCPP_ROBIN_HPP 1

#include "dllist.hpp" // import dllink
#include <vector>

template <typename T> class robin {
  private:
    std::vector<dllink<T>> cycle;

    struct iterator {
        dllink<T> *cur;
        bool operator!=(const iterator &other) const {
            return cur != other.cur;
        }
        bool operator==(const iterator &other) const {
            return cur == other.cur;
        }
        iterator &operator++() {
            cur = cur->next;
            return *this;
        }
        const T &operator*() const { return cur->key; }
    };

    struct iterable_wrapper {
        robin<T> *rr;
        T fromPart;
        auto begin() { return iterator{rr->cycle[fromPart].next}; }
        auto end() { return iterator{&rr->cycle[fromPart]}; }
        // auto size() const -> size_t { return rr->cycle.size() - 1; }
    };

  public:
    explicit robin(T K1) : cycle(K1 + 1) {
        for (auto k = 0U; k < K1; ++k) {
            this->cycle[k].next = &this->cycle[k + 1];
            this->cycle[k].key = k;
        }
        this->cycle[K1].next = &this->cycle[0];
        this->cycle[K1].key = K1;
    }

    auto exclude(T fromPart) { return iterable_wrapper{this, fromPart}; }
};

#endif