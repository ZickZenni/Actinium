#include "icon_cache.h"

#include "core/logger.h"
#include "util/lib/qt.h"

#include <QCryptographicHash>
#include <QSaveFile>
#include <QThreadPool>
#include <cpr/cpr.h>

namespace Actinium
{
    IconCache::IconCache(const std::filesystem::path& cache_path)
        : m_cache_path(cache_path)
    {
        std::filesystem::create_directories(m_cache_path);
    }

    QIcon IconCache::GetIconFromUrl(const std::string& url)
    {

        if (m_icons.contains(url))
        {
            return m_icons.at(url);
        }

        const auto icon_path = GetIconPathFromUrl(url);

        if (std::filesystem::exists(icon_path))
        {
            QIcon icon(icon_path.c_str());

            if (!icon.isNull())
            {
                m_icons.insert({url, icon});

                return icon;
            }
        }

        DownloadIcon(url, icon_path);

        return GetPlaceholderIcon();
    }
    QIcon IconCache::GetPlaceholderIcon()
    {
        static const auto placeholder_icon = QIcon("./resources/instances/test.png");

        return placeholder_icon;
    }

    std::filesystem::path IconCache::GetIconPathFromUrl(const std::string& url) const
    {
        const auto hash = QCryptographicHash::hash(url, QCryptographicHash::Sha256).toHex();

        return m_cache_path / hash.toStdString();
    }

    void IconCache::DownloadIcon(const std::string& url, const std::filesystem::path& file_path)
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
                Logger::Info("ui::icon_cache::download_icon", "Begin download for icon (url={}, path={})", url,
                    file_path.string());

                const auto response = cpr::Get(cpr::Url {url}, cpr::Redirect {true}, cpr::Timeout {10000});

                Logger::Info("ui::icon_cache::download_icon", "Download finished (url={}, path={}, status_code={})",
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

                const QByteArray data(response.text.data(), static_cast<int>(response.text.size()));

                QPixmap pixmap;

                if (!pixmap.loadFromData(data))
                {
                    QT::Invoke(self,
                        [url, self]()
                        {
                            self->m_pending.erase(url);
                        });

                    return;
                }

                QSaveFile file(file_path);

                if (file.open(QIODevice::WriteOnly))
                {
                    file.write(data);
                    file.commit();

                    Logger::Debug(
                        "ui::icon_cache::download_icon", "Saved icon (url={}, path={})", url, file_path.string());
                }
                else
                {
                    Logger::Error("ui::icon_cache::download_icon", "Failed to save icon (url={}, path={})", url,
                        file_path.string());
                }

                QT::Invoke(self,
                    [url, self, pixmap]()
                    {
                        self->m_pending.erase(url);

                        QIcon icon(pixmap);
                        self->m_icons.insert({url, icon});
                    });
            });
    }
}