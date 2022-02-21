#include <gtest/gtest.h>

#include "restriction_times.hpp"

void PrintTo(const restricted_time::restriction_time& time, ::std::ostream* os)
{
    *os << "with_hours = " << time.with_hours << ", from = " << time.from << " to = " << time.to;
}

namespace
{
std::time_t generate_time_t(int hours, int minutes, int seconds)
{
    tm time_info;
    time_info.tm_year = 70;
    time_info.tm_mon = 0;
    time_info.tm_mday = 1;
    time_info.tm_hour = hours;
    time_info.tm_min = minutes;
    time_info.tm_sec = seconds;

    return mktime(&time_info);
}
}

TEST(restriction_tests, parse_invalid_time)
{
    const std::string example("10:00:0");
    const restricted_time r(example);

    const std::vector<restricted_time::restriction_time> expected;

    EXPECT_EQ(r.get_restrictions(), expected);
}

TEST(restriction_tests, parse_invalid_duration)
{
    const std::string example("10:00:00,11:00:00");
    const restricted_time r(example);

    const restricted_time::restriction_time expected_value { 1, 36000, 36000 };
    const restricted_time::restriction_time expected_value_2 { 1, 39600, 39600 };
    const std::vector<restricted_time::restriction_time> expected { expected_value, expected_value_2};

    EXPECT_EQ(r.get_restrictions(), expected);
}

TEST(restriction_tests, parse_simple_time)
{
    const std::string example("10:00:00-30");
    const restricted_time r(example);

    const restricted_time::restriction_time expected_value { 1, 36000, 36030 };
    const std::vector<restricted_time::restriction_time> expected { expected_value };

    EXPECT_EQ(r.get_restrictions(), expected);
}

TEST(restriction_tests, parse_simple_time_with_overflow)
{
    const std::string example("23:59:40-30");
    const restricted_time r(example);

    const restricted_time::restriction_time expected_value { 1, 86380, 10 };
    const std::vector<restricted_time::restriction_time> expected { expected_value };

    EXPECT_EQ(r.get_restrictions(), expected);
}

TEST(restriction_tests, parse_simple_times)
{
    const std::string example("10:00:00-10,00:00:00-20");
    const restricted_time r(example);

    const restricted_time::restriction_time first_expected_value { 1, 36000, 36010 };
    const restricted_time::restriction_time second_expected_value { 1, 0, 20 };
    const std::vector<restricted_time::restriction_time> expected { first_expected_value, second_expected_value };

    EXPECT_EQ(r.get_restrictions(), expected);
}

TEST(restriction_tests, parse_time_with_mask)
{
    const std::string example("XX:00:00-10,xx:00:00-10");
    const restricted_time r(example);

    const restricted_time::restriction_time expected_value { 0, 0, 10 };
    const std::vector<restricted_time::restriction_time> expected { expected_value, expected_value };

    EXPECT_EQ(r.get_restrictions(), expected);
}

TEST(restriction_tests, check_restrictions_simple)
{
    const std::string example("10:00:00-10");
    const restricted_time r(example);

    EXPECT_TRUE(r.is_time_restricted(generate_time_t(10, 0, 5)));
    EXPECT_FALSE(r.is_time_restricted(generate_time_t(9, 0, 5)));
}

TEST(restriction_tests, check_restrictions_overlaped)
{
    const std::string example("23:59:40-30");
    const restricted_time r(example);

    EXPECT_TRUE(r.is_time_restricted(generate_time_t(23, 59, 40)));
    EXPECT_TRUE(r.is_time_restricted(generate_time_t(0, 0, 10)));

    EXPECT_FALSE(r.is_time_restricted(generate_time_t(23, 59, 39)));
    EXPECT_FALSE(r.is_time_restricted(generate_time_t(0, 0, 11)));
}
