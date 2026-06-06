#pragma once

#include "loader/loader_library.h"

namespace Actinium
{
    struct Game
    {
        std::string id;
        std::string name;
        std::string executable_name;
        std::vector<LoaderLibrary> libraries;
        std::optional<uint64_t> steam_app_id;
        std::vector<std::string> steam_start_parameters;
    };
}