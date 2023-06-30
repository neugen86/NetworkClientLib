#include "NetworkTaskManager.h"

#include "Task/TaskStorage.h"
#include "NetworkTask.h"

NetworkTaskManager::NetworkTaskManager(QObject* parent)
    : QObject(parent)
    , m_queue(new TaskQueue(this))
    , m_storage(new TaskStorage)
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
{
    stop();
}

bool NetworkTaskManager::start()
{
    m_status = Connecting;

    QMetaObject::invokeMethod(this, [this]()
    {
        m_status = Ready;
        emit statusChanged();
    },
    Qt::QueuedConnection);

    return true;
}

void NetworkTaskManager::stop()
{
    m_queue->clear();
    m_storage->clear();

    m_status = Disconnected;
    emit statusChanged();
}

NetworkTaskManager::Status NetworkTaskManager::status() const
{
    return m_status;
}

TaskQueue& NetworkTaskManager::queue() const
{
    return *m_queue.data();
}

NetworkTaskRef NetworkTaskManager::execute(
        const NetworkTaskPtr& task,
        TaskQueue::ExecType execType)
{
    if (m_queue->execute(task.data(), execType))
    {
        m_storage->add(task);
    }

    return task.toWeakRef();
}

NetworkTaskRef NetworkTaskManager::repeat(
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
