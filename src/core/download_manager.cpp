#include "download_manager.h"

#include "logger.h"
#include "util/fs/path.h"
#include "util/lib/qt.h"

#include <QPointer>
#include <QThreadPool>
#include <cpr/cpr.h>
#include <fstream>

namespace Actinium
{
    DownloadManager::DownloadManager(const uint8_t max_concurrent_downloads, QWidget* parent)
        : QWidget(parent)
        , m_max_concurrent_downloads(max_concurrent_downloads)
        , m_running_downloads(0)
        , m_running(true)
        , m_next_id(0)
    {
    }

    DownloadManager::~DownloadManager()
    {
        m_running.store(false);

        QWidget::~QWidget();
    }

    DownloadId DownloadManager::Queue(const std::string& url)
    {
        static const auto REDIRECT_OPTION = cpr::Redirect {true};

        QueuedDownload entry;
        entry.id = ++m_next_id;
        entry.url = url;

        m_queue.push_back(entry);

        if (m_running_downloads < m_max_concurrent_downloads)
        {
            const auto thread_id = ++m_running_downloads;

            QPointer self(this);
            QThreadPool::globalInstance()->start(
                [self, thread_id]
                {
                    Logger::Debug("core::download_manager", "Starting thread {}", thread_id);

                    while (self->m_running.load())
                    {
                        if (self == nullptr)
                        {
                            return;
                        }

                        std::unique_lock lock(self->m_queue_mutex);

                        if (self->m_queue.empty())
                        {
                            break;
                        }

                        const auto download = self->m_queue.front();
                        self->m_queue.pop_front();
                        lock.unlock();

                        /**
                         * We cannot use a `TempFile` as the file should not be deleted directly after downloading.
                         * The object that queued the download should handle the file accordingly.
                         */
                        const auto file_path = Path::CreateTempFilePath();
                        std::ofstream out(file_path, std::ios::binary);

                        const auto url = cpr::Url {download.url};
                        const auto response = cpr::Download(out, url, REDIRECT_OPTION);
                        out.close();

                        if (response.error || response.status_code < 200 || response.status_code >= 300)
                        {
                            Logger::Error("core::download_manager", "Failed to download file (url={}, status_code={}, error={})", url.str(), response.status_code, response.error.message);

                            std::error_code code;
                            std::filesystem::remove(file_path, code);

                            continue;
                        }

                        QT::Invoke(self,
                            [self, file_path, download]()
                            {
                                self->emit DownloadFinished(download.id, file_path);
                            });
                    }

                    Logger::Debug("core::download_manager", "Shutting down thread {}", thread_id);
                    --self->m_running_downloads;
                });
        }

        return entry.id;
    }
}