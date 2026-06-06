#include "util/std/string.h"

namespace Actinium::String
{
    std::string Join(const std::span<const std::string>& values, const std::string_view& separator)
    {
        if (values.empty())
        {
            return "";
        }

        auto total_size = separator.size() * (values.size() - 1);

        for (const auto& value : values)
        {
            total_size += value.size();
        }

        std::string result;
        result.reserve(total_size);

        result += values.front();

        for (auto it = std::next(values.begin()); it != values.end(); ++it)
        {
            result += separator;
            result += *it;
        }

        return result;
    }
}