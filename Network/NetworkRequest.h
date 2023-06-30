#pragma once

#include <QIODevice>
#include <QSharedPointer>

using IODevicePtr = QSharedPointer<QIODevice>;

class NetworkRequest
{
protected:
    explicit NetworkRequest(IODevicePtr output = {});
    virtual ~NetworkRequest();

public:
    static const int DefaultTimeout = 3000;
    int timeoutMs = DefaultTimeout;

    QIODevice* output() const;

private:
    IODevicePtr m_output;

};
