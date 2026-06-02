#pragma once

#include "game/game.h"

#include <filesystem>

namespace Actinium
{
    class Instance
    {
    public:
        std::string name;

        explicit Instance(Game *game, std::string name);
        explicit Instance(Game *game, std::string name, const std::string &directory_name);

        /**
         * Saves all instances data to the disk.
         */
        void Save() const;

        /**
         * Retrieves the absolute path of the folder, where all data is stored in.
         */
        std::filesystem::path GetAbsolutePath() const;

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
        [[nodiscard]] Game *GetGame() const
        {
            return m_game;
        }

        /**
         * Loads an instance into memory by reading the data saved on the disk.
         */
        static Instance *Load(const std::string &directory_name);

        /**
         * Constructs the absolute path of an instance folder.
         */
        static std::filesystem::path GetAbsolutePath(const std::string &directory_name);

    private:
        std::string m_directory_name;
        Game *m_game;
    };
}