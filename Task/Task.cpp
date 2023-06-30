#include "Task.h"

#include <QDebug>

namespace
{
TaskId MakeTaskId()
{
    static TaskId nextId = 0;
    return nextId++;
}
} // anonymous namespace


void TaskHandler::execute()
{
    m_task.setStatus(Task::Running);
    executeImpl();
}

void TaskHandler::suspend()
{
    m_task.setStatus(Task::Suspended);
    suspendImpl();
}

void TaskHandler::abort()
{
    m_task.setStatus(Task::Aborted);
    abortImpl();
}

void TaskHandler::drop()
{
    m_task.setStatus(Task::Dropped);
    dropImpl();
}


Task::Task(QObject* parent)
    : QObject(parent)
    , TaskHandler(*this)
    , c_id(MakeTaskId())
{}

Task::~Task()
{
    qInfo() << name() << "destroyed";
}

TaskId Task::id() const
{
    return c_id;
}

QString Task::name() const
{
    return QString("[Task_%1]").arg(c_id);
}

Task::Status Task::status() const
{
    return m_status;
}

QVariant Task::result() const
{
    return m_result;
}

int Task::errorCode() const
{
    return m_errorCode;
}

bool Task::isCompleted() const
{
    return m_completed;
}

void Task::cancel()
{
    setStatus(Cancelled);
}

void Task::setResult(const QVariant& result)
{
    setStatus(Succeeded);

    qInfo() << name() << "result:" << result;

    m_result = result;
    emit resultChanged();
}

void Task::setFailed(int errorCode)
{
    setStatus(Failed);

    m_errorCode = errorCode;
    emit errorCodeChanged();
}

void Task::setStatus(Status value)
{
    if (m_status == value)
    {
        return;
    }

    qInfo() << name() << "status" << value;

    m_status = value;
    emit statusChanged();

    QMetaObject::invokeMethod(
        this, &Task::updateCompleted,
        Qt::QueuedConnection
    );
}

void Task::updateCompleted()
{
    const bool wasCompleted = m_completed;

    switch (m_status)
    {
    case New:
    case Queued:
    case Running:
    case Suspended:
        m_completed = false;
        m_errorCode = NoError;

        switch (m_status)
        {
        case Running:
            emit started();
            break;

        case Suspended:
            emit suspended();

        default: {}
        }

        break;

    case Cancelled:
    case Dropped:
    case Aborted:
        emit rejected();
        m_completed = true;
        break;

    case Succeeded:
        emit succeeded();
        m_completed = true;
        break;

    case Failed:
        emit failed();
        m_completed = true;
        break;
    }

    if (m_completed && !wasCompleted)
    {
        emit completed();
    }
}
