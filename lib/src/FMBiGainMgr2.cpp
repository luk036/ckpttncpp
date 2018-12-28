#include <ckpttncpp/FMBiGainCalc.hpp>
#include <ckpttncpp/FMBiGainMgr.hpp>

/**
 * @brief
 *
 * @param part
 */
auto FMBiGainMgr::init(const std::vector<std::uint8_t> &part) -> int
{
    auto totalcost = Base::init(part);
    for (auto k = 0u; k < this->K; ++k) {
        this->gainbucket[k]->clear();
    }
    for (auto v = 0u; v < this->H.number_of_modules(); ++v) {
        auto &vlink = this->gainCalc.vertex_list[v];
        auto toPart = 1 - part[v];
        this->gainbucket[toPart]->append_direct(vlink);
    }
    for (auto v : this->H.module_fixed) {
        this->lock_all(part[v], v);
    }
    return totalcost;
}
