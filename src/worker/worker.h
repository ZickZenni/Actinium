#pragma once

#include "task.h"

#include <QThread>

namespace Actinium
{
    class Worker : public QObject
    {
        Q_OBJECT

    public:
        explicit Worker(QObject *parent = nullptr);
        ~Worker() override;

        void Abort();

        void SetTasks(std::vector<Task *> tasks);

        [[nodiscard]] bool IsAborted() const
        {
            return m_aborted.load();
        }

    public slots:
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