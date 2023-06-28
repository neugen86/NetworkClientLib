#include "SharedTaskStorage.h"

#include "Task.h"

void SharedTaskStorage::add(const SharedTask& task)
{
    Q_ASSERT(task);
    m_tasks.insert(task->id(), task);
}

SharedTask SharedTaskStorage::get(TaskId id) const
{
    return m_tasks.value(id);
}

bool SharedTaskStorage::remove(TaskId id)
{
    return m_tasks.remove(id) > 0;
}
