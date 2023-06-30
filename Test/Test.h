#pragma once

#include <functional>

#include <QObject>
#include <QList>

class Test : public QObject
{
    Q_OBJECT

public:
    explicit Test(QObject* parent = nullptr);

    void run();

private:
    class TestClient* makeClient();

    void test_1();
    void test_2();

signals:
    void testFinished();

private:
    QList<std::function<void()>> m_tests;

};
