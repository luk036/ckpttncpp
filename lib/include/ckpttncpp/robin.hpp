#ifndef CKPTTNCPP_ROBIN_HPP
#define CKPTTNCPP_ROBIN_HPP 1

#include <vector>
#include "dllist.hpp" // import dllink

template<typename T>
class robin
{
private:
    std::vector<dllink<T>> cycle;

    struct iterator
    {
        dllink<T>* cur;
        bool       operator!=(const iterator& other) const { return cur != other.cur; }
        bool       operator==(const iterator& other) const { return cur == other.cur; }
        iterator&  operator++()
        {
            cur = cur->next;
            return *this;
        }
        const T& operator*() const { return cur->key; }
    };

    struct iterable_wrapper
    {
        robin<T>* rr;
        T         fromPart;
        auto      begin() { return iterator{rr->cycle[fromPart].next}; }
        auto      end() { return iterator{&rr->cycle[fromPart]}; }
        // auto size() const -> size_t { return rr->cycle.size() - 1; }
    };

public:
    explicit robin(T K) : cycle(K)
    {
        K -= 1;
        for (auto k = 0U; k < K; ++k)
        {
            this->cycle[k].next = &this->cycle[k + 1];
            this->cycle[k].key  = k;
        }
        this->cycle[K].next = &this->cycle[0];
        this->cycle[K].key  = K;
    }

    auto exclude(T fromPart) { return iterable_wrapper{this, fromPart}; }
};

#endif