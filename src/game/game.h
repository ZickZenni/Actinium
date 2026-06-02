#pragma once

#include "util/api/github.h"

namespace Actinium
{
    class Game
    {
    public:
        Game(const std::string_view &id, const std::string_view &name, std::vector<GitHub::Repo> libraries)
            : m_id(id)
            , m_name(name)
            , m_libraries(libraries)
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

        /**
         * Retrieves the libraries that mods may require to function properly.
         */
        [[nodiscard]] const std::vector<GitHub::Repo> &GetLibraries() const
        {
            return m_libraries;
        }

    private:
        std::string_view m_id;
        std::string_view m_name;
        std::vector<GitHub::Repo> m_libraries;
    };
}