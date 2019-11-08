#include <ckpttncpp/FMBiGainCalc.hpp>
#include <ckpttncpp/FMBiGainMgr.hpp>

/**
 * @brief
 *
 * @param part
 */
int FMBiGainMgr::init(gsl::span<const uint8_t> part)
{
    auto totalcost = Base::init(part);
    for (auto k = 0U; k < this->K; ++k)
    {
        this->gainbucket[k].clear();
    }
    for (auto v : this->H.modules)
    {
        auto& vlink = this->gainCalc.vertex_list[v];
        auto toPart = 1 - part[v];
        this->gainbucket[toPart].append_direct(vlink);
    }
    for (auto v : this->H.module_fixed)
    {
        this->lock_all(part[v], v);
    }
    return totalcost;
}
