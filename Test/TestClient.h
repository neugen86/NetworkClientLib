#pragma once

#include <QObject>

#include "Network/Http/HttpRequestManager.h"

class TestClient : public QObject
{
    Q_OBJECT

public:
    explicit TestClient(QObject* parent = nullptr);

    NetworkTaskResult<QString> sendRequest(const QUrl& url);
    NetworkTaskResult<bool> pingServer(const QUrl& url);

    bool isReady() const;
    bool isExecuting() const;

signals:
    void readyChanged(bool value);
    void executionFinished();

private:
    bool m_ready = false;
    HttpRequestManager m_manager;

};
