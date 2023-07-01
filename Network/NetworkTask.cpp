#include "NetworkTask.h"

NetworkTask::NetworkTask(QObject* parent)
    : Task(parent)
    , m_upload(new DataProgress(this))
    , m_download(new DataProgress(this))
{
    Q_ASSERT(m_upload);
    Q_ASSERT(m_download);

    connect(this, &Task::rejected, this, [=]()
    {
        abortExecution(RejectedError, QLatin1String("Rejected"));
    });
}

NetworkTask::~NetworkTask()
{}

DataProgress* NetworkTask::upload() const
{
    return m_upload.data();
}

DataProgress* NetworkTask::download() const
{
    return m_download.data();
}

NetworkTask::DataDirection NetworkTask::dataDirection() const
{
    return m_dataDirection;
}

void NetworkTask::setDataDirection(DataDirection value)
{
    if (m_dataDirection != value)
    {
        m_dataDirection = value;
        emit dataDirectionChanged();
    }
}
