#include <gtest/gtest.h>

#include "restriction_times.hpp"

void PrintTo(const restricted_time::restriction_time& time, ::std::ostream* os)
{
    *os << "with_hours = " << time.with_hours << ", from = " << time.from << " to = " << time.to;
}

TEST(restriction_tests, parse_simple_time)
{
    const std::string example("10:00:00-10");
    restricted_time r(example);

    const restricted_time::restriction_time expected_value { 1, 36000, 36010 };
    const std::vector<restricted_time::restriction_time> expected { expected_value };

    EXPECT_EQ(r.get_restrictions(), expected);
}

TEST(restriction_tests, parse_simple_time_with_overflow)
{
    const std::string example("23:59:40-30");
    restricted_time r(example);

    const restricted_time::restriction_time expected_value { 1, 86380, 10 };
    const std::vector<restricted_time::restriction_time> expected { expected_value };

    EXPECT_EQ(r.get_restrictions(), expected);
}

TEST(restriction_tests, parse_simple_times)
{
    const std::string example("10:00:00-10,00:00:00-20");
    restricted_time r(example);

    const restricted_time::restriction_time first_expected_value { 1, 36000, 36010 };
    const restricted_time::restriction_time second_expected_value { 1, 0, 20 };
    const std::vector<restricted_time::restriction_time> expected { first_expected_value, second_expected_value };

    EXPECT_EQ(r.get_restrictions(), expected);
}

TEST(restriction_tests, parse_time_with_mask)
{
    const std::string example("XX:00:00-10,xx:00:00-10");
    restricted_time r(example);

    const restricted_time::restriction_time expected_value { 0, 0, 10 };
    const std::vector<restricted_time::restriction_time> expected { expected_value, expected_value };

    EXPECT_EQ(r.get_restrictions(), expected);
}
