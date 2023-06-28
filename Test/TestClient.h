#pragma once

#include <QObject>

#include "Network/NetworkTaskResult.h"
#include "Network/Http/HttpRequestManager.h"

class TestClient : public QObject
{
    Q_OBJECT

public:
    explicit TestClient(QObject* parent = nullptr);

    NetworkTaskResult<qint64> call();
    WeakNetworkTask ping();

private:
    HttpRequestManager m_manager;

};
