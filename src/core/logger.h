#pragma once

#include <spdlog/common.h>
#include <spdlog/spdlog.h>

namespace Actinium::Logger
{
    inline std::string Quote(const std::string_view &value)
    {
        std::string result;
        result.reserve(value.size() + 2);

        result.push_back('"');

        for (const char c : value)
        {
            if (c == '"' || c == '\\')
            {
                result.push_back('\\');
            }

            result.push_back(c);
        }

        result.push_back('"');
        return result;
    }

    template<typename... Args> void Log(spdlog::level::level_enum level, std::string_view component,
        std::string_view event, fmt::format_string<Args...> message_format, Args &&...args)
    {
        spdlog::log(level, "component={} event={} message={}", component, event,
            Quote(fmt::format(message_format, std::forward<Args>(args)...)));
    }

    template<typename... Args> void Trace(
    std::string_view component, std::string_view event, fmt::format_string<Args...> message_format, Args &&...args)
    {
        Log(spdlog::level::trace, component, event, message_format, std::forward<Args>(args)...);
    }

    template<typename... Args> void Debug(
        std::string_view component, std::string_view event, fmt::format_string<Args...> message_format, Args &&...args)
    {
        Log(spdlog::level::debug, component, event, message_format, std::forward<Args>(args)...);
    }

    template<typename... Args> void Info(
        std::string_view component, std::string_view event, fmt::format_string<Args...> message_format, Args &&...args)
    {
        Log(spdlog::level::info, component, event, message_format, std::forward<Args>(args)...);
    }

    template<typename... Args> void Warn(
        std::string_view component, std::string_view event, fmt::format_string<Args...> message_format, Args &&...args)
    {
        Log(spdlog::level::warn, component, event, message_format, std::forward<Args>(args)...);
    }

    template<typename... Args> void Error(
        std::string_view component, std::string_view event, fmt::format_string<Args...> message_format, Args &&...args)
    {
        Log(spdlog::level::err, component, event, message_format, std::forward<Args>(args)...);
    }
}