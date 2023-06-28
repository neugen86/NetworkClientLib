#pragma once

#include <QMap>

#include "TaskDefines.h"

class SharedTaskStorage
{
public:
    void add(const SharedTask& task);
    SharedTask get(TaskId id) const;
    bool remove(TaskId id);

private:
    QMap<TaskId, SharedTask> m_tasks;

};
