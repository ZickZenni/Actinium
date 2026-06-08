// ReSharper disable CppDFAMemoryLeak
#include "launch_instance_dialog.h"

#include "core/application.h"
#include "task/download_libraries_task.h"
#include "task/download_loader_task.h"
#include "util/lib/qt.h"

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
        , m_progress_bar(nullptr)
        , m_task_progress_bar(nullptr)
        , m_abort_button(nullptr)
        , m_worker_thread(new QThread(this))
        , m_worker(new Worker())
    {
        setObjectName("LaunchInstanceDialog");
        setWindowTitle("Please wait...");
        setModal(true);

        PrepareUI();

        std::vector<Task*> tasks;
        tasks.push_back(new DownloadLoaderTask(instance, m_worker));
        tasks.push_back(new DownloadLibrariesTask(instance, instance->GetGame()->libraries, m_worker));

        m_worker->SetTasks(tasks);
        m_worker->moveToThread(m_worker_thread);

        connect(m_worker, &Worker::ProgressChanged, this,
            [this](const int progress)
            {
                m_progress_bar->setValue(progress);
            });
        connect(m_worker, &Worker::TaskProgressChanged, this,
            [this](const int progress, const int total)
            {
                m_task_progress_bar->setMaximum(total);
                m_task_progress_bar->setValue(progress);
            });
        connect(m_worker, &Worker::Finished, this,
            [this]
            {
                GApp->LaunchGameWithLoader(m_instance);
                accept();
            });
        connect(m_worker, &Worker::Error, this,
            [this](const std::string& error)
            {
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
        m_worker->Abort();
        m_abort_button->setEnabled(false);
    }

    void LaunchInstanceDialog::PrepareUI()
    {
        m_progress_bar = new QProgressBar(this);
        m_progress_bar->setRange(0, 100);

        m_task_progress_bar = new QProgressBar(this);
        m_task_progress_bar->setRange(0, 100);

        m_abort_button = new QPushButton("Abort", this);
        connect(m_abort_button, &QPushButton::clicked, this, &LaunchInstanceDialog::Abort);

        auto* layout = new QVBoxLayout(this);
        layout->addWidget(new QLabel("Launching, please wait...", this));
        layout->addWidget(m_progress_bar);
        layout->addWidget(m_task_progress_bar);
        layout->addWidget(m_abort_button);
    }
}
// ReSharper restore CppDFAMemoryLeak