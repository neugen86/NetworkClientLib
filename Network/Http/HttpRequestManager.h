#pragma once

#include <QObject>
#include <QPointer>

#include <QNetworkProxy>

#include "HttpRequest.h"
#include "Network/AbstractNetworkTaskManager.h"
#include "Network/NetworkTaskResult.h"

class NetworkTaskQueue;
class QNetworkAccessManager;

class HttpRequestManager : public AbstractNetworkTaskManager
{
    Q_OBJECT

public:
    explicit HttpRequestManager(QObject* parent = nullptr);
    ~HttpRequestManager();

    void setProxy(const QNetworkProxy& proxy);

    template <typename T = QVariant>
    NetworkTaskResult<T> execute(const HttpRequest& request,
                                 TaskQueue::ExecType execType)
    {
        return NetworkTaskResult<T>(
            AbstractNetworkTaskManager::execute(
                makeTask(request), execType
            )
        );
    }

    template <typename T = QVariant>
    NetworkTaskResult<T> repeat(const HttpRequest& request,
                                TaskQueue::ExecType execType,
                                TaskQueue::SuspendType suspendType
                                  = TaskQueue::SuspendType::Suspend)
    {
        return NetworkTaskResult<T>(
            AbstractNetworkTaskManager::repeat(
                makeTask(request), execType, suspendType
            )
        );
    }

private:
    NetworkTaskPtr makeTask(const HttpRequest& request);

private:
    QScopedPointer<QNetworkAccessManager> m_nam;

};
