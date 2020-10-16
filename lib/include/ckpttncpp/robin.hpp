#pragma once

#include "dllist.hpp" // import dllink
#include <vector>

template <typename T>
class robin
{
  private:
    std::vector<dllink<T>> cycle;

    struct iterator
    {
        dllink<T>* cur;
        auto operator!=(const iterator& other) const -> bool
        {
            return cur != other.cur;
        }
        auto operator==(const iterator& other) const -> bool
        {
            return cur == other.cur;
        }
        auto operator++() -> iterator&
        {
            cur = cur->next;
            return *this;
        }
        auto operator*() const -> const T&
        {
            return cur->key;
        }
    };

    struct iterable_wrapper
    {
        robin<T>* rr;
        T fromPart;
        auto begin()
        {
            return iterator {rr->cycle[fromPart].next};
        }
        auto end()
        {
            return iterator {&rr->cycle[fromPart]};
        }
        // auto size() const -> size_t { return rr->cycle.size() - 1; }
    };

  public:
    explicit robin(T K)
        : cycle(K)
    {
        K -= 1;
        for (auto k = 0U; k != K; ++k)
        {
            this->cycle[k].next = &this->cycle[k + 1];
            this->cycle[k].key = k;
        }
        this->cycle[K].next = &this->cycle[0];
        this->cycle[K].key = K;
    }

    auto exclude(T fromPart)
    {
        return iterable_wrapper {this, fromPart};
    }
};
