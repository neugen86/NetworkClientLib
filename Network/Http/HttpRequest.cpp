#include "HttpRequest.h"

#include <QDebug>

#include <QNetworkReply>
#include <QNetworkAccessManager>

HttpRequest::HttpRequest(Method method,
                         IODevicePtr output)
    : NetworkRequest(output)
    , c_method(method)
{}

HttpRequest::HttpRequest(const QByteArray& method,
                         IODevicePtr output)
    : NetworkRequest(output)
    , c_method(Method::Head)
    , c_customMethod(method)
{}

QNetworkReply* HttpRequest::execute(QNetworkAccessManager* nam) const
{
    Q_ASSERT(nam);

    QNetworkReply* reply = nullptr;

    if (request.url().scheme().isEmpty())
    {
        const QString prev = request.url().toString();
        request.setUrl(QUrl(QString("https://%1").arg(prev)));
    }

    if (!c_customMethod.isEmpty())
    {
        reply = std::visit([&](auto&& value)
        {
            return nam->sendCustomRequest(request, c_customMethod, value);
        }, requestData);
    }
    else
    {
        switch (c_method)
        {
        case Method::Get:
            reply = nam->get(request);
            break;

        case Method::Head:
            reply = nam->head(request);
            break;

        case Method::Post:
            reply = std::visit([&](auto&& value)
            {
                return nam->post(request, value);
            }, requestData);
            break;

        case Method::Put:
            reply = std::visit([&](auto&& value)
            {
                return nam->put(request, value);
            }, requestData);
            break;

        case Method::Delete:
            reply = nam->deleteResource(request);
            break;
        }
    }

    return reply;
}

