#include "worker.h"

namespace Actinium
{
    Worker::Worker(QObject* parent)
        : QObject(parent)
    {
    }

    Worker::~Worker()
    {
        for (const auto& task : m_tasks)
        {
            delete task;
        }
    }

    void Worker::Abort()
    {
        m_aborted.store(true);
    }

    void Worker::SetTasks(std::vector<Task*> tasks)
    {
        m_tasks = std::move(tasks);
    }

    void Worker::Run()
    {
        const auto per_task = static_cast<int>(100 / m_tasks.size());
        auto index = 0;

        for (const auto& task : m_tasks)
        {
            if (IsAborted())
            {
                return;
            }

            emit TaskProgressChanged(0, 100);

            task->Run();

            emit ProgressChanged((index + 1) * per_task);

            index++;
        }

        QThread::msleep(50);
        emit Finished();
    }
}