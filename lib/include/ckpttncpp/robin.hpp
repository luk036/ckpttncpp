#ifndef HOME_UBUNTU_GITHUB_CKPTTNCPP_ROBIN_HPP
#define HOME_UBUNTU_GITHUB_CKPTTNCPP_ROBIN_HPP 1

#include "dllist.hpp" // import dllink
#include <vector>

class robin;

class robin {
  private:
    std::vector<dllink> cycle;

    struct iterator {
        dllink *cur;
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
        int operator*() const { return cur->key; }
    };
    struct iterable_wrapper {
        robin *rr;
        size_t fromPart;
        auto begin() { return iterator{rr->cycle[fromPart].next}; }
        auto end() { return iterator{&rr->cycle[fromPart]}; }
        auto size() const -> size_t { return rr->cycle.size() - 1; }
    };

  public:
    explicit robin(size_t K) : cycle(K) {
        for (auto k = 0u; k < K - 1; ++k) {
            this->cycle[k].next = &this->cycle[k + 1];
            this->cycle[k].key = k;
        }
        this->cycle[K - 1].next = &this->cycle[0];
        this->cycle[K - 1].key = K - 1;
    }

    auto exclude(size_t fromPart) { return iterable_wrapper{this, fromPart}; }
};

#endif