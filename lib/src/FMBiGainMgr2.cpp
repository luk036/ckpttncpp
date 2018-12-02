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
auto FMBiGainMgr::init(const std::vector<std::uint8_t> &part) -> void {
    Base::init(part);

    for (auto v : this->H.module_list) {
        auto &vlink = this->gainCalc.vertex_list[v];
        auto toPart = 1 - part[v];
        this->gainbucket[toPart]->append(vlink, vlink.key);
    }
}
