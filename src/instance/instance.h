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
        explicit Instance(Game *game, std::string name, std::string directory_name);

        /**
         * Discovers all mods saved on the disk.
         */
        void DiscoverMods();

        /**
         * Saves all instances data to the disk.
         */
        void Save() const;

        /**
         * Enables or disables the specified mod.
         */
        void SetModEnabled(const InstalledMod &mod, bool state);

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
         * Retrieves the mods of the instance.
         */
        [[nodiscard]] const std::vector<InstalledMod> &GetMods() const;

        /**
         * Loads an instance into memory by reading the data saved on the disk.
         */
        static Instance *Load(const std::string &directory_name);

        /**
         * Constructs the absolute path of an instance folder.
         */
        static std::filesystem::path GetAbsolutePath(const std::string &directory_name);

        /**
         * efsw::FileWatchListener: Handles the action file action
         *
         * @param watch_id The watch id for the directory
         * @param dir The directory
         * @param file_name The filename that was accessed (not the full path)
         * @param action Action that was performed
         * @param old_file_name The name of the file or directory moved
         */
        void handleFileAction(efsw::WatchID watch_id, const std::string &dir, const std::string &file_name,
            efsw::Action action, const std::string &old_file_name) override;

    private:
        std::string m_directory_name;
        Game *m_game;
        std::vector<InstalledMod> m_installed_mods;
        efsw::WatchID m_mods_folder_watch_id;

        /**
         * Adds a new mod to the instance.
         */
        InstalledMod *AddMod(const std::filesystem::path &directory_path);

        /**
         * Removes the disabled prefix from the given name.
         */
        static std::string RemoveDisabledPrefix(const std::string &name, bool *out_is_disabled);

        friend class Application;
    };
}