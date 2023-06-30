#include "HttpRequestManager.h"

#include <QNetworkAccessManager>

#include "HttpRequestTask.h"

HttpRequestManager::HttpRequestManager(QObject* parent)
    : NetworkTaskManager(parent)
    , m_nam(new QNetworkAccessManager(this))
{
    Q_ASSERT(m_nam);

    m_nam->setRedirectPolicy(QNetworkRequest::UserVerifiedRedirectPolicy);
}

HttpRequestManager::~HttpRequestManager()
{}

void HttpRequestManager::setProxy(const QNetworkProxy& proxy)
{
    m_nam->setProxy(proxy);
}

NetworkTaskPtr HttpRequestManager::makeTask(const HttpRequest& request)
{
    return QSharedPointer<HttpRequestTask>(
        new HttpRequestTask(request, m_nam.data(), this),
        &QObject::deleteLater
    ).staticCast<NetworkTask>();
}
