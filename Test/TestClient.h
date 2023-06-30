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

    bool isReady() const { return m_ready; }

signals:
    void readyChanged();
    void finished();

private:
    bool m_ready = false;
    HttpRequestManager m_manager;

};
