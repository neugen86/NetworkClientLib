#pragma once

#include <variant>
#include <functional>

#include <QSet>
#include <QVariant>
#include <QPointer>
#include <QByteArray>
#include <QNetworkRequest>
#include <QNetworkReply>

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
        QVariant(QNetworkReply*, OutputDevice)>;

    using FailCallback = std::function<void(QNetworkReply*)>;

    explicit HttpRequest(Method method, OutputDevice output = {});
    explicit HttpRequest(const QByteArray& method, OutputDevice output = {});

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
