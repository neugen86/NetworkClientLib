#pragma once

#include <QPointer>
#include <QIODevice>

using NetworkOutputDevice = QPointer<QIODevice>;

class NetworkRequest
{
protected:
    explicit NetworkRequest(NetworkOutputDevice output = {});
    ~NetworkRequest();

public:
    NetworkOutputDevice output() const;

public:
    int timeoutMs = 1000;

private:
    NetworkOutputDevice m_output;

};
