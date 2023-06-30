#pragma once

#include <QPointer>
#include <QIODevice>

using OutputDevice = QPointer<QIODevice>;

class NetworkRequest
{
protected:
    explicit NetworkRequest(OutputDevice output = {});
    virtual ~NetworkRequest();

public:
    static const int DefaultTimeout = 1000;
    int timeoutMs = DefaultTimeout;

    OutputDevice output() const;

private:
    OutputDevice m_output;

};
