#include "HttpRequestTask.h"

#include <QDebug>
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

    m_redirectionsCount = 0;
    m_reply = c_request.execute(m_nam);

    connect(m_reply, &QNetworkReply::sslErrors,
            this, [=](const QList<QSslError>& errors)
    {
        qWarning() << name() << "SSL errors: " << errors;
        m_reply->ignoreSslErrors(errors);
    });

    connect(m_reply, &QNetworkReply::errorOccurred,
            this, [=](QNetworkReply::NetworkError error)
    {
        qWarning() << name() << "Network error: " << error;
        abortExecution();
    });

    connect(m_reply, &QNetworkReply::redirected, this, [=](const QUrl& url)
    {
        qInfo() << name() << "Trying to redirect to" << url;

        if (c_request.redirectLimit < 0 ||
            m_redirectionsCount < c_request.redirectLimit)
        {
            qInfo() << name() << "Redirect to" << url << "allowed";

            emit m_reply->redirectAllowed();
            ++m_redirectionsCount;
        }
        else
        {
            qWarning() << name() << "Redirections limit exceeded:"
                       << c_request.redirectLimit;
            abortExecution();
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

        if (c_request.output()->open(QIODevice::ReadWrite |
                                     QIODevice::Truncate))
        {
            connect(m_reply, &QNetworkReply::readyRead, this, [=]
                    {
                        c_request.output()->write(m_reply->readAll());
                    });
        }
        else
        {
            abortExecution();
        }
    }

    QObject::connect(m_reply, &QNetworkReply::finished, this, [=]()
    {
        disconnectReply();

        const int error = m_reply->error();

        const int code = m_reply->attribute(
            QNetworkRequest::HttpStatusCodeAttribute).toInt();

        qInfo() << name() << "Execution finished, code:" << code << "error:" << error;

        if (error == QNetworkReply::NoError &&
            c_request.acceptCodes.contains(code))
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

        abortExecution();
    });
}

void HttpRequestTask::abortExecution()
{
    disconnectReply();

    int error = Task::UnknownError;

    if (m_reply)
    {
        error = m_reply->error();

        qInfo() << name() << "Execution aborted, error:" << error;

        if (c_request.onFail)
        {
            c_request.onFail(m_reply);
        }

        m_reply->abort();
        m_reply = nullptr;
    }

    setFailed(error);
}


void HttpRequestTask::disconnectReply()
{
    if (m_reply)
    {
        m_reply->disconnect(this);
        m_reply->deleteLater();
    }
}
