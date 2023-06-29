#include "AbstractTask.h"

#include <QDebug>

namespace
{
TaskId MakeTaskId()
{
    static TaskId nextId = 0;
    return nextId++;
}
} // anonymous namespace


void AbstractTaskHandler::execute()
{
    m_task.setStatus(AbstractTask::Running);
    executeImpl();
}

void AbstractTaskHandler::suspend()
{
    m_task.setStatus(AbstractTask::Suspended);
    suspendImpl();
}

void AbstractTaskHandler::abort()
{
    m_task.setStatus(AbstractTask::Aborted);
    abortImpl();
}

void AbstractTaskHandler::drop()
{
    m_task.setStatus(AbstractTask::Dropped);
    dropImpl();
}


AbstractTask::AbstractTask(QObject* parent)
    : QObject(parent)
    , AbstractTaskHandler(*this)
    , c_id(MakeTaskId())
{}

AbstractTask::~AbstractTask()
{
    qInfo() << name() << "destroyed";
}

TaskId AbstractTask::id() const
{
    return c_id;
}

QString AbstractTask::name() const
{
    return QString("[Task_%1]").arg(c_id);
}

AbstractTask::Status AbstractTask::status() const
{
    return m_status;
}

QVariant AbstractTask::result() const
{
    return m_result;
}

int AbstractTask::errorCode() const
{
    return m_errorCode;
}

bool AbstractTask::isCompleted() const
{
    return m_completed;
}

void AbstractTask::cancel()
{
    setStatus(Cancelled);
}

void AbstractTask::setResult(const QVariant& result)
{
    setStatus(Succeeded);

    qInfo() << name() << "result:" << result;

    m_result = result;
    emit resultChanged();
}

void AbstractTask::setFailed(int errorCode)
{
    setStatus(Failed);

    m_errorCode = errorCode;
    emit errorCodeChanged();
}

void AbstractTask::setStatus(Status value)
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
    case Queued:
        emit started();
        m_completed = false;
        m_errorCode = NoError;
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

    default: {}
    }

    if (m_completed && !wasCompleted)
    {
        emit completed();
    }
}
