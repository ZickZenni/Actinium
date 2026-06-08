#pragma once

#include <span>
#include <string>
#include <vector>

namespace Actinium::String
{
    /**
     * Joins a collection of strings into a single string, separated by the specified separator.
     */
    std::string Join(const std::span<const std::string> &values, const std::string_view &separator);

    std::vector<std::string> ToVector(int argc, char *argv[]);
}