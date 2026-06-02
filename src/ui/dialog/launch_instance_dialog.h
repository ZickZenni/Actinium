#pragma once

#include "instance/instance.h"
#include "worker/worker.h"

#include <QDialog>
#include <QProgressBar>

namespace Actinium
{
    class LaunchInstanceDialog : public QDialog
    {
    public:
        explicit LaunchInstanceDialog(Instance *instance, QWidget *parent = nullptr);
        ~LaunchInstanceDialog() override;

    private:
        Instance *m_instance;

        QProgressBar *m_progress_bar;
        QProgressBar *m_task_progress_bar;
        QPushButton *m_abort_button;
        QThread *m_worker_thread;
        Worker *m_worker;

        void Abort() const;

        void PrepareUI();
    };
}