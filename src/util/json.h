#pragma once

#include <nlohmann/json.hpp>

namespace nlohmann
{
    /**
     * Tries to get a value from a JSON object.
     */
    template<typename T> std::optional<T> try_get(const json &j, std::string_view key)
    {
        const auto it = j.find(key);

        if (it == j.end() || it->is_null())
        {
            return std::nullopt;
        }

        try
        {
            return it->get<T>();
        }
        catch (const json::exception &)
        {
            return std::nullopt;
        }
    }
}