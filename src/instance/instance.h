#pragma once

#include "game/game.h"

namespace Actinium
{
    class Instance
    {
    public:
        std::string name;

        explicit Instance(const Game *game, std::string name);
        explicit Instance(const Game *game, std::string name, const std::string &directory_name);

        /**
         * Saves all instances data to the disk.
         */
        void Save() const;

        /**
         * Retrieves the directory name where all data is stored in.
         */
        [[nodiscard]] const std::string &GetDirectoryName() const
        {
            return m_directory_name;
        }

        /**
         * Retrieves the game that the instance is for.
         */
        [[nodiscard]] const Game *GetGame() const
        {
            return m_game;
        }

        /**
         * Loads an instance into memory by reading the data saved on the disk.
         */
        static Instance *Load(const std::string &directory_name);

    private:
        std::string m_directory_name;
        const Game *m_game;
    };
}