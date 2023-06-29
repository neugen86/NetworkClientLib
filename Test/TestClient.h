#pragma once

#include <QObject>

#include "Network/Http/HttpRequestManager.h"

class TestClient : public QObject
{
    Q_OBJECT

public:
    explicit TestClient(QObject* parent = nullptr);

    NetworkTaskResult<QString> callMicrosoft();
    NetworkTaskResult<bool> pingGoogle();

private:
    HttpRequestManager m_manager;

};
