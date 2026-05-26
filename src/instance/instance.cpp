#include "instance.h"

#include "core/application.h"
#include "util/json.h"
#include "util/path.h"

#include <fstream>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <utility>

namespace Actinium
{
    Instance::Instance(const Game* game, std::string name)
        : Instance(game, std::move(name), PathUtils::SanitizeName(name))
    {
    }

    Instance::Instance(const Game* game, std::string name, const std::string& directory_name)
        : name(std::move(name))
        , m_directory_name(directory_name)
        , m_game(game)
    {
    }

    void Instance::Save() const
    {
        const auto location = Application::GetAppDataPath() / "instances" / m_directory_name;
        const auto instance_file = location / "instance.json";

        std::filesystem::create_directories(location / "mods");

        const nlohmann::json json { { "name", name }, { "game", m_game->GetId() } };

        std::ofstream(instance_file) << json.dump(4);
    }

    Instance* Instance::Load(const std::string& directory_name)
    {
        const auto location = Application::GetAppDataPath() / "instances" / directory_name;
        const auto instance_file = location / "instance.json";

        if (!std::filesystem::exists(instance_file))
        {
            SPDLOG_ERROR("instance.json does not exist at: {}", location.string());
            return nullptr;
        }

        const auto json = nlohmann::json::parse(std::ifstream(instance_file));
        const auto name = nlohmann::try_get<std::string>(json, "name");
        const auto game_id = nlohmann::try_get<std::string>(json, "game");

        if (!name.has_value() || !game_id.has_value())
        {
            SPDLOG_ERROR("Invalid structured json inside instance.json at: {}", location.string());
            return nullptr;
        }

        const auto game = Application::GetGameById(game_id.value());

        if (game == nullptr)
        {
            SPDLOG_ERROR("Game with id '{}' does not exist", game_id.value());
            return nullptr;
        }

        return new Instance(game, name.value(), directory_name);
    }
}