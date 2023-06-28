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
    m_task.setStatus(Task::Status::Running);
    executeImpl();
}

void TaskHandler::suspend()
{
    m_task.setStatus(Task::Status::Suspended);
    suspendImpl();
}

void TaskHandler::abort()
{
    m_task.setStatus(Task::Status::Aborted);
    abortImpl();
}

void TaskHandler::drop()
{
    m_task.setStatus(Task::Status::Dropped);
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

bool Task::isCompleted() const
{
    return m_completed;
}

void Task::cancel()
{
    setStatus(Status::Cancelled);
}

void Task::setResult(const QVariant& result)
{
    setStatus(Status::Succeeded);

    qInfo() << name() << "result:" << result;

    m_result = result;
    emit resultChanged();
}

void Task::setFailed()
{
    setStatus(Status::Failed);
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

    const bool wasCompleted = m_completed;

    switch (m_status)
    {
    case Status::New:
    case Status::Queued:
        emit started();
        m_completed = false;
        break;

    case Status::Cancelled:
        emit cancelled();
        m_completed = true;
        break;

    case Status::Succeeded:
        emit succeeded();
        m_completed = true;
        break;

    case Status::Dropped:
    case Status::Aborted:
    case Status::Failed:
        emit failed();
        m_completed = true;
        break;

    default: {}
    }

    if (m_completed && !wasCompleted)
    {
        emit completed();
    }
}
