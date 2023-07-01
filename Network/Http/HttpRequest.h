#pragma once

#include <variant>
#include <functional>

#include <QSet>
#include <QVariant>
#include <QPointer>
#include <QByteArray>
#include <QNetworkReply>
#include <QNetworkRequest>

#include "Network/NetworkRequest.h"

class QIODevice;
class QHttpMultiPart;
class QNetworkAccessManager;

class HttpRequest : public NetworkRequest
{
public:
    enum Method
    {
        Get,
        Head,
        Post,
        Put,
        Delete
    };

    using RequestData = std::variant
    <
        QByteArray,
        QIODevice*,
        QHttpMultiPart*
    >;

    using SuccessCallback = std::function<
        QVariant(QNetworkReply*, QIODevice*)>;

    using FailCallback = std::function<void(QNetworkReply*)>;

    explicit HttpRequest(Method method, IODevicePtr output = {});
    explicit HttpRequest(const QByteArray& method, IODevicePtr output = {});

    QNetworkReply* execute(QNetworkAccessManager* nam) const;

private:
    const Method c_method;
    const QByteArray c_customMethod;

public:
    int redirectLimit = 3;
    RequestData requestData;
    mutable QNetworkRequest request;
    QSet<int> acceptCodes = { 200 };

    SuccessCallback onSuccess;
    FailCallback onFail;

};
