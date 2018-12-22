#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <ckpttncpp/bpqueue.hpp> // import bpqueue
#include <ckpttncpp/dllist.hpp>  // import dllink

TEST_CASE("Test BPQueue", "[bpqueue]") {
    auto bpq1 = bpqueue{-10, 10};
    auto bpq2 = bpqueue{-10, 10};

    CHECK(bpq1.is_empty());

    auto d = dllink{};
    auto e = dllink{};
    auto f = dllink{};

    CHECK(d.key == 0);

    bpq1.append(e, 3);
    bpq1.append(f, -10);
    bpq1.append(d, 5);

    bpq2.append(bpq1.popleft(), -6); // d
    bpq2.append(bpq1.popleft(), 3);
    bpq2.append(bpq1.popleft(), 0);

    bpq2.modify_key(d, 15);
    bpq2.modify_key(d, -3);
    CHECK(bpq1.is_empty());
    CHECK(bpq2.get_max() == 6);

    auto nodelist = std::vector<dllink>(10);

    auto i = 0u;
    for (auto &it : nodelist) {
        it.key = 2 * i - 10;
        i += 1;
    }
    bpq1.appendfrom(nodelist);

    auto count = 0u;
    for (auto &node : bpq1) {
        count += 1;
    }
    CHECK(count == 10);
}
