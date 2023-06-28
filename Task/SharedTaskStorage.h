#pragma once

#include <QMap>
#include <QObject>

#include "TaskDefines.h"

class SharedTaskStorage : public QObject
{
    Q_OBJECT

public:
    explicit SharedTaskStorage(QObject* parent = nullptr);

    void add(const SharedTask& task);
    SharedTask get(TaskId id) const;
    bool remove(TaskId id);

private:
    QMap<TaskId, SharedTask> m_tasks;

};
