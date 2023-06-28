#pragma once

#include <QObject>
#include <QPointer>

#include <QNetworkProxy>

#include "HttpRequest.h"
#include "Network/NetworkTaskManager.h"

class NetworkTaskQueue;
class QNetworkAccessManager;

class HttpRequestManager : public NetworkTaskManager
{
    Q_OBJECT

public:
    explicit HttpRequestManager(QObject* parent = nullptr);
    ~HttpRequestManager();

    void setProxy(const QNetworkProxy& proxy);

    WeakNetworkTask execute(const HttpRequest& request,
                            TaskQueue::ExecType execType);

    WeakNetworkTask repeat(const HttpRequest& request,
                           TaskQueue::ExecType execType,
                           TaskQueue::SuspendType suspendType
                           = TaskQueue::SuspendType::Suspend);

private:
    SharedNetworkTask makeTask(const HttpRequest& request);

private:
    QScopedPointer<QNetworkAccessManager> m_nam;

};
