#include <catch.hpp>
#include <ckpttncpp/dllist.hpp> // import  dllink
#include <ckpttncpp/robin.hpp>  // import  dllink

TEST_CASE("Test dllist", "[dllist]") {
    auto L1 = dllink{0};
    auto L2 = dllink{0};
    auto d = dllink{0};
    auto e = dllink{0};
    auto f = dllink{0};
    CHECK(L1.is_empty());

    L1.appendleft(e);
    CHECK(!L1.is_empty());

    L1.appendleft(f);
    L1.append(d);
    L2.append(L1.pop());
    L2.append(L1.popleft());
    CHECK(!L1.is_empty());
    CHECK(!L2.is_empty());

    auto count = 0U;
    for (auto &_ : L2.items()) {
        count += 1;
    }
    CHECK(count == 2);
}

TEST_CASE("Test robin", "[robin]") {
    auto RR = robin<int>(6);
    auto count = 0U;
    for (auto _ : RR.exclude(2)) {
        count += 1;
    }
    CHECK(count == 5);
}
