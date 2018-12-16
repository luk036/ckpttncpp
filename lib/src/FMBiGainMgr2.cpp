#include <ckpttncpp/FMBiGainCalc.hpp>
#include <ckpttncpp/FMBiGainMgr2.hpp>

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
    for (auto i_v = 0u; i_v < this->H.number_of_modules(); ++i_v) {
        auto &vlink = this->gainCalc.vertex_list[i_v];
        auto toPart = 1 - part[i_v];
        this->gainbucket[toPart]->append(vlink, vlink.key);
    }
}
