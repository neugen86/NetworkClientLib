#pragma once

#include <QObject>
#include <QString>
#include <QVariant>

#include "TaskDefines.h"

class AbstractTaskHandler
{
protected:
    explicit AbstractTaskHandler(AbstractTask& task)
        : m_task(task)
    {}
    virtual ~AbstractTaskHandler() = default;

public:
    const AbstractTask& task() const;

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
    AbstractTask& m_task;

};

class AbstractTask : public QObject
                   , public AbstractTaskHandler
{
    Q_OBJECT

    friend class TaskQueue;
    friend class AbstractTaskHandler;

    Q_PROPERTY(TaskId id READ id CONSTANT)
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(QVariant result READ result NOTIFY resultChanged)
    Q_PROPERTY(int errorCode READ errorCode NOTIFY errorCodeChanged)
    Q_PROPERTY(bool completed READ isCompleted NOTIFY completed)

protected:
    explicit AbstractTask(QObject* parent = nullptr);
    ~AbstractTask();

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
    int errorCode() const;
    bool isCompleted() const;

    Q_INVOKABLE void cancel();

protected:
    void setResult(const QVariant& value);
    void setFailed(int errorCode);

private:
    void setStatus(Status value);

signals:
    void statusChanged(QPrivateSignal = {});
    void resultChanged(QPrivateSignal = {});
    void errorCodeChanged(QPrivateSignal = {});

    void started(QPrivateSignal = {});
    void rejected(QPrivateSignal = {});
    void succeeded(QPrivateSignal = {});
    void failed(QPrivateSignal = {});
    void completed(QPrivateSignal = {});

private:
    const TaskId c_id;
    bool m_completed = true;
    Status m_status = Status::New;
    int m_errorCode = NoError;
    QVariant m_result;

};
