#pragma once

#include <nlohmann/json.hpp>

#define JSON_CHECK_INVALID_VAR(json, name, type) if (!nlohmann::try_get<type>(json, name).has_value())
#define JSON_REQUIRE_VAR(json, name, type, ret)                                                                        \
    JSON_CHECK_INVALID_VAR(json, name, type)                                                                           \
    {                                                                                                                  \
        return ret;                                                                                                    \
    }
#define JSON_REQUIRE_VAR_DEBUG(json, name, type, ret, debug_component, debug_msg)                                      \
    JSON_CHECK_INVALID_VAR(json, name, type)                                                                           \
    {                                                                                                                  \
        Logger::Error(debug_component, "{} (error=missing or invalid {})", debug_msg, name);                                                                                               \
        return ret;                                                                                                    \
    }

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