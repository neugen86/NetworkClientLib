#pragma once

#include <QObject>
#include <QScopedPointer>

#include "Task/TaskQueue.h"
#include "NetworkDefines.h"

class TaskStorage;

class NetworkTaskManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(Status status READ status NOTIFY statusChanged)

protected:
    explicit NetworkTaskManager(QObject* parent = nullptr);
    ~NetworkTaskManager();

public:
    enum Status
    {
        Ready,
        Connecting,
        Disconnected
    };
    Q_ENUM(Status);

    bool start();
    void stop();

    Status status() const;
    TaskQueue& queue() const;

protected:
    NetworkTaskRef execute(const NetworkTaskPtr& task,
                           TaskQueue::ExecType execType);

    NetworkTaskRef repeat(const NetworkTaskPtr& task,
                          TaskQueue::ExecType execType,
                          TaskQueue::SuspendType suspendType
                          = TaskQueue::SuspendType::Suspend);

signals:
    void statusChanged(QPrivateSignal = {});

private:
    Status m_status = Disconnected;
    QScopedPointer<TaskQueue> m_queue;
    QScopedPointer<TaskStorage> m_storage;

};
