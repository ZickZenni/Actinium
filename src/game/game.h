#pragma once

#include "loader/loader_library.h"

namespace Actinium
{
    class Game
    {
    public:
        Game(const std::string_view &id, const std::string_view &name, const std::string_view &executable_name,
            const std::vector<LoaderLibrary> &libraries, const std::optional<uint64_t> steam_app_id = std::nullopt,
            const std::optional<std::vector<std::string>> &steam_start_parameters = std::nullopt)
            : m_id(id)
            , m_name(name)
            , m_executable_name(executable_name)
            , m_libraries(libraries)
            , m_steam_app_id(steam_app_id)
            , m_steam_start_parameters(steam_start_parameters)
        {
        }

        /**
         * Retrieves the identifier of the game.
         */
        [[nodiscard]] const std::string_view &GetId() const
        {
            return m_id;
        }

        /**
         * Retrieves the display name of the game.
         */
        [[nodiscard]] const std::string_view &GetName() const
        {
            return m_name;
        }

        [[nodiscard]] const std::string_view &GetExecutableName() const
        {
            return m_executable_name;
        }

        /**
         * Retrieves the libraries that mods may require to function properly.
         */
        [[nodiscard]] const std::vector<LoaderLibrary> &GetLibraries() const
        {
            return m_libraries;
        }

        [[nodiscard]] std::optional<uint64_t> GetSteamAppId() const
        {
            return m_steam_app_id;
        }

        [[nodiscard]] std::optional<std::vector<std::string>> GetSteamStartParameters() const
        {
            return m_steam_start_parameters;
        }

    private:
        std::string_view m_id;
        std::string_view m_name;
        std::string_view m_executable_name;
        std::vector<LoaderLibrary> m_libraries;
        std::optional<uint64_t> m_steam_app_id;
        std::optional<std::vector<std::string>> m_steam_start_parameters;
    };
}