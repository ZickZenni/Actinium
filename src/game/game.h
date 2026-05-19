#pragma once

#include <string>

namespace Actinium
{
    class Game
    {
    public:
        constexpr Game(const std::string_view &id, const std::string_view &name)
            : m_id(id)
            , m_name(name)
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

    private:
        std::string_view m_id;
        std::string_view m_name;
    };
}