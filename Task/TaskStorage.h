#pragma once

#include <QMap>

#include "TaskDefines.h"

class TaskStorage
{
public:
    void add(const TaskPtr& task);
    TaskPtr get(TaskId id) const;
    bool remove(TaskId id);
    void clear();

private:
    QMap<TaskId, TaskPtr> m_tasks;

};
