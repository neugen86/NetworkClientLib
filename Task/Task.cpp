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
{
    qInfo() << name() << status();
}

Task::~Task()
{
    qInfo() << name() << "destroyed" << status();
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

int Task::errorCode() const
{
    return m_errorCode;
}

const QString& Task::message() const
{
    return m_message;
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

    setStatus(Succeeded);
}

void Task::setFailed(int code, const QString& message)
{
    m_errorCode = code;
    m_message = message;

    setStatus(Failed);
}

void Task::setStatus(Status value)
{
    if (m_status == value)
    {
        return;
    }

    qInfo() << name() << "status:" << value;

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
        m_message = QLatin1String("Ok");
        break;

    case Failed:
        emit failed();
        m_completed = true;
        break;
    }

    if (m_completed && !wasCompleted)
    {
        qInfo() << name() << "completed,"
                << "status:" << m_status
                << "message:" << m_message
                << "code:" << m_errorCode;

        if (!m_completed)
        {
            m_errorCode = NoError;
            m_message.clear();
        }

        emit completed();
    }
}
