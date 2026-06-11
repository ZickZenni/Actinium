#pragma once

#include <QWidget>
#include <filesystem>
#include <unordered_map>
#include <unordered_set>

namespace Actinium
{
    class IconCache : public QWidget
    {
    public:
        explicit IconCache(const std::filesystem::path &cache_path);

        /**
         * Retrieves an icon from the url.
         */
        QIcon GetIconFromUrl(const std::string &url);

        /**
         * Retrieves the placeholder icon.
         */
        static QIcon GetPlaceholderIcon();

    private:
        std::filesystem::path m_cache_path;
        std::unordered_map<std::string, QIcon> m_icons;
        std::unordered_set<std::string> m_pending;

        /**
         * Retrieves the path where the icon is going to be stored.
         */
        std::filesystem::path GetIconPathFromUrl(const std::string &url) const;

        /**
         * Downloads the icon to the disk.
         */
        void DownloadIcon(const std::string &url, const std::filesystem::path &file_path);
    };
}