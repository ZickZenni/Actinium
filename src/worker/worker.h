#pragma once

#include "task.h"

#include <QThread>

namespace Actinium
{
    class Worker : public QObject
    {
        Q_OBJECT

    public:
        explicit Worker(QObject *parent = nullptr)
            : QObject(parent)
        {
        }

        void SetTasks(std::vector<Task *> tasks)
        {
            m_tasks = std::move(tasks);
        }

        void Abort()
        {
            m_aborted.store(true);
        }

        [[nodiscard]] bool IsAborted() const
        {
            return m_aborted.load();
        }

    public slots:
        void Run()
        {
            for (const auto &task : m_tasks)
            {
                task->Run();
            }

            QThread::msleep(50);
            emit Finished();
        }

    signals:
        void ProgressChanged(int value);
        void Finished();
        void Error(const std::string &message);
        void Aborted();

    private:
        std::atomic_bool m_aborted;
        std::vector<Task *> m_tasks;
    };
}