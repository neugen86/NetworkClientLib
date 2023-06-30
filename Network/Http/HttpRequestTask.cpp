#include "HttpRequestTask.h"

#include <QDebug>
#include <QTimer>
#include <QNetworkAccessManager>

HttpRequestTask::HttpRequestTask(const HttpRequest& request,
                                 QNetworkAccessManager* nam,
                                 QObject* parent)
    : NetworkTask(parent)
    , c_request(request)
    , m_nam(nam)
{}

void HttpRequestTask::executeImpl()
{
    Q_ASSERT(m_nam);
    Q_ASSERT(!m_reply);

    m_redirectCount = 0;
    m_reply = c_request.execute(m_nam);

    QTimer::singleShot(c_request.timeoutMs, m_reply, [this]()
    {
        abortExecution(Timeout);
    });

    connect(m_reply, &QNetworkReply::sslErrors,
            this, [=](const QList<QSslError>& errors)
    {
        qWarning() << name() << "SSL errors: " << errors;
        m_reply->ignoreSslErrors(errors);
    });

    connect(m_reply, &QNetworkReply::errorOccurred,
            this, [=](QNetworkReply::NetworkError error)
    {
        abortExecution(error);
    });

    connect(m_reply, &QNetworkReply::redirected, this, [=](const QUrl& url)
    {
        qInfo() << name() << "Trying to redirect to" << url;

        if (c_request.redirectLimit < 0 ||
            m_redirectCount++ < c_request.redirectLimit)
        {
            qInfo() << name() << "Redirect to" << url << "allowed";

            emit m_reply->redirectAllowed();
        }
        else
        {
            qWarning() << name() << "Redirections limit exceeded:"
                       << c_request.redirectLimit;
            abortExecution(RedirectLimit);
        }
    });

    connect(m_reply, &QNetworkReply::uploadProgress,
            this, [=](qint64 current, qint64 total)
    {
        upload()->setTotal(total);
        upload()->setCurrent(current);
    });

    connect(m_reply, &QNetworkReply::downloadProgress,
            this, [=](qint64 current, qint64 total)
    {
        download()->setTotal(total);
        download()->setCurrent(current);
    });

    if (c_request.output())
    {
        c_request.output()->close();

        if (c_request.output()->open(QIODevice::WriteOnly |
                                     QIODevice::Truncate))
        {
            connect(m_reply, &QNetworkReply::readyRead, this, [=]
            {
                if (!c_request.output())
                {
                    qWarning() << name() << "Output device is null";
                    abortExecution(OutputDeviceOpen);
                    return;
                }

//                if (const QString error = c_request.output()->errorString();
//                    !error.isEmpty())
//                {
//                    qWarning() << name() << "Output device error:" << error;
//                    abortExecution(OutputDeviceWrite);
//                    return;
//                }

                c_request.output()->write(m_reply->readAll());
            });
        }
        else
        {
            qWarning() << name() << "Can't open output device";
            abortExecution(OutputDeviceOpen);
        }
    }

    QObject::connect(m_reply, &QNetworkReply::finished, this, [=]()
    {
        disconnectReply();

        const int errorCode = m_reply->error();

        const int statusCode = m_reply->attribute(
            QNetworkRequest::HttpStatusCodeAttribute).toInt();

        qInfo() << name() << "Execution finished," <<
            "status:" << statusCode << "error:" << errorCode;

        if (errorCode == QNetworkReply::NoError &&
            c_request.acceptCodes.contains(statusCode))
        {
            QVariant result;
            if (c_request.onSuccess)
            {
                result = c_request.onSuccess(m_reply, c_request.output());
            }

            setResult(result);
            m_reply = nullptr;

            return;
        }

        abortExecution(errorCode);
    });
}

void HttpRequestTask::abortExecution(int errorCode)
{
    disconnectReply();

    if (m_reply)
    {
        if (const int replyError = m_reply->error();
            replyError != QNetworkReply::NoError)
        {
            errorCode = replyError;
        }

        if (c_request.onFail)
        {
            c_request.onFail(m_reply);
        }

        if (!m_reply->isRunning())
        {
            m_reply->abort();
        }

        m_reply = nullptr;
    }

    qInfo() << name() << "Execution aborted, error:" << errorCode;

    setFailed(errorCode);
}


void HttpRequestTask::disconnectReply()
{
    if (m_reply)
    {
        m_reply->disconnect(this);
        m_reply->deleteLater();
    }
}
