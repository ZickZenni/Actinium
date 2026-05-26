#include "path.h"

namespace Actinium
{
    const std::unordered_set<std::string> PathUtils::RESERVED
        = { "CON", "PRN", "AUX", "NUL", "COM1", "COM2", "COM3", "COM4", "COM5", "COM6", "COM7", "COM8", "COM9", "LPT1",
              "LPT2", "LPT3", "LPT4", "LPT5", "LPT6", "LPT7", "LPT8", "LPT9" };

    std::string PathUtils::SanitizeName(const std::filesystem::path& path)
    {
        auto clean_name = RemoveUnallowedChars(path.filename().string());
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

    std::string PathUtils::RemoveUnallowedChars(const std::string& value)
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
}