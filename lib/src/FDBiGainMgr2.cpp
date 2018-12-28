#include <ckpttncpp/FDBiGainCalc.hpp>
#include <ckpttncpp/FDBiGainMgr.hpp>

/**
 * @brief
 *
 * @param part
 */
auto FDBiGainMgr::init(const PartInfo &part_info) -> int
{
    auto totalcost = Base::init(part_info);
    auto const& [part, extern_nets] = part_info;

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
