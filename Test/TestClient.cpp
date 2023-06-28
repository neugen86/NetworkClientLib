#include "TestClient.h"

#include <QDebug>
#include <QBuffer>

namespace
{
HttpRequest MakeRequest()
{
    HttpRequest request(HttpRequest::Get, new QBuffer);
    request.request.setUrl(QUrl("google.com"));
    request.redirectLimit = 1;

    request.onSuccess = [](QNetworkReply* reply, NetworkOutputDevice device)
    {
        Q_UNUSED(reply);
        qDebug() << "HttpRequest::onSuccess handled";

        return device->size();
    };

    request.onFail = [](QNetworkReply* reply)
    {
        Q_UNUSED(reply);
        qDebug() << "HttpRequest::onFail handled";
    };

    return request;
}
} // anonymous namespace


TestClient::TestClient(QObject* parent)
    : QObject(parent)
{
    m_manager.queue().setBatchSize(3);
}

NetworkTaskResult<qint64> TestClient::call()
{
    return NetworkTaskResult<qint64>(
        m_manager.execute(
            MakeRequest(),
            TaskQueue::ExecType::Queued
        )
    );
}

WeakNetworkTask TestClient::ping()
{
    return m_manager.repeat(
        MakeRequest(),
        TaskQueue::ExecType::Immediate
    );
}
