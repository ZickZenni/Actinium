#include "instance.h"

#include <utility>

namespace Actinium
{
    Instance::Instance(Game* game, std::string name)
        : Instance(game, std::move(name), GenerateUUID())
    {
    }

    Instance::Instance(Game* game, std::string name, const UUIDv4::UUID& uuid)
        : name(std::move(name))
        , m_game(game)
        , m_uuid(uuid)
    {
    }

    UUIDv4::UUID Instance::GenerateUUID()
    {
        static UUIDv4::UUIDGenerator<std::mt19937_64> generator;

        return generator.getUUID();
    }
}