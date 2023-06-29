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
    NetworkOutputDevice output() const;

public:
    int timeoutMs = 1000;

private:
    NetworkOutputDevice m_output;

};
