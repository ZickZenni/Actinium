// ReSharper disable CppDFAMemoryLeak
#include "launch_instance_dialog.h"

#include "util/github.h"
#include "util/qt.h"
#include "worker/tasks/download_loader_task.h"

#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QThread>
#include <QVBoxLayout>
#include <spdlog/spdlog.h>

namespace Actinium
{
    LaunchInstanceDialog::LaunchInstanceDialog(Instance* instance, QWidget* parent)
        : QDialog(parent)
        , m_instance(instance)
    {
        setObjectName("LaunchInstanceDialog");
        setWindowTitle("Please wait...");
        setModal(true);

        auto* label = new QLabel("Launching, please wait...", this);

        m_progress_bar = new QProgressBar(this);
        m_progress_bar->setRange(0, 100);

        m_task_progress_bar = new QProgressBar(this);
        m_task_progress_bar->setRange(0, 100);

        m_abort_button = new QPushButton("Abort", this);
        connect(m_abort_button, &QPushButton::clicked, this, &LaunchInstanceDialog::Abort);

        auto* layout = new QVBoxLayout(this);
        layout->addWidget(label);
        layout->addWidget(m_progress_bar);
        layout->addWidget(m_task_progress_bar);
        layout->addWidget(m_abort_button);

        m_worker_thread = new QThread(this);
        m_worker = new Worker();
        m_worker->SetTasks({ new DownloadLoaderTask(instance->GetAbsolutePath(), m_worker) });
        m_worker->moveToThread(m_worker_thread);

        connect(m_worker, &Worker::ProgressChanged, this,
            [this](int progress)
            {
                m_progress_bar->setValue(progress);
            });
        connect(m_worker, &Worker::TaskProgressChanged, this,
            [this](int progress, int total)
            {
                m_task_progress_bar->setMaximum(total);
                m_task_progress_bar->setValue(progress);
            });
        connect(m_worker, &Worker::Finished, this,
            [this]
            {
                accept();
            });
        connect(m_worker, &Worker::Error, this,
            [this](const std::string& error)
            {
                SPDLOG_ERROR("Error: {}", error);
                QMessageBox::critical(nullptr, "Error", strq(error), QMessageBox::Close);
                reject();
            });
        connect(m_worker, &Worker::Aborted, this,
            [this]
            {
                reject();
            });

        connect(m_worker, &Worker::Finished, m_worker_thread, &QThread::quit);
        connect(m_worker, &Worker::Error, m_worker_thread, &QThread::quit);
        connect(m_worker, &Worker::Aborted, m_worker_thread, &QThread::quit);

        connect(m_worker_thread, &QThread::finished, m_worker, &QObject::deleteLater);
        connect(m_worker_thread, &QThread::started, m_worker, &Worker::Run);

        m_worker_thread->start();
    }

    LaunchInstanceDialog::~LaunchInstanceDialog()
    {
        if (m_worker_thread != nullptr)
        {
            m_worker_thread->quit();
            m_worker_thread->wait();
        }

        delete m_worker_thread;
    }

    void LaunchInstanceDialog::Abort() const
    {
        SPDLOG_INFO("Aborting launch...");

        m_worker->Abort();
        m_abort_button->setEnabled(false);
    }
}
// ReSharper restore CppDFAMemoryLeak