#include "util/std/string.h"

#include <numeric>

namespace Actinium::String
{
    std::string Join(const std::vector<std::string>& vec, const std::string& separator)
    {
        if (vec.empty())
        {
            return "";
        }

        return std::accumulate(std::next(vec.begin()), vec.end(), vec[0],
            [&separator](const std::string& a, const std::string& b)
            {
                return a + separator + b;
            });
    }
}