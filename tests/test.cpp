#include <algorithm>
#include <cstddef>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "utils.h"

TEST_CASE("swap")
{
    int x = 10;
    int y = -20;
    swap(&x, &y);
    REQUIRE(x == -20);
    REQUIRE(y == 10);
}

TEST_CASE("swap with itself")
{
    int x = 69;
    swap(&x, &x);
    REQUIRE(x == 69);
}

static bool cmp_less(int x, int y)
{
    return x < y;
}

static bool cmp_greater(int x, int y)
{
    return x > y;
}

static bool cmp_div8(int x, int y)
{
    return x / 8 < y / 8;
}

static bool cmp_mod7(int x, int y)
{
    return x % 7 < y % 7;
}

static bool cmp_all_equivalent(int, int)
{
    return false;
}

TEST_CASE("sort")
{
    auto data = GENERATE(
            std::vector<int>(),
            std::vector<int>(1, 1337),
            std::vector<int>{11, 22, 33, 44, 55},
            std::vector<int>{-10, -10, -20, -20, -30, -40, -50, -50, -50},
            std::vector<int>{140, 120, 80, 110, 100, 20, 10, 50, 70, 130, 150, 170, 90, 40, 30, 60, 160},
            std::vector<int>{2, -4, -3, 5, 1, 0, 3, -3, -2, 5, 3, 5, -3, -1, 4});

    auto [comparator, comparator_name] = GENERATE(table<bool (*)(int, int), std::string>({
            {&cmp_less,           "less"          },
            {&cmp_greater,        "greater"       },
            {&cmp_div8,           "div 8"         },
            {&cmp_mod7,           "mod 7"         },
            {&cmp_all_equivalent, "all equivalent"}
    }));

    CAPTURE(data, comparator_name);

    std::vector actual = data;
    std::vector expected = data;

    sort_n(actual.data(), actual.size(), comparator);
    std::stable_sort(expected.begin(), expected.end(), comparator);

    REQUIRE(actual == expected);
}

TEST_CASE("rotate")
{
    auto [data, shift] = GENERATE(table<std::vector<int>, std::size_t>({
            {std::vector<int>(),                                    0 },
            {{1337},                                                0 },
            {{1337},                                                1 },
            {{1, 2, 3},                                             0 },
            {{1, 2, 3},                                             1 },
            {{1, 2, 3},                                             2 },
            {{1, 2, 3},                                             3 },
            {{-1, -2, -3, -4},                                      0 },
            {{-1, -2, -3, -4},                                      1 },
            {{-1, -2, -3, -4},                                      2 },
            {{-1, -2, -3, -4},                                      3 },
            {{-1, -2, -3, -4},                                      4 },
            {{50, -10, -60, 0, -50, 40, 10, 30, 20, -40, -30, -20}, 0 },
            {{50, -10, -60, 0, -50, 40, 10, 30, 20, -40, -30, -20}, 1 },
            {{50, -10, -60, 0, -50, 40, 10, 30, 20, -40, -30, -20}, 2 },
            {{50, -10, -60, 0, -50, 40, 10, 30, 20, -40, -30, -20}, 3 },
            {{50, -10, -60, 0, -50, 40, 10, 30, 20, -40, -30, -20}, 4 },
            {{50, -10, -60, 0, -50, 40, 10, 30, 20, -40, -30, -20}, 5 },
            {{50, -10, -60, 0, -50, 40, 10, 30, 20, -40, -30, -20}, 6 },
            {{50, -10, -60, 0, -50, 40, 10, 30, 20, -40, -30, -20}, 7 },
            {{50, -10, -60, 0, -50, 40, 10, 30, 20, -40, -30, -20}, 8 },
            {{50, -10, -60, 0, -50, 40, 10, 30, 20, -40, -30, -20}, 9 },
            {{50, -10, -60, 0, -50, 40, 10, 30, 20, -40, -30, -20}, 10},
            {{50, -10, -60, 0, -50, 40, 10, 30, 20, -40, -30, -20}, 11},
            {{50, -10, -60, 0, -50, 40, 10, 30, 20, -40, -30, -20}, 12},
    }));

    CAPTURE(data, shift);

    std::vector actual = data;
    std::vector expected = data;

    rotate(actual.data(), actual.data() + actual.size(), shift);
    if (shift != expected.size())
    {
        std::rotate(expected.begin(), expected.begin() + shift, expected.end());
    }

    REQUIRE(actual == expected);
}

TEST_CASE("copy simple")
{
    auto [from_data, to_data] = GENERATE(table<std::vector<int>, std::vector<int>>({
            {std::vector<int>(),                        std::vector<int>()                       },
            {{1337},                                    {228}                                    },
            {{1, 2, 3, 4, 5, 6},                        {-1, -2, -3, -4, -5, -6}                 },
            {{1, -1, 4, -4, 2, 3, 4, 1, -2, -1, 5, -2}, {-4, 2, 4, -4, -4, 0, -4, 1, 2, -1, 3, 0}},
    }));

    CAPTURE(from_data, to_data);

    std::vector from_actual = from_data;
    std::vector to_actual = to_data;
    std::vector from_expected = from_data;
    std::vector to_expected = to_data;

    copy_n(from_actual.data(), from_actual.size(), to_actual.data());
    std::copy_n(from_expected.begin(), from_expected.size(), to_expected.begin());

    REQUIRE(from_actual == from_expected); // From data should be unchanged
    REQUIRE(to_actual == to_expected);
}

TEST_CASE("copy overlapping")
{
    static const std::vector vector2(1, 1337);
    static const std::vector vector3 = {11, 22, 33, 44, 55};
    static const std::vector vector4 = {-1, -2, -3, -4, -5, -6, -7};
    static const std::vector vector5 = {140, 120, 80, 110, 100, 20, 10, 50, 70, 130, 150, 170, 90, 40, 30, 60, 160};
    static const std::vector vector6 = {2, -4, -3, 5, 1, 0, 3, -3, -2, 5, 3, 5, -3, -1, 4};

    auto [data,
          from_start_index,
          to_start_index,
          length] = GENERATE(table<std::vector<int>, std::size_t, std::size_t, std::size_t>({
            {vector3, 0, 2, 3 },
            {vector4, 3, 1, 4 },
            {vector2, 0, 0, 1 },
            {vector6, 2, 2, 9 },
            {vector5, 2, 7, 10},
            {vector5, 7, 2, 9 },
    }));

    CAPTURE(data, from_start_index, to_start_index, length);

    std::vector actual = data;
    std::vector expected = data;

    copy_n(actual.data() + from_start_index, length, actual.data() + to_start_index);
    std::memmove(expected.data() + to_start_index, expected.data() + from_start_index, length * sizeof(expected[0]));

    REQUIRE(actual == expected);
}
