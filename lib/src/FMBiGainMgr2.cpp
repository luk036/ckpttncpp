#include <ckpttncpp/FMBiGainCalc.hpp>
#include <ckpttncpp/FMBiGainMgr.hpp>

/* linux-2.6.38.8/include/linux/compiler.h */
#include <stdio.h>
#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

/**
 * @brief
 *
 * @param part
 */
auto FMBiGainMgr::init(const std::vector<std::uint8_t> &part) -> void
{
    Base::init(part);
    for (auto k = 0u; k < this->K; ++k) {
        this->gainbucket[k]->clear();
    }
    for (auto v = 0u; v < this->H.number_of_modules(); ++v) {
        auto &vlink = this->gainCalc.vertex_list[v];
        auto toPart = 1 - part[v];
        this->gainbucket[toPart]->append_direct(vlink);
    }
}
