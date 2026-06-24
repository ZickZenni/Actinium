#pragma once

#include <QWidget>
#include <cstdint>
#include <deque>
#include <filesystem>
#include <string>

namespace Actinium
{
    using DownloadId = uint32_t;

    class DownloadManager : public QWidget
    {
        Q_OBJECT

    public:
        explicit DownloadManager(uint8_t max_concurrent_downloads, QWidget *parent = nullptr);
        ~DownloadManager() override;

        DownloadId Queue(const std::string &url);

    signals:
        void DownloadFinished(DownloadId id, const std::filesystem::path &file_path);

    private:
        struct QueuedDownload
        {
            DownloadId id;
            std::string url;
        };

        uint8_t m_max_concurrent_downloads;
        std::atomic<uint8_t> m_running_downloads;
        std::atomic<bool> m_running;
        DownloadId m_next_id;

        std::deque<QueuedDownload> m_queue;
        std::mutex m_queue_mutex;
    };
}