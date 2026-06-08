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
        const auto entries = std::filesystem::directory_iterator(location);

        for (const auto& entry : entries)
        {
            if (!std::filesystem::is_directory(entry))
            {
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

            Logger::Info("instance", "discover_mods_mod_found", "Found mod \"{}\"", mod.name);

            m_installed_mods.push_back(mod);
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
            Logger::Error(
                "instance", "load_instance_failed", "instance.json does not exist inside \"{}\"", location.string());
            return nullptr;
        }

        const auto json = nlohmann::json::parse(std::ifstream(instance_file));
        const auto name = nlohmann::try_get<std::string>(json, "name");
        const auto game_id = nlohmann::try_get<std::string>(json, "game");

        if (!name.has_value() || !game_id.has_value())
        {
            Logger::Error("instance", "load_instance_failed",
                "Invalid structured json inside instance.json inside \"{}\"", location.string());
            return nullptr;
        }

        const auto game = Application::GetGameById(game_id.value());

        if (game == nullptr)
        {
            Logger::Error("instance", "load_instance_failed", "Game with id \"{}\" does not exist", game_id.value());
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
}