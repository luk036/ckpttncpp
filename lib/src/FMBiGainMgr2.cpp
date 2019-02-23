#include <ckpttncpp/FMBiGainCalc.hpp>
#include <ckpttncpp/FMBiGainMgr.hpp>

/**
 * @brief
 *
 * @param part
 */
auto FMBiGainMgr::init(const PartInfo &part_info) -> int {
    auto totalcost = Base::init(part_info);
    for (auto k = 0U; k < this->K; ++k) {
        this->gainbucket[k]->clear();
    }
    auto const &[part, extern_nets] = part_info;
    for (auto v : this->H.modules) {
        auto &vlink = this->gainCalc.vertex_list[v];
        auto toPart = 1 - part[v];
        this->gainbucket[toPart]->append_direct(vlink);
    }
    for (auto v : this->H.module_fixed) {
        this->lock_all(part[v], v);
    }
    return totalcost;
}
