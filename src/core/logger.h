#pragma once

#include <spdlog/common.h>
#include <spdlog/spdlog.h>

namespace Actinium::Logger
{
    inline void Init()
    {
        spdlog::set_level(spdlog::level::trace);
        spdlog::set_pattern("[%Y-%m-%d][%H:%M:%S.%e]%v");
    }

    template<typename T> void Log(const spdlog::level::level_enum level, std::string_view component, const T &msg)
    {
        spdlog::log(level, "[{}][{}] {}", component, spdlog::level::to_string_view(level), msg);
    }

    template<typename... Args> void Log(const spdlog::level::level_enum level, std::string_view component,
        typename fmt::fstring<Args...>::t message_format, Args &&...args)
    {
        spdlog::log(level, "[{}][{}] {}", component, spdlog::level::to_string_view(level),
            fmt::format(message_format, std::forward<Args>(args)...));
    }

    template<typename T> void Trace(const std::string_view component, const T &msg)
    {
        Log<T>(spdlog::level::trace, component, msg);
    }

    template<typename... Args>
    void Trace(std::string_view component, fmt::format_string<Args...> message_format, Args &&...args)
    {
        Log(spdlog::level::trace, component, message_format, std::forward<Args>(args)...);
    }

    template<typename T> void Debug(const std::string_view component, const T &msg)
    {
        Log<T>(spdlog::level::debug, component, msg);
    }

    template<typename... Args>
    void Debug(std::string_view component, fmt::format_string<Args...> message_format, Args &&...args)
    {
        Log(spdlog::level::debug, component, message_format, std::forward<Args>(args)...);
    }

    template<typename T> void Info(const std::string_view component, const T &msg)
    {
        Log<T>(spdlog::level::info, component, msg);
    }

    template<typename... Args>
    void Info(std::string_view component, fmt::format_string<Args...> message_format, Args &&...args)
    {
        Log(spdlog::level::info, component, message_format, std::forward<Args>(args)...);
    }

    template<typename T> void Warn(const std::string_view component, const T &msg)
    {
        Log<T>(spdlog::level::warn, component, msg);
    }

    template<typename... Args>
    void Warn(std::string_view component, fmt::format_string<Args...> message_format, Args &&...args)
    {
        Log(spdlog::level::warn, component, message_format, std::forward<Args>(args)...);
    }

    template<typename T> void Error(const std::string_view component, const T &msg)
    {
        Log<T>(spdlog::level::err, component, msg);
    }

    template<typename... Args>
    void Error(std::string_view component, fmt::format_string<Args...> message_format, Args &&...args)
    {
        Log(spdlog::level::err, component, message_format, std::forward<Args>(args)...);
    }
}