#include "TaskQueue.h"

#include "Task.h"

namespace
{
bool IsValid(TaskQPtr task)
{
    if (task)
    {
        switch (task->status())
        {
        case Task::Status::Dropped:
        case Task::Status::Aborted:
        case Task::Status::Cancelled:
            return false;

        default:
            return true;
        }
    }
    return false;
};
} // anonymous namespace


TaskQueue::TaskQueue(QObject* parent)
    : QObject(parent)
{
    connect(this, &TaskQueue::countChanged, this, [=]()
    {
        if (isEmpty())
        {
            emit empty();
        }
    });
}

TaskQueue::~TaskQueue()
{
    reset();
}

int TaskQueue::capacity() const
{
    QMutexLocker lock(&m_mutex);
    return m_capacity;
}

void TaskQueue::setCapacity(int value)
{
    QMutexLocker lock(&m_mutex);

    if (value < 1)
    {
        value = (value != 0) ? -1 : 1;
    }

    if (m_capacity != value)
    {
        m_capacity = value;
        emit capacityChanged();

        Q_ASSERT(m_capacity > 0 || m_capacity == -1);
    }
}

int TaskQueue::batchSize() const
{
    QMutexLocker lock(&m_mutex);
    return m_batchSize;
}

void TaskQueue::setBatchSize(int value)
{
    QMutexLocker lock(&m_mutex);

    value = qMax(1, value);

    if (m_batchSize != value)
    {
        m_batchSize = value;
        emit batchSizeChanged();

        Q_ASSERT(m_batchSize > 0);
    }
}

int TaskQueue::count() const
{
    QMutexLocker lock(&m_mutex);
    return m_tasks.count();
}
int TaskQueue::runningCount() const
{
    QMutexLocker lock(&m_mutex);

    return m_suspeded ? m_sustainingIds.count()
                      : m_runningIds.count();
}

bool TaskQueue::isSuspended() const
{
    QMutexLocker lock(&m_mutex);
    return m_suspeded;
}

bool TaskQueue::isEmpty() const
{
    QMutexLocker lock(&m_mutex);
    return m_tasks.isEmpty();
}

bool TaskQueue::suspend()
{
    QMutexLocker lock(&m_mutex);

    if (m_suspeded)
    {
        return false;
    }

    removeInvalidTasks();

    foreach (auto task, m_tasks)
    {
        if (!m_sustainingIds.contains(task->id()))
        {
            task->suspend();
        }
    }

    m_suspeded = true;
    emit suspendedChanged();

    emit runningCountChanged();

    return true;
}

bool TaskQueue::resume()
{
    QMutexLocker lock(&m_mutex);

    if (!m_suspeded)
    {
        return false;
    }

    removeInvalidTasks();

    foreach (auto task, m_tasks)
    {
        if (m_runningIds.contains(task->id()) &&
            task->status() == Task::Status::Suspended)
        {
            executeTask(task);
        }
    }

    m_suspeded = false;
    emit suspendedChanged();

    emit runningCountChanged();

    executeNextQueuedBatch();

    return true;
}

void TaskQueue::reset()
{
    QMutexLocker lock(&m_mutex);

    foreach (auto task, m_tasks)
    {
        if (task)
        {
            task->disconnect(this);
            task->abort();
        }
    }

    m_tasks.clear();
    m_queuedIds.clear();
    m_runningIds.clear();
    m_immediateIds.clear();
    m_repeatingIds.clear();
    m_sustainingIds.clear();

    emit runningCountChanged();
    emit countChanged();
}


bool TaskQueue::execute(Task* task, ExecType execType)
{
    QMutexLocker lock(&m_mutex);

    if (!add(task))
    {
        return false;
    }

    const TaskId id = task->id();

    switch (execType)
    {
    case ExecType::Immediate:
    {
        m_immediateIds.insert(id);

        m_runningIds.insert(id);
        emit runningCountChanged();

        if (!isSuspended())
        {
            executeTask(task);
        }

        break;
    }

    case ExecType::Queued:
    {
        m_queuedIds.append(id);
        executeNextQueuedBatch();
        break;
    }

    default:
        Q_ASSERT(false);
        return false;
    }

    return true;
}

bool TaskQueue::repeat(Task* task, ExecType execType, SuspendType suspedType)
{
    QMutexLocker lock(&m_mutex);

    if (execute(task, execType))
    {
        const TaskId id = task->id();

        m_repeatingIds.insert(id);

        if (suspedType == SuspendType::Sustain)
        {
            m_sustainingIds.insert(id);
        }

        return true;
    }

    return false;
}

bool TaskQueue::add(Task* task)
{
    QMutexLocker lock(&m_mutex);

    Q_ASSERT(task);
    Q_ASSERT(task->status() == Task::Status::New);

    if (m_capacity > 0 && count() >= m_capacity)
    {
        task->drop();
        return false;
    }

    if (m_tasks.contains(task->id()))
    {
        task->drop();
        return false;
    }

    connect(task, &Task::completed, this, [=]()
    {
        TaskQueue::OnTaskCompleted(task->id());
    });

    m_tasks.insert(task->id(), task);
    task->setStatus(Task::Status::Queued);

    if (isSuspended())
    {
        task->suspend();
    }

    emit countChanged();

    return true;
}

void TaskQueue::removeTask(TaskId id)
{
    if (auto task = m_tasks[id])
    {
        task->disconnect(this);
        emit dequeued(id);
    }

    m_tasks.remove(id);
    emit countChanged();
}

void TaskQueue::removeInvalidTasks()
{
    auto collect = [=](const auto& set)
    {
        QSet<TaskId> result;

        foreach (TaskId id, set)
        {
            if (!IsValid(m_tasks[id]))
            {
                m_tasks.remove(id);
                emit countChanged();
                emit dequeued(id);

                result.insert(id);
            }
        }

        return result;
    };

    auto removeFromSet = [](auto& from, const auto& set)
    {
        foreach (TaskId id, set)
        {
            from.remove(id);
        }
    };

    const auto runningToRemove = collect(m_runningIds);
    {
        if (!runningToRemove.isEmpty())
        {
            removeFromSet(m_runningIds, runningToRemove);
            emit runningCountChanged();
        }
    }

    const auto queuedToRemove = collect(m_queuedIds);
    {
        if (!queuedToRemove.isEmpty())
        {
            foreach (TaskId id, queuedToRemove)
            {
                m_queuedIds.removeAll(id);
            }
        }
    }

    const auto tasksToRemove = runningToRemove + queuedToRemove;
    {
        removeFromSet(m_immediateIds, tasksToRemove);
        removeFromSet(m_repeatingIds, tasksToRemove);
        removeFromSet(m_sustainingIds, tasksToRemove);
    }
}

void TaskQueue::executeNextQueuedBatch()
{
    QMutexLocker lock(&m_mutex);

    if (isSuspended())
    {
        return;
    }

    removeInvalidTasks();

    if (m_tasks.isEmpty())
    {
        return;
    }

    auto hasMore = [this]()
    {
        const int max = qMin(m_batchSize, m_queuedIds.count());
        const int rem = qMax(0, m_runningIds.size() - m_immediateIds.size());
        return qMax(0, max - rem);
    };

    while (hasMore())
    {
        const TaskId id = m_queuedIds.front();
        m_queuedIds.pop_front();

        m_runningIds.insert(id);
        executeTask(m_tasks[id]);
    }
}

void TaskQueue::executeTask(TaskQPtr task)
{
    QMetaObject::invokeMethod(this, [task]()
    {
        if (task)
        {
            task->execute();
        }
    },
    Qt::QueuedConnection);
}

void TaskQueue::OnTaskCompleted(TaskId id)
{
    QMutexLocker lock(&m_mutex);

    if (m_repeatingIds.contains(id))
    {
        auto task = m_tasks[id];

        if (IsValid(task))
        {
            executeTask(task);
            return;
        }
    }

    m_immediateIds.remove(id);

    m_runningIds.remove(id);
    emit runningCountChanged();

    removeTask(id);

    executeNextQueuedBatch();
}
