#pragma once

#include "game/game.h"
#include "installed_mod.h"

#include <efsw/efsw.hpp>
#include <filesystem>

namespace Actinium
{
    class Instance : public efsw::FileWatchListener
    {
    public:
        std::string name;

        explicit Instance(Game *game, std::string name);
        explicit Instance(Game *game, std::string name, const std::string &directory_name);

        void DiscoverMods();

        /**
         * Saves all instances data to the disk.
         */
        void Save() const;

        /**
         * Retrieves the absolute path of the folder, where all data is stored in.
         */
        [[nodiscard]] std::filesystem::path GetAbsolutePath() const;

        /**
         * Retrieves the directory name where all data is stored in.
         */
        [[nodiscard]] const std::string &GetDirectoryName() const;

        /**
         * Retrieves the game that the instance is for.
         */
        [[nodiscard]] Game *GetGame() const;

        /**
         * Loads an instance into memory by reading the data saved on the disk.
         */
        static Instance *Load(const std::string &directory_name);

        /**
         * Constructs the absolute path of an instance folder.
         */
        static std::filesystem::path GetAbsolutePath(const std::string &directory_name);

        void handleFileAction(efsw::WatchID watch_id, const std::string &dir, const std::string &file_name,
            efsw::Action action, const std::string &old_file_name) override;

    private:
        std::string m_directory_name;
        Game *m_game;
        std::vector<InstalledMod> m_installed_mods;
        efsw::WatchID m_mods_folder_watch_id;

        friend class Application;
    };
}