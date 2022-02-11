#pragma once

#include <cstdint>
#include <string>
#include <vector>

class restricted_time
{
public:
    struct restriction_time
    {
        int with_hours {1};
        int from {0};
        int to {0};

        bool operator==(const restriction_time & other) const
        {
            return with_hours == other.with_hours &&
                   from == other.from &&
                   to == other.to;
        }

        bool operator!=(const restriction_time & other) const
        {
            return !(*this == other);
        }
    };

    restricted_time(const std::string & times);

    std::vector<restriction_time> get_restrictions() const;
private:
    static bool is_time_point_restricted(const restriction_time & rest, int tp);

private:
    std::vector<restriction_time> restrictions_;
};
