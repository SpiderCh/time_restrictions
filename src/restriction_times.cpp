#include "restriction_times.hpp"

#include <ctime>
#include <cstring>

#include <array>

namespace
{
constexpr int max_seconds_in_minute = 60;
constexpr int max_seconds_in_hour = max_seconds_in_minute * 60;
constexpr int max_seconds_in_day = max_seconds_in_hour * 24;

constexpr std::array<char, 4> delimiters {':', ':', '-', ','};
constexpr std::array<int, 3> convert_to_seconds {max_seconds_in_hour, max_seconds_in_minute, 1};
}

restricted_time::restricted_time(const std::string & times)
{
    char num[128] {};
    for (size_t i = 0; i < times.size();)
    {
        restriction_time r{};
        if (!isdigit(times[i]))
            r.with_hours = 0;

        for (size_t j = 0; j < delimiters.size() - 1; ++j, i += 3)
        {
            strncpy(num, times.c_str() + i, 2);
            r.from += atoi(num) * convert_to_seconds[j];
        }

        const size_t begin = i;
        i = times.find(delimiters[3], i);
        if (i == std::string::npos)
            i = times.size();

        strncpy(num, times.c_str() + begin, i - begin);
        r.to = r.from + atoi(num);

        const int max_seconds = r.with_hours ? max_seconds_in_day : max_seconds_in_hour;
        if (r.to > max_seconds)
            r.to -= max_seconds;

        restrictions_.emplace_back(r);
        ++i;
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
