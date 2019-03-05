#include <ckpttncpp/FDBiGainCalc.hpp>
#include <ckpttncpp/FDBiGainMgr.hpp>

/**
 * @brief
 *
 * @param part_info
 */
auto FDBiGainMgr::init(const PartInfo &part_info) -> int {
    auto totalcost = Base::init(part_info);
    auto const &[part, extern_nets] = part_info;

    for (auto k = 0U; k < this->K; ++k) {
        this->gainbucket[k]->clear();
    }
    for (auto i_v = 0U; i_v < this->H.number_of_modules(); ++i_v) {
        auto &vlink = this->gainCalc.vertex_list[i_v];
        auto toPart = 1 - part[i_v];
        this->gainbucket[toPart]->append_direct(vlink);
    }
    for (auto v : this->H.module_fixed) {
        auto i_v = this->H.module_map[v];
        this->lock_all(part[i_v], i_v);
    }
    return totalcost;
}
