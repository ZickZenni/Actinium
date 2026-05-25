#include "instance.h"

#include "core/application.h"
#include "util/json.h"

#include <fstream>
#include <nlohmann/json.hpp>
#include <utility>

namespace Actinium
{
    Instance::Instance(const Game* game, std::string name)
        : Instance(game, std::move(name), GenerateUUID())
    {
    }

    Instance::Instance(const Game* game, std::string name, const UUIDv4::UUID& uuid)
        : name(std::move(name))
        , m_game(game)
        , m_uuid(uuid)
    {
    }

    void Instance::Save() const
    {
        const auto location = Application::GetAppDataPath() / "instances" / m_uuid.str();
        const auto instance_file = location / "instance.json";

        std::filesystem::create_directories(location / "mods");

        const nlohmann::json json { { "name", name }, { "game", m_game->GetId() } };

        std::ofstream(instance_file) << json.dump(4);
    }

    UUIDv4::UUID Instance::GenerateUUID()
    {
        static UUIDv4::UUIDGenerator<std::mt19937_64> generator;

        return generator.getUUID();
    }

    Instance* Instance::Load(const std::string& directory_name)
    {
        const auto location = Application::GetAppDataPath() / "instances" / directory_name;
        const auto instance_file = location / "instance.json";

        if (!std::filesystem::exists(instance_file))
        {
            std::cout << "[Instance::Load] Instance file does not exist" << std::endl;
            return nullptr;
        }

        const auto json = nlohmann::json::parse(std::ifstream(instance_file));
        const auto name = nlohmann::try_get<std::string>(json, "name");
        const auto game_id = nlohmann::try_get<std::string>(json, "game");

        if (!name.has_value() || !game_id.has_value())
        {
            std::cout << "[Instance::Load] Invalid instance file" << std::endl;
            return nullptr;
        }

        const auto game = Application::GetGameById(game_id.value());

        if (game == GAMES.end())
        {
            std::cout << "[Instance::Load] Invalid game id" << std::endl;
            return nullptr;
        }

        return new Instance(game, name.value());
    }
}