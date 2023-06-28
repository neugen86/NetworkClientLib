#pragma once

#include <QObject>
#include <QScopedPointer>

#include "Task/TaskQueue.h"
#include "NetworkDefines.h"

class TaskStorage;

class NetworkTaskManager : public QObject
{
    Q_OBJECT

protected:
    explicit NetworkTaskManager(QObject* parent = nullptr);
    ~NetworkTaskManager();

public:
    TaskQueue& queue() const;

protected:
    NetworkTaskRef execute(const NetworkTaskPtr& task,
                           TaskQueue::ExecType execType);

    NetworkTaskRef repeat(const NetworkTaskPtr& task,
                          TaskQueue::ExecType execType,
                          TaskQueue::SuspendType suspendType
                          = TaskQueue::SuspendType::Suspend);

private:
    QScopedPointer<TaskStorage> m_storage;
    QScopedPointer<TaskQueue> m_queue;

};
