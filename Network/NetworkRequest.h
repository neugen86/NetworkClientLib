#pragma once

#include <QPointer>
#include <QIODevice>

using NetworkOutputDevice = QPointer<QIODevice>;

class NetworkRequest
{
public:
    explicit NetworkRequest(NetworkOutputDevice output = {});
    ~NetworkRequest();

    NetworkOutputDevice output() const;

public:
    int timeoutMs = 1000;

private:
    NetworkOutputDevice m_output;

};
