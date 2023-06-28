#pragma once

#include <QObject>

#include "TestClient.h"

class Test : public QObject
{
    Q_OBJECT

public:
    explicit Test(QObject* parent = nullptr);

    void run();

private:
    TestClient m_client;

};
