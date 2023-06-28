#pragma once

#include <QObject>
#include <QScopedPointer>

#include "Task/TaskQueue.h"
#include "NetworkDefines.h"

class SharedTaskStorage;

class NetworkTaskManager : public QObject
{
    Q_OBJECT

public:
    explicit NetworkTaskManager(QObject* parent = nullptr);
    ~NetworkTaskManager();

    TaskQueue& queue() const;

protected:
    WeakNetworkTask execute(const SharedNetworkTask& task,
                            TaskQueue::ExecType execType);

    WeakNetworkTask repeat(const SharedNetworkTask& task,
                           TaskQueue::ExecType execType,
                           TaskQueue::SuspendType suspendType
                           = TaskQueue::SuspendType::Suspend);

private:
    QScopedPointer<SharedTaskStorage> m_storage;
    QScopedPointer<TaskQueue> m_queue;

};
