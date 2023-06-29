#include "TestClient.h"

#include <QDebug>
#include <QBuffer>
#include <QCoreApplication>

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

NetworkTaskResult<QString> TestClient::callMicrosoft()
{
    HttpRequest request(HttpRequest::Get, new QBuffer);
    request.request.setUrl(QUrl("microsoft.com"));
    request.redirectLimit = 1;

    request.onSuccess = [](QNetworkReply* reply, NetworkOutputDevice device)
    {
        Q_UNUSED(reply);
        return QString("%1 bytes from microsoft received").arg(device->size());
    };

    request.onFail = [](QNetworkReply* reply)
    {
        Q_UNUSED(reply);
        qDebug() << "callMicrosoft failed";
    };

    return m_manager.execute<QString>(
        request, TaskQueue::ExecType::Queued
    );
}

NetworkTaskResult<bool> TestClient::pingGoogle()
{
    HttpRequest request(HttpRequest::Get, new QBuffer);
    request.request.setUrl(QUrl("google.com"));
    request.redirectLimit = 1;

    request.onSuccess = [](QNetworkReply* reply, NetworkOutputDevice device)
    {
        Q_UNUSED(reply);
        return device->size() > 0;
    };

    request.onFail = [](QNetworkReply* reply)
    {
        Q_UNUSED(reply);
        qDebug() << "pingGoogle failed";
    };

    return m_manager.repeat<bool>(
        request, TaskQueue::ExecType::Immediate
    );
}
