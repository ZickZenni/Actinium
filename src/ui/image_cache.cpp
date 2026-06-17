#include "image_cache.h"

#include "core/logger.h"
#include "util/fs/path.h"
#include "util/fs/temp_file.h"
#include "util/lib/qt.h"

#include <QCryptographicHash>
#include <QSaveFile>
#include <QThreadPool>
#include <cpr/cpr.h>

#ifdef WIN32
#ifdef LoadImage
#undef LoadImage
#endif
#endif

namespace Actinium
{
    ImageCache::ImageCache(const std::filesystem::path& cache_path)
        : m_cache_path(cache_path)
    {
        std::filesystem::create_directories(m_cache_path);
    }

    QImage ImageCache::GetImageFromUrl(const std::string& url)
    {
        if (m_images.contains(url))
        {
            return m_images.at(url);
        }

        const auto image_path = GetImagePathFromUrl(url);

        if (std::filesystem::exists(image_path))
        {
            const auto image = LoadImage(image_path);

            if (!image.isNull())
            {
                m_images.insert({url, image});

                return image;
            }
        }

        DownloadImage(url, image_path);

        return {};
    }

    std::filesystem::path ImageCache::GetImagePathFromUrl(const std::string& url) const
    {
        const auto hash = QCryptographicHash::hash(url, QCryptographicHash::Sha256).toHex();

        return m_cache_path / hash.toStdString();
    }

    void ImageCache::DownloadImage(const std::string& url, const std::filesystem::path& file_path)
    {
        if (m_pending.contains(url))
        {
            return;
        }

        m_pending.insert(url);

        QPointer self(this);
        QThreadPool::globalInstance()->start(
            [self, url, file_path]
            {
                const TempFile temp_file(Path::CreateTempFilePath());
                std::ofstream out(temp_file.GetPath(), std::ios::binary);

                if (!out)
                {
                    QT::Invoke(self,
                        [url, self]()
                        {
                            self->m_pending.erase(url);
                        });

                    return;
                }

                Logger::Info("ui::image_cache::download_image", "Begin download for image (url={}, path={})", url,
                    file_path.string());

                const auto response = cpr::Download(out, cpr::Url {url}, cpr::Redirect {true}, cpr::Timeout {10000});
                out.close();

                Logger::Info("ui::image_cache::download_image", "Download finished (url={}, path={}, status_code={})",
                    url, file_path.string(), response.status_code);

                if (self == nullptr)
                {
                    return;
                }

                if (response.error)
                {
                    QT::Invoke(self,
                        [url, self]()
                        {
                            self->m_pending.erase(url);
                        });

                    return;
                }

                if (response.status_code < 200 || response.status_code >= 300)
                {
                    QT::Invoke(self,
                        [url, self]()
                        {
                            self->m_pending.erase(url);
                        });

                    return;
                }

                std::error_code copy_error_code;
                std::filesystem::copy_file(temp_file.GetPath(), file_path, copy_error_code);

                if (copy_error_code)
                {
                    Logger::Error("ui::image_cache::download_image", "Failed to save image (url={}, path={}, error={})",
                        url, file_path.string(), copy_error_code.message());

                    return;
                }

                Logger::Debug(
                    "ui::image_cache::download_image", "Saved image (url={}, path={})", url, file_path.string());

                QT::Invoke(self,
                    [url, self, file_path]()
                    {
                        const auto image = LoadImage(file_path);

                        if (image.isNull())
                        {
                            self->m_pending.erase(url);

                            Logger::Error("ui::image_cache::download_image", "Failed to load image (url={}, path={})",
                                url, file_path.string());
                            return;
                        }

                        self->m_images.insert({url, image});
                        self->m_pending.erase(url);
                        self->emit OnDownloadFinished(url);
                    });
            });
    }

    QImage ImageCache::LoadImage(const std::filesystem::path& path)
    {
        return QImage(path.string().c_str());
    }
}