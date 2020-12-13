#include <ckpttncpp/bpqueue.hpp>

/**
 * @brief sentinel
 *
 * @tparam T
 */
template <typename T, typename Int>
dllink<std::pair<T, Int>> bpqueue<T, Int>::sentinel {};

template class bpqueue<int, int16_t>;