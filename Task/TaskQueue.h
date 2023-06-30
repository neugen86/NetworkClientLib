#pragma once

#include <QMap>
#include <QSet>
#include <QList>
#include <QMutex>
#include <QPointer>

#include "TaskDefines.h"

using TaskQPtr = QPointer<AbstractTask>;

class TaskQueue : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int capacity READ capacity WRITE setCapacity NOTIFY capacityChanged)
    Q_PROPERTY(int batchSize READ batchSize WRITE setBatchSize NOTIFY batchSizeChanged)

    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(int runnngCount READ runningCount NOTIFY runningCountChanged)

    Q_PROPERTY(bool suspended READ isSuspended NOTIFY suspendedChanged)
    Q_PROPERTY(bool empty READ isEmpty NOTIFY countChanged)

public:
    enum class ExecType
    {
        Immediate,
        Queued
    };
    Q_ENUM(ExecType)

    enum class SuspendType
    {
        Suspend,
        Sustain
    };
    Q_ENUM(SuspendType)

    explicit TaskQueue(QObject* parent = nullptr);
    ~TaskQueue();

    int capacity() const;
    void setCapacity(int value);

    int batchSize() const;
    void setBatchSize(int value);

    int count() const;
    int runningCount() const;

    bool isSuspended() const;
    bool isEmpty() const;

    Q_INVOKABLE bool execute(
        AbstractTask* task, ExecType execType);

    Q_INVOKABLE bool repeat(
        AbstractTask* task, ExecType execType,
        SuspendType suspedType = SuspendType::Sustain);

    Q_INVOKABLE bool suspend();
    Q_INVOKABLE bool resume();
    Q_INVOKABLE void clear();

private:
    bool add(AbstractTask* task);
    void processAdded(TaskId id, ExecType execType);
    void removeTask(TaskId id);
    void removeInvalidTasks();
    void executeNextQueuedBatch();
    void executeTask(TaskQPtr task);
    void OnTaskCompleted(TaskId id);

signals:
    void countChanged(QPrivateSignal = {});
    void capacityChanged(QPrivateSignal = {});
    void batchSizeChanged(QPrivateSignal = {});
    void runningCountChanged(QPrivateSignal = {});
    void suspendedChanged(QPrivateSignal = {});
    void dequeued(TaskId id, QPrivateSignal = {});
    void empty(QPrivateSignal = {});

private:
    int m_capacity = -1;
    int m_batchSize = 1;
    bool m_suspeded = false;

    QList<TaskId> m_queuedIds;
    QSet<TaskId> m_runningIds;
    QSet<TaskId> m_immediateIds;
    QSet<TaskId> m_repeatingIds;
    QSet<TaskId> m_sustainingIds;
    QMap<TaskId, TaskQPtr> m_tasks;

    mutable QRecursiveMutex m_mutex;

};
