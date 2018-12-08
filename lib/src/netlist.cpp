#include <ckpttncpp/netlist.hpp>
#include <py2cpp/py2cpp.hpp>

/**
 * @brief Construct a new Netlist object
 *
 * @param G
 * @param module_list
 * @param net_list
 * @param module_fixed
 */
Netlist::Netlist(xn::grAdaptor<graph_t> &&G, size_t num_modules,
                 size_t num_nets, nodevec_t module_fixed)
    : G{std::move(G)}, num_modules{num_modules}, num_nets{num_nets},
      module_fixed{module_fixed} //
{
    this->has_fixed_modules = (!this->module_fixed.empty());
    auto deg_cmp = [this](size_t v, size_t w) -> size_t {
        return this->G.degree(v) < this->G.degree(w);
    };
    auto rng = py::range(this->number_of_modules());
    auto result1 = std::max_element(rng.begin(), rng.end(), deg_cmp);
    this->max_degree = this->G.degree(*result1);
    auto rng_net =
        py::range2(this->number_of_modules(), this->number_of_nodes());
    auto result2 = std::max_element(rng_net.begin(), rng_net.end(), deg_cmp);
    this->max_net_degree = this->G.degree(*result2);
}
