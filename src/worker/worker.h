#pragma once

#include "task/task.h"

#include <QThread>

namespace Actinium
{
    class Worker : public QObject
    {
        Q_OBJECT

    public:
        explicit Worker(QObject *parent = nullptr);
        ~Worker() override;

        /**
         * Aborts the current task and all other queued tasks.
         */
        void Abort();

        /**
         * Sets the list of tasks to be executed.
         */
        void SetTasks(std::vector<Task *> tasks);

        /**
         * Checks if the worker is set to abort all operations.
         */
        [[nodiscard]] bool IsAborted() const
        {
            return m_aborted.load();
        }

    public slots:
        /**
         * Executes all queued tasks.
         */
        void Run();

    signals:
        void ProgressChanged(int value);
        void TaskProgressChanged(int value, int total);
        void Finished();
        void Error(const std::string &message);
        void Aborted();

    private:
        std::atomic_bool m_aborted;
        std::vector<Task *> m_tasks;
    };
}