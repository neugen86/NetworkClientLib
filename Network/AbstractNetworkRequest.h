#pragma once

#include <QPointer>
#include <QIODevice>

using NetworkOutputDevice = QPointer<QIODevice>;

class AbstractNetworkRequest
{
protected:
    explicit AbstractNetworkRequest(
        NetworkOutputDevice output = {});

    virtual ~AbstractNetworkRequest();

public:
    static const int DefaultTimeout = 1000;

    NetworkOutputDevice output() const;

public:
    int timeoutMs = DefaultTimeout;

private:
    NetworkOutputDevice m_output;

};
