#include <ckpttncpp/bpqueue.hpp>

/**
 * @brief sentinel
 *
 * @tparam T
 */
template <typename T, typename Int, class Container>
dllink<std::pair<T, Int>> bpqueue<T, Int, Container>::sentinel {};

template class bpqueue<int, int16_t>;
// template class bpqueue<int, int16_t, 
//                std::pmr::vector<dllink<std::pair<int, int16_t>> > >;
