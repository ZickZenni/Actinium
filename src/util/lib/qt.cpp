#include "qt.h"

#include <QApplication>
#include <QPainter>

namespace Actinium::QT
{
    QSize ViewItemTextSize(const QStyleOptionViewItem& option)
    {
        const auto style = GetCorrectStyle(option);

        QTextOption text_option;
        text_option.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);

        QTextLayout text_layout;
        text_layout.setTextOption(text_option);
        text_layout.setFont(option.font);
        text_layout.setText(option.text);

        const auto text_margin = style->pixelMetric(QStyle::PM_FocusFrameHMargin, &option, option.widget) + 1;
        const auto bounds = QRect(0, 0, 100 - 2 * text_margin, 600);

        qreal height = 0, used_width = 0;
        ViewItemTextLayout(text_layout, bounds.width(), height, used_width);

        const auto size = QSize(qCeil(used_width), qCeil(height));
        return {size.width() + 2 * text_margin, size.height()};
    }

    void ViewItemTextLayout(QTextLayout& layout, const int lineWidth, qreal& height, qreal& used_width)
    {
        height = 0;
        used_width = 0;
        layout.beginLayout();

        while (true)
        {
            auto line = layout.createLine();

            if (!line.isValid())
            {
                break;
            }

            if (line.textLength() == 0)
            {
                break;
            }

            line.setLineWidth(lineWidth);
            line.setPosition(QPointF(0, height));

            height += line.height();
            used_width = qMax(used_width, line.naturalTextWidth());
        }

        layout.endLayout();
    }

    QStyle* GetCorrectStyle(const QStyleOptionViewItem& option)
    {
        return option.widget ? option.widget->style() : QApplication::style();
    }

    QUrl CreateFileUrl(const std::filesystem::path& path)
    {
        return QUrl::fromLocalFile(strq(path.string()));
    }

    std::vector<std::string> ToStdVector(const QStringList& list)
    {
        std::vector<std::string> result;
        result.reserve(list.size());

        for (const auto& str : list)
        {
            result.emplace_back(str.toStdString());
        }

        return result;
    }

    void PaintIconCoveredInArea(QPainter* painter, const QIcon& icon, const QRect& area)
    {
        const auto dpr = painter->device()->devicePixelRatioF();
        const auto target_size = area.size() * dpr;

        auto pixmap = icon.pixmap(target_size);
        pixmap.setDevicePixelRatio(dpr);

        const auto scaled = pixmap.scaled(area.size() * dpr, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        const auto diff_width = scaled.width() - target_size.width();
        const auto diff_height = scaled.height() - target_size.height();
        const auto source_rect = QRect(QPoint(diff_width / 2, diff_height / 2), target_size);

        painter->drawPixmap(area, scaled, source_rect);
    }
}