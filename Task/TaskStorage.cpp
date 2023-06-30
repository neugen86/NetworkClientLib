#include "TaskStorage.h"

#include "Task.h"

void TaskStorage::add(const TaskPtr& task)
{
    Q_ASSERT(task);
    m_tasks.insert(task->id(), task);
}

TaskPtr TaskStorage::get(TaskId id) const
{
    return m_tasks.value(id);
}

bool TaskStorage::remove(TaskId id)
{
    return m_tasks.remove(id) > 0;
}

void TaskStorage::clear()
{
    m_tasks.clear();
}
