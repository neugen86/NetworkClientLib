#pragma once

#include <QObject>
#include <QPointer>

#include "HttpRequest.h"
#include "Network/AbstractNetworkTask.h"

class QNetworkAccessManager;

class HttpRequestTask : public AbstractNetworkTask
{
    Q_OBJECT

public:
    explicit HttpRequestTask(const HttpRequest& request,
                             QNetworkAccessManager* nam,
                             QObject* parent = nullptr);

    enum ErrorCode
    {
        Timeout = 2000,
        RedirectLimit,
        OutputDeviceOpen,
        OutputDeviceWrite,
    };
    Q_ENUM(ErrorCode);

private: // TaskHandler
    void executeImpl() override;

private: // NetworkTask
    void abortExecution(int errorCode) override;

private:
    void disconnectReply();

private:
    int m_redirectCount = 0;
    const HttpRequest c_request;
    QPointer<QNetworkReply> m_reply;
    QPointer<QNetworkAccessManager> m_nam;

};
