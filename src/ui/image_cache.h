#pragma once

#include <QWidget>
#include <filesystem>
#include <unordered_map>
#include <unordered_set>

namespace Actinium
{
    class ImageCache : public QWidget
    {
        Q_OBJECT

    public:
        explicit ImageCache(const std::filesystem::path &cache_path);

        /**
         * Retrieves an icon from the url.
         */
        QImage GetImageFromUrl(const std::string &url);

    signals:
        void OnDownloadFinished(const std::string &url);

    private:
        std::filesystem::path m_cache_path;
        std::unordered_map<std::string, QImage> m_images;
        std::unordered_set<std::string> m_pending;

        /**
         * Retrieves the path where the icon is going to be stored.
         */
        std::filesystem::path GetImagePathFromUrl(const std::string &url) const;

        /**
         * Downloads the icon to the disk.
         */
        void DownloadImage(const std::string &url, const std::filesystem::path &file_path);

        /**
         * Loads the icon from the disk correctly (prevents only having 24x24 size)
         */
        static QImage LoadImage(const std::filesystem::path &path);
    };
}