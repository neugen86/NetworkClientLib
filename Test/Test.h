#pragma once

#include <functional>

#include <QObject>
#include <QList>

class TestClient;

class Test : public QObject
{
    Q_OBJECT

public:
    explicit Test(QObject* parent = nullptr);

    void run();

private:
    using Callback = std::function
    <
        void(Test*, TestClient*)
    >;

    void makeTest(Callback callback);
    void test_1(TestClient* client);
    void test_2(TestClient* client);

signals:
    void testFinished();

private:
    QList<std::function<void()>> m_tests;

};
