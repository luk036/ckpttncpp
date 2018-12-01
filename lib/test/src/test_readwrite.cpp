// -*- coding: utf-8 -*-
#include <catch.hpp>
#include <ckpttncpp/netlist.hpp>

extern Netlist readNetD(const char *netDFileName);
void readAre(Netlist& H, const char *areFileName);

TEST_CASE("Test Read Dwarf", "[test_readwrite]")
{
    auto H = readNetD("../../testcases/dwarf1.netD");
    readAre(H, "../../testcases/dwarf1.are");

    CHECK(H.number_of_modules() == 7);
    CHECK(H.number_of_nets() == 5);
    CHECK(H.number_of_pins() == 13);
    CHECK(H.get_max_degree() == 3);
    CHECK(H.get_max_net_degree() == 3);
    CHECK(!H.has_fixed_modules);
    CHECK(H.get_module_weight(1) == 2);
}

TEST_CASE("Test Read p1", "[test_readwrite]")
{
    auto H = readNetD("../../testcases/p1.net");

    CHECK(H.number_of_modules() == 833);
    CHECK(H.number_of_nets() == 902);
    CHECK(H.number_of_pins() == 2908);
    CHECK(H.get_max_degree() == 9);
    CHECK(H.get_max_net_degree() == 18);
    CHECK(!H.has_fixed_modules);
    CHECK(H.get_module_weight(1) == 1);
}