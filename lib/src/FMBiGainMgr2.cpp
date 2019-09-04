#include <ckpttncpp/FMBiGainCalc.hpp>
#include <ckpttncpp/FMBiGainMgr.hpp>

/**
 * @brief
 *
 * @param part
 */
int FMBiGainMgr::init(const std::vector<uint8_t>& part)
{
    auto totalcost = Base::init(part);
    for (auto k = 0U; k < this->K; ++k)
    {
        this->gainbucket[k].clear();
    }
    for (auto i_v = 0U; i_v < this->H.number_of_modules(); ++i_v)
    {
        auto& vlink = this->gainCalc.vertex_list[i_v];
        auto toPart = 1 - part[i_v];
        this->gainbucket[toPart].append_direct(vlink);
    }
    for (auto v : this->H.module_fixed)
    {
        auto i_v = this->H.module_map[v];
        this->lock_all(part[i_v], i_v);
    }
    return totalcost;
}
