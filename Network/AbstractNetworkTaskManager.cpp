#include "AbstractNetworkTaskManager.h"

#include "Task/TaskStorage.h"
#include "AbstractNetworkTask.h"

AbstractNetworkTaskManager::AbstractNetworkTaskManager(QObject* parent)
    : QObject(parent)
    , m_storage(new TaskStorage)
    , m_queue(new TaskQueue(this))
{
    Q_ASSERT(m_queue);
    Q_ASSERT(m_storage);

    connect(m_queue.data(), &TaskQueue::dequeued,
            this, [=](TaskId id)
    {
        m_storage->remove(id);
    });
}

AbstractNetworkTaskManager::~AbstractNetworkTaskManager()
{}

TaskQueue& AbstractNetworkTaskManager::queue() const
{
    return *m_queue.data();
}

NetworkTaskRef AbstractNetworkTaskManager::execute(
        const NetworkTaskPtr& task,
        TaskQueue::ExecType execType)
{
    if (m_queue->execute(task.data(), execType))
    {
        m_storage->add(task);
    }

    return task.toWeakRef();
}

NetworkTaskRef AbstractNetworkTaskManager::repeat(
        const NetworkTaskPtr& task,
        TaskQueue::ExecType execType,
        TaskQueue::SuspendType suspendType)
{
    if (m_queue->repeat(task.data(), execType, suspendType))
    {
        m_storage->add(task);
    }

    return task.toWeakRef();
}
