#pragma once

namespace Actinium
{
    class Worker;

    class Task
    {
    public:
        explicit Task(Worker *worker)
            : m_worker(worker)
        {
        }

        virtual ~Task() = default;

        virtual void Run() = 0;

    protected:
        Worker *m_worker;
    };
}