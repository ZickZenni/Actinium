#pragma once

#include <filesystem>
#include <utility>

namespace Actinium
{
    class TempFile
    {
    public:
        explicit TempFile(std::filesystem::path path)
            : m_path(std::move(path))
        {
        }

        ~TempFile()
        {
            if (!m_path.empty() && std::filesystem::exists(m_path))
            {
                std::filesystem::remove(m_path);
            }
        }

        TempFile(const TempFile &) = delete;
        TempFile &operator=(const TempFile &) = delete;

        /**
         * Retrieves the path of the temporary file.
         */
        [[nodiscard]] const std::filesystem::path &GetPath() const
        {
            return m_path;
        }

    private:
        std::filesystem::path m_path;
    };
}