#include "instance.h"

#include "core/application.h"
#include "core/logger.h"
#include "util/fs/path.h"
#include "util/lib/json.h"

#include <fstream>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <utility>

namespace Actinium
{
    Instance::Instance(Game* game, std::string name)
        : Instance(game, std::move(name), Path::SanitizeName(name))
    {
    }

    Instance::Instance(Game* game, std::string name, const std::string& directory_name)
        : name(std::move(name))
        , m_directory_name(directory_name)
        , m_game(game)
        , m_mods_folder_watch_id(-1)
    {
    }

    void Instance::DiscoverMods()
    {
        constexpr std::string_view DISABLED_PREFIX = "DISABLED_";

        const auto location = GetAbsolutePath() / "mods";

        Logger::Info("instance::discover_mods", "Discovering mods (path={})", location.string());

        const auto entries = std::filesystem::directory_iterator(location);

        for (const auto& entry : entries)
        {
            if (!std::filesystem::is_directory(entry))
            {
                Logger::Warn(
                    "instance::discover_mods", "Skipping non-directory entry (path={})", entry.path().string());
                continue;
            }

            auto mod_name = entry.path().filename().string();
            auto disabled = false;

            if (mod_name.starts_with(DISABLED_PREFIX))
            {
                mod_name = mod_name.substr(DISABLED_PREFIX.size());
                disabled = true;
            }

            InstalledMod mod;
            mod.name = mod_name;
            mod.path = entry.path();
            mod.disabled = disabled;

            m_installed_mods.push_back(mod);

            Logger::Info("instance::discover_mods", "Discovered mod (name={}, path={})", mod_name, mod.path.string());
        }
    }

    void Instance::Save() const
    {
        const auto location = GetAbsolutePath();
        const auto instance_file = location / "instance.json";

        std::filesystem::create_directories(location / "mods");

        const nlohmann::json json {{"name", name}, {"game", m_game->id}};

        std::ofstream(instance_file) << json.dump(4);
    }

    std::filesystem::path Instance::GetAbsolutePath() const
    {
        return GetAbsolutePath(m_directory_name);
    }

    const std::string& Instance::GetDirectoryName() const
    {
        return m_directory_name;
    }

    Game* Instance::GetGame() const
    {
        return m_game;
    }

    Instance* Instance::Load(const std::string& directory_name)
    {
        const auto location = GetAbsolutePath(directory_name);
        const auto instance_file = location / "instance.json";

        if (!std::filesystem::exists(instance_file))
        {
            Logger::Error("instance::load", "Failed to load instance (error=instance.json file does not exist)");
            return nullptr;
        }

        const auto json = nlohmann::json::parse(std::ifstream(instance_file));
        const auto name = nlohmann::try_get<std::string>(json, "name");
        const auto game_id = nlohmann::try_get<std::string>(json, "game");

        if (!name.has_value() || !game_id.has_value())
        {
            Logger::Error("instance::load", "Failed to load instance (error=Invalid structured instance.json)");
            return nullptr;
        }

        const auto game = Application::GetGameById(game_id.value());

        if (game == nullptr)
        {
            Logger::Error("instance::load", "Failed to load instance (error=Game with the value has not been found, value={})", game_id.value());
            return nullptr;
        }

        const auto instance = new Instance(game, name.value(), directory_name);
        instance->DiscoverMods();

        return instance;
    }

    std::filesystem::path Instance::GetAbsolutePath(const std::string& directory_name)
    {
        return Application::GetAppDataPath() / "instances" / directory_name;
    }

    void Instance::handleFileAction(efsw::WatchID watch_id, const std::string& dir, const std::string& file_name,
        const efsw::Action action, const std::string& old_file_name)
    {
        switch (action)
        {
            case efsw::Actions::Add:
                Logger::Debug("instance::watcher", "(event=add, file={}, dir={})", file_name, dir);
                break;
            case efsw::Actions::Delete:
                Logger::Debug("instance::watcher", "(event=delete, file={}, dir={})", file_name, dir);
                break;
            case efsw::Actions::Modified:
                Logger::Debug("instance::watcher", "(event=modify, file={}, dir={})", file_name, dir);
                break;
            case efsw::Actions::Moved:
                Logger::Debug("instance::watcher", "(event=move, old_file={}, new_file={}, dir={})", old_file_name,
                    file_name, dir);
                break;
            default:
                break;
        }
    }
}