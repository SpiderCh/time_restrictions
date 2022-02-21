#include "restriction_times.hpp"

#include <charconv>
#include <ctime>

#include <array>
#include <numeric>

namespace
{
constexpr int max_seconds_in_minute = 60;
constexpr int max_seconds_in_hour = max_seconds_in_minute * 60;
constexpr int max_seconds_in_day = max_seconds_in_hour * 24;

constexpr std::array<char, 4> delimiters {':', ':', '-', ','};
constexpr std::array<int, 3> convert_to_seconds {max_seconds_in_hour, max_seconds_in_minute, 1};

int number(std::string_view s)
{
    int num = 0;
    std::from_chars(s.begin(), s.end(), num);
    return num;
}

}

restricted_time::restricted_time(const std::string & times)
{
    for (size_t i = 0; i + 8 <= times.size(); ++i)
    {
        restriction_time r{};
        if (!std::isdigit(times[i]))
            r.with_hours = 0;

        const std::array<const char *, 3> nums {times.data() + i, times.data() + i + 3, times.data() + i + 6};
        r.from = std::inner_product(nums.begin(), nums.end(), convert_to_seconds.begin(), 0, std::plus<>(), [](const char * n, int value){
            return value * number({n, 2});
        });

        int duration = 0;
        size_t begin = times.find(delimiters[2], i);
        i = times.find(delimiters[3], i);

        if (i == std::string::npos)
            i = times.size();

        if (begin != std::string::npos)
        {
            ++begin;
            duration = number({times.c_str() + begin, i - begin});
        }

        r.to = r.from + duration;

        const int max_seconds = r.with_hours ? max_seconds_in_day : max_seconds_in_hour;
        if (r.to > max_seconds)
            r.to -= max_seconds;

        restrictions_.emplace_back(r);
    }
}

std::vector<restricted_time::restriction_time> restricted_time::get_restrictions() const
{
    return restrictions_;
}

bool restricted_time::is_now_restricted() const
{
    return is_time_restricted(std::time(nullptr));
}

bool restricted_time::is_time_restricted(std::time_t time) const
{
    if (restrictions_.empty())
        return false;

    std::tm ct{};
    localtime_r(&time, &ct);
    int current_time_point = ct.tm_sec + ct.tm_min * max_seconds_in_minute;
    for (auto && restriction : restrictions_)
    {
        current_time_point += ct.tm_hour * max_seconds_in_hour * restriction.with_hours;
        if (is_time_point_restricted(restriction, current_time_point))
            return true;
    }
    return false;
}

bool restricted_time::is_time_point_restricted(const restriction_time & rest, int tp)
{
    if (rest.to > rest.from)
        return rest.from <= tp && tp <= rest.to;
    return rest.from <= tp || tp <= rest.to;
}
