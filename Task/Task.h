#pragma once

#include <QObject>
#include <QString>
#include <QVariant>

#include "TaskDefines.h"

class TaskHandler
{
protected:
    explicit TaskHandler(Task& task)
        : m_task(task)
    {}
    virtual ~TaskHandler() = default;

public:
    const Task& task() const;

    void execute();
    void suspend();
    void abort();
    void drop();

private:
    virtual void executeImpl() = 0;
    virtual void suspendImpl() {};
    virtual void abortImpl() {};
    virtual void dropImpl() {};

private:
    Task& m_task;

};

class Task : public QObject
           , public TaskHandler
{
    Q_OBJECT

    friend class TaskQueue;
    friend class TaskHandler;

    Q_PROPERTY(TaskId id READ id CONSTANT)
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(QVariant result READ result NOTIFY resultChanged)
    Q_PROPERTY(bool completed READ isCompleted NOTIFY completed)
    Q_PROPERTY(int errorCode READ errorCode NOTIFY completed)
    Q_PROPERTY(QString message READ message NOTIFY completed)

protected:
    explicit Task(QObject* parent = nullptr);
    ~Task();

public:
    enum Status
    {
        New,
        Queued,
        Dropped,
        Aborted,
        Running,
        Suspended,
        Cancelled,
        Succeeded,
        Failed
    };
    Q_ENUM(Status)

    enum ErrorCode
    {
        NoError = 0,
        RejectedError = 1000,
    };
    Q_ENUM(ErrorCode)

    TaskId id() const;
    QString name() const;
    Status status() const;
    QVariant result() const;
    bool isCompleted() const;
    int errorCode() const;
    const QString& message() const;

    Q_INVOKABLE void cancel();

protected:
    void setResult(const QVariant& value);
    void setFailed(int code, const QString& message);

private:
    void setStatus(Status value);
    void updateCompleted();

signals:
    void statusChanged(QPrivateSignal = {});
    void resultChanged(QPrivateSignal = {});

    void started(QPrivateSignal = {});
    void suspended(QPrivateSignal = {});
    void rejected(QPrivateSignal = {});
    void succeeded(QPrivateSignal = {});
    void failed(QPrivateSignal = {});
    void completed(QPrivateSignal = {});

private:
    const TaskId c_id;
    bool m_completed = true;
    Status m_status = Status::New;
    QVariant m_result;
    int m_errorCode = NoError;
    QString m_message;

};
