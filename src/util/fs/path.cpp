#include "path.h"

#include <random>

namespace Actinium::Path
{
    std::string SanitizeName(const std::string& name)
    {
        auto clean_name = RemoveUnallowedChars(name);
        auto upper = clean_name;

        std::transform(upper.begin(), upper.end(), upper.begin(),
            [](const unsigned char c)
            {
                return static_cast<char>(std::toupper(c));
            });

        if (RESERVED.contains(upper))
        {
            return "_" + clean_name;
        }

        return clean_name;
    }

    std::string SanitizeName(const std::filesystem::path& path)
    {
        return SanitizeName(path.filename().string());
    }

    std::string RemoveUnallowedChars(const std::string& value)
    {
        std::string result;
        result.reserve(value.size());

        for (const auto ch : value)
        {
            switch (ch)
            {
                case '<':
                case '>':
                case ':':
                case '"':
                case '/':
                case '\\':
                case '|':
                case '?':
                case '*':
                    break;
                default:
                    if (ch >= 32)
                    {
                        result.push_back(ch);
                    }
                    break;
            }
        }

        /**
         * Remove trailing spaces and periods.
         */
        while (!result.empty() && (result.back() == ' ' || result.back() == '.'))
        {
            result.pop_back();
        }

        return result;
    }

    std::filesystem::path CreateTempFilePath()
    {
        static std::random_device random_device;
        static std::mt19937 generator(random_device());
        static std::uniform_int_distribution<uint64_t> distribution;

        const auto temp_dir = std::filesystem::temp_directory_path();

        auto temp_file_path = temp_dir / std::to_string(distribution(generator));

        while (std::filesystem::exists(temp_file_path))
        {
            temp_file_path = temp_dir / std::to_string(distribution(generator));
        }

        return temp_file_path;
    }
}