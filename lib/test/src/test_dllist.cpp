#include <catch.hpp>
#include <ckpttncpp/dllist.hpp> // import  dllink
#include <ckpttncpp/robin.hpp> // import  dllink

TEST_CASE("Test dllist", "[dllist]") {
    auto L1 = dllink{};
    auto L2 = dllink{};
    auto d = dllink{};
    auto e = dllink{};
    auto f = dllink{};
    CHECK(L1.is_empty());

    L1.appendleft(e);
    CHECK(!L1.is_empty());

    L1.appendleft(f);
    L1.append(d);
    L2.append(L1.pop());
    L2.append(L1.popleft());
    CHECK(!L1.is_empty());
    CHECK(!L2.is_empty());

    auto count = 0u;
    for (auto &node : L2) {
        count += 1;
    }
    CHECK(count == 2);
}

TEST_CASE("Test robin", "[robin]") {
    auto RR = robin(6);
    auto count = 0u;
    for (auto k : RR.exclude(2)) {
        count += 1;
    }
    CHECK(count == 5);
}
