#pragma once

#include "game/game.h"

#include <uuid_v4.h>

namespace Actinium
{
    class Instance
    {
    public:
        std::string name;

        explicit Instance(const Game *game, std::string name);
        explicit Instance(const Game *game, std::string name, const UUIDv4::UUID &uuid);

        void Save() const;

        /**
         * Retrieves the game that the instance is for.
         */
        [[nodiscard]] const Game *GetGame() const
        {
            return m_game;
        }

        /**
         * Retrieves the UUID of the instance.
         */
        [[nodiscard]] const UUIDv4::UUID &GetUUID() const
        {
            return m_uuid;
        }

        static UUIDv4::UUID GenerateUUID();

        static Instance* Load(const std::string& directory_name);

    private:
        const Game *m_game;
        UUIDv4::UUID m_uuid;
    };
}