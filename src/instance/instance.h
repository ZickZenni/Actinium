#pragma once

#include "game/game.h"

#include <uuid_v4.h>

namespace Actinium
{
    class Instance
    {
    public:
        std::string name;

        explicit Instance(Game *game, std::string name);
        explicit Instance(Game *game, std::string name, const UUIDv4::UUID &uuid);

        [[nodiscard]] Game *GetGame() const
        {
            return m_game;
        }

        [[nodiscard]] const UUIDv4::UUID &GetUUID() const
        {
            return m_uuid;
        }

        static UUIDv4::UUID GenerateUUID();

    private:
        Game *m_game;
        UUIDv4::UUID m_uuid;
    };
}