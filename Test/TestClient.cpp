#include "TestClient.h"

#include <QDebug>
#include <QBuffer>
#include <QCoreApplication>

namespace
{
HttpRequest MakeRequest(const QUrl& url)
{
    HttpRequest request(HttpRequest::Get, new QBuffer);
    request.request.setUrl(url);
    request.redirectLimit = 1;

    request.onSuccess = [](QNetworkReply* reply, NetworkOutputDevice device)
    {
        Q_UNUSED(reply);
        qDebug() << "HttpRequest::onSuccess";

        return device->size();
    };

    request.onFail = [](QNetworkReply* reply)
    {
        Q_UNUSED(reply);
        qDebug() << "HttpRequest::onFail";
    };

    return request;
}
} // anonymous namespace


TestClient::TestClient(QObject* parent)
    : QObject(parent)
{
    m_manager.queue().setBatchSize(3);

    connect(&m_manager.queue(), &TaskQueue::empty, this, []()
    {
        qDebug() << "TaskQueue is empty";
        qApp->exit();
    });
}

NetworkTaskResult<QVariant> TestClient::callQt()
{
    return m_manager.execute<>(
        MakeRequest(QUrl("qt.io")),
        TaskQueue::ExecType::Queued
    );
}

NetworkTaskResult<QVariant> TestClient::pingGoogle()
{
    return m_manager.repeat(
        MakeRequest(QUrl("google.com")),
        TaskQueue::ExecType::Immediate
    );
}
