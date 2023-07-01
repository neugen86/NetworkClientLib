#include "TestClient.h"

#include <QDebug>

TestClient::TestClient(QObject* parent)
    : QObject(parent)
{
    connect(&m_manager.queue(), &TaskQueue::countChanged,
            &m_manager, [this]()
    {
        if (m_manager.queue().isEmpty())
        {
            qDebug() << "=== [client] execution finished";
            emit executionFinished();
        }
        else
        {
            qDebug() << "=== [client] execution started";
        }
    });

    connect(&m_manager, &NetworkTaskManager::statusChanged,
            this, [this]()
    {
        const auto status = m_manager.status();

        if (m_ready = (status == NetworkTaskManager::Ready);
            m_ready)
        {
            qDebug() << "=== [client] ready";
        }

        emit readyChanged(m_ready);
    });

    m_manager.queue().setBatchSize(3);
    m_manager.start();
}

bool TestClient::isReady() const
{
    return m_ready;
}

bool TestClient::isExecuting() const
{
    return !m_manager.queue().isEmpty();
}

NetworkTaskResult<QString> TestClient::sendRequest(const QUrl& url)
{
    HttpRequest request(HttpRequest::Get);
    request.request.setUrl(url);
    request.redirectLimit = 1;

    request.onSuccess = [](QNetworkReply* reply, QIODevice* device)
    {
        qDebug() << "=== [sendRequest] succeeded";

        return QString("%1 bytes from %2 received")
            .arg(device ? device->size() : 0)
            .arg(reply->url().toString());
    };

    request.onFail = [](QNetworkReply* reply)
    {
        qDebug() << QString("=== [sendRequest] request to %1 failed")
                        .arg(reply->url().toString());
    };

    return m_manager.execute<QString>(
        request, TaskQueue::ExecType::Queued
    );
}

NetworkTaskResult<bool> TestClient::pingServer(const QUrl& url)
{
    HttpRequest request(HttpRequest::Get);
    request.request.setUrl(url);
    request.redirectLimit = 1;

    request.onSuccess = [](QNetworkReply* reply, QIODevice* device)
    {
        Q_UNUSED(reply);
        qDebug() << "=== [ping] succeeded";

        return device && device->size() > 0;
    };

    request.onFail = [](QNetworkReply* reply)
    {
        qDebug() << QString("=== [ping] ping %1 failed")
                        .arg(reply->url().toString());
    };

    return m_manager.repeat<bool>(
        request, TaskQueue::ExecType::Immediate
    );
}
