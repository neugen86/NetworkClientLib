#include "NetworkTaskManager.h"

#include "Task/SharedTaskStorage.h"
#include "NetworkTask.h"

NetworkTaskManager::NetworkTaskManager(QObject* parent)
    : QObject(parent)
    , m_storage(new SharedTaskStorage)
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

NetworkTaskManager::~NetworkTaskManager()
{}

TaskQueue& NetworkTaskManager::queue() const
{
    return *m_queue.data();
}

WeakNetworkTask NetworkTaskManager::execute(const SharedNetworkTask& task,
                                            TaskQueue::ExecType execType)
{
    QMetaObject::invokeMethod(this, [=]()
    {
        if (m_queue->execute(task.data(), execType))
        {
            m_storage->add(task);
        }
    }, Qt::QueuedConnection);

    return task.toWeakRef();
}

WeakNetworkTask NetworkTaskManager::repeat(const SharedNetworkTask& task,
                                           TaskQueue::ExecType execType,
                                           TaskQueue::SuspendType suspendType)
{
    QMetaObject::invokeMethod(this, [=]()
    {
        if (m_queue->repeat(task.data(), execType, suspendType))
        {
            m_storage->add(task);
        }
    }, Qt::QueuedConnection);

    return task.toWeakRef();
}
