#include "TestClient.h"

#include <QDebug>
#include <QBuffer>

TestClient::TestClient(QObject* parent)
    : QObject(parent)
{
    connect(&m_manager.queue(), &TaskQueue::empty,
            this, &TestClient::finished);

    connect(&m_manager, &AbstractNetworkTaskManager::statusChanged,
            this, [this]()
    {
        const auto status = m_manager.status();

        const bool isReady = (status == AbstractNetworkTaskManager::Ready);
        if (isReady)
        {
            qDebug() << "[client] ready";
        }

        m_ready = isReady;
        emit readyChanged();

        if (status == AbstractNetworkTaskManager::Disconnected)
        {
            qDebug() << "[client] finished";
            emit finished();
        }
    });

//    m_manager.queue().setBatchSize(3);
    m_manager.start();
}

NetworkTaskResult<QString> TestClient::sendRequest(const QUrl& url)
{
    HttpRequest request(HttpRequest::Get, new QBuffer);
    request.request.setUrl(url);
    request.redirectLimit = 1;

    request.onSuccess = [](QNetworkReply* reply, NetworkOutputDevice device)
    {
        return QString("%1 bytes from %2 received")
            .arg(device->size())
            .arg(reply->url().toString());
    };

    request.onFail = [](QNetworkReply* reply)
    {
        qDebug() << QString("Request to %1 failed")
                        .arg(reply->url().toString());
    };

    return m_manager.execute<QString>(
        request, TaskQueue::ExecType::Queued
    );
}

NetworkTaskResult<bool> TestClient::pingServer(const QUrl& url)
{
    HttpRequest request(HttpRequest::Get, new QBuffer);
    request.request.setUrl(url);
    request.redirectLimit = 1;

    request.onSuccess = [](QNetworkReply* reply, NetworkOutputDevice device)
    {
        Q_UNUSED(reply);
        return device->size() > 0;
    };

    request.onFail = [](QNetworkReply* reply)
    {
        qDebug() << QString("Ping %1 failed")
                        .arg(reply->url().toString());
    };

    return m_manager.repeat<bool>(
        request, TaskQueue::ExecType::Immediate
    );
}
