#pragma once

#include <QObject>
#include <QPointer>

#include "HttpRequest.h"
#include "Network/NetworkTask.h"

class QNetworkAccessManager;

class HttpRequestTask : public NetworkTask
{
    Q_OBJECT

public:
    explicit HttpRequestTask(const HttpRequest& request,
                             QNetworkAccessManager* nam,
                             QObject* parent = nullptr);

private: // TaskHandler
    void executeImpl() override;

private: // NetworkTask
    void abortExecution() override;

private:
    void disconnectReply();

private:
    int m_redirectionsCount = 0;
    const HttpRequest c_request;
    QPointer<QNetworkReply> m_reply;
    QPointer<QNetworkAccessManager> m_nam;

};