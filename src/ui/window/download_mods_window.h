#pragma once

#include "base_window.h"
#include "instance/instance.h"
#include "ui/model/provider/provider_mod_model.h"

#include <QComboBox>
#include <QListView>
#include <QTextBrowser>

namespace Actinium
{
    class DownloadModsWindow : public BaseWindow
    {
        Q_OBJECT

    public:
        explicit DownloadModsWindow(Instance *instance, QWidget *parent = nullptr);
        ~DownloadModsWindow() override;

    private:
        Instance *m_instance;
        ProviderModModel *m_model;
        QListView *m_view;
        QTextBrowser *m_mod_description;
        QComboBox* m_mod_file_selector;
        Provider* m_current_provider;
        uint16_t m_current_page;
        bool m_waiting_for_response;
        bool m_end_of_list;
        bool m_ready_to_scroll_search;

        void OnSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

        void OnScrollBarChanged(int value);

        void Search();

        static void OnSearchResponse(DownloadModsWindow *self, const Provider::SearchResponse &response);
    };
}