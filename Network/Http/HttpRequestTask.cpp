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
        abortExecution(Timeout, QString("Timeout %1ms exceeded")
                                    .arg(c_request.timeoutMs));
    });

    connect(m_reply, &QNetworkReply::sslErrors,
            this, [=](const QList<QSslError>& errors)
    {
        qWarning() << name() << "SSL errors: " << errors;
        m_reply->ignoreSslErrors(errors);
    });

    connect(m_reply, &QNetworkReply::errorOccurred,
            this, [=](QNetworkReply::NetworkError code)
    {
        abortExecution(code, QString("Network error: %1")
                                 .arg(m_reply->errorString()));
    });

    connect(m_reply, &QNetworkReply::redirected, this, [=](const QUrl& url)
    {
        qInfo() << name() << "Redirecting to" << url.toString();

        if (c_request.redirectLimit < 0 ||
            m_redirectCount++ < c_request.redirectLimit)
        {
            emit m_reply->redirectAllowed();
        }
        else
        {
            abortExecution(RedirectLimit, QString("Redirections limit exceeded: %1")
                                              .arg(c_request.redirectLimit));
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
        if (c_request.output()->isOpen())
        {
            c_request.output()->close();
        }

        if (c_request.output()->open(
                QIODevice::ReadWrite | QIODevice::Truncate))
        {
            connect(m_reply, &QNetworkReply::readyRead, this, [=]()
            {
                c_request.output()->write(m_reply->readAll());
            });
        }
        else
        {
            abortExecution(
                OutputDeviceOpen, QLatin1String("Can't open output device"));
        }
    }

    QObject::connect(m_reply, &QNetworkReply::finished, this, [=]()
    {
        disconnectReply();

        const int code = m_reply->error();

        const int status = m_reply->attribute(
            QNetworkRequest::HttpStatusCodeAttribute).toInt();

        if (code == QNetworkReply::NoError &&
            c_request.acceptCodes.contains(status))
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

        abortExecution(code, m_reply->errorString());
    });
}

void HttpRequestTask::abortExecution(int code, const QString& message)
{
    disconnectReply();

    if (m_reply)
    {
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

    setFailed(code, message.isEmpty()
                        ? QLatin1String("Execution aborted")
                        : message);
}

void HttpRequestTask::disconnectReply()
{
    if (m_reply)
    {
        m_reply->disconnect(this);
        m_reply->deleteLater();
    }
}
