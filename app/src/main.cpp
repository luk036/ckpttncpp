#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <ckpttncpp/dllist.hpp> // import  dllist, dllink
#include <ckpttncpp/bpqueue.hpp> // import  bpqueue

TEST_CASE("Test BPQueue 2", "[bpqueue]") {
    auto bpq1 = bpqueue(-11, 11);
    auto bpq2 = bpqueue(-11, 11);

    CHECK (bpq1.is_empty());

    auto d = dllink();
    auto e = dllink();
    auto f = dllink();

    bpq1.append(e, 3);
    bpq1.append(f, -10);
    bpq1.append(d, 5);

    bpq2.append(bpq1.popleft(), -6); // d
    bpq2.append(bpq1.popleft(), 3);
    bpq2.append(bpq1.popleft(), 0);

    bpq2.increase_key(d, 15);
    bpq2.decrease_key(d, 3);
    CHECK (bpq1.is_empty());
    CHECK (bpq2.get_max() == 6);

    auto nodelist = std::vector<dllink>(10);
    auto gainlist = std::vector<int>{-3, -5, 4, 0, 7, 4, 7, -2, 4, 6};
    bpq1.appendfrom(nodelist, gainlist);
    
    auto count = 0u;
    for (auto& node : bpq1) {
        count += 1;
    }
    CHECK (count == 10);
}
