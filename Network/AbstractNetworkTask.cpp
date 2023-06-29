#include "AbstractNetworkTask.h"

AbstractNetworkTask::AbstractNetworkTask(QObject *parent)
    : AbstractTask(parent)
    , m_upload(new DataProgress(this))
    , m_download(new DataProgress(this))
{
    Q_ASSERT(m_upload);
    Q_ASSERT(m_download);

    connect(this, &AbstractTask::rejected, this, [=]()
    {
        abortExecution(RejectedError);
    });
}

AbstractNetworkTask::~AbstractNetworkTask()
{}

DataProgress* AbstractNetworkTask::upload() const
{
    return m_upload.data();
}

DataProgress* AbstractNetworkTask::download() const
{
    return m_download.data();
}

AbstractNetworkTask::DataDirection AbstractNetworkTask::dataDirection() const
{
    return m_dataDirection;
}

void AbstractNetworkTask::setDataDirection(DataDirection value)
{
    if (m_dataDirection != value)
    {
        m_dataDirection = value;
        emit dataDirectionChanged();
    }
}
