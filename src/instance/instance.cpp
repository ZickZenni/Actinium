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

            const auto mod = AddMod(entry.path());
            Logger::Info("instance::discover_mods", "Discovered mod (name={}, path={})", mod->name, mod->path.string());
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
            Logger::Error("instance::load",
                "Failed to load instance (error=Game with the value has not been found, value={})", game_id.value());
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
        const auto absolute_path = GetAbsolutePath() / "mods" / file_name;

        switch (action)
        {
            case efsw::Actions::Add:
                Logger::Debug("instance::watcher", "(event=add, file={}, dir={})", file_name, dir);

                if (std::filesystem::is_directory(absolute_path))
                {
                    const auto mod = AddMod(absolute_path);
                    Logger::Info("instance::watcher", "Added mod (name={}, path={})", mod->name, mod->path.string());
                }
                break;
            case efsw::Actions::Delete:
            {
                Logger::Debug("instance::watcher", "(event=delete, file={}, dir={})", file_name, dir);

                const auto original_size = m_installed_mods.size();

                std::erase_if(m_installed_mods,
                    [&](const InstalledMod& mod)
                    {
                        return mod.path == absolute_path;
                    });

                if (m_installed_mods.size() < original_size)
                {
                    Logger::Info("instance::watcher", "Deleted mod (path={})", absolute_path.string());
                }

                break;
            }
            case efsw::Actions::Modified:
                Logger::Debug("instance::watcher", "(event=modify, file={}, dir={})", file_name, dir);
                break;
            case efsw::Actions::Moved:
            {
                Logger::Debug("instance::watcher", "(event=move, old_file={}, new_file={}, dir={})", old_file_name,
                    file_name, dir);

                const auto old_absolute_path = GetAbsolutePath() / "mods" / old_file_name;
                const auto new_absolute_path = absolute_path;

                for (auto& mod : m_installed_mods)
                {
                    if (mod.path == old_absolute_path)
                    {
                        mod.name = RemoveDisabledPrefix(file_name, &mod.disabled);
                        mod.path = new_absolute_path;

                        Logger::Info("instance::watcher", "Moved / Renamed mod (name={}, new_path={}, old_path={})",
                            mod.name, new_absolute_path.string(), old_absolute_path.string());

                        break;
                    }
                }

                break;
            }
            default:
                break;
        }
    }

    InstalledMod* Instance::AddMod(const std::filesystem::path& directory_path)
    {
        auto disabled = false;
        const auto mod_name = RemoveDisabledPrefix(directory_path.filename().string(), &disabled);

        m_installed_mods.push_back(InstalledMod {
            .name = mod_name,
            .path = directory_path,
            .disabled = disabled,
        });

        return &m_installed_mods.back();
    }

    std::string Instance::RemoveDisabledPrefix(const std::string& name, bool* out_is_disabled)
    {
        constexpr std::string_view DISABLED_PREFIX = "DISABLED_";

        if (name.starts_with(DISABLED_PREFIX))
        {
            if (out_is_disabled != nullptr)
            {
                *out_is_disabled = true;
            }

            return name.substr(DISABLED_PREFIX.size());
        }

        if (out_is_disabled != nullptr)
        {
            *out_is_disabled = false;
        }

        return name;
    }
}