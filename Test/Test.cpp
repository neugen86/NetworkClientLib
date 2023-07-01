#include "Test.h"

#include <QDebug>
#include <QPointer>
#include <QCoreApplication>

#include "TestClient.h"

Test::Test(QObject* parent)
    : QObject(parent)
{}

void Test::run()
{
    m_tests =
    {
        [this](){ makeTest(&Test::test_1); },
        [this](){ makeTest(&Test::test_2); },
    };

    connect(this, &Test::testFinished, this, [this]()
    {
        if (m_tests.isEmpty())
        {
            qApp->exit();
            return;
        }

        qDebug() << "";
        qDebug() << "=======================";
        qDebug() << "= Test started ========";

        m_tests.first()();
        m_tests.pop_front();

    }, Qt::QueuedConnection);

    emit testFinished();
}

void Test::makeTest(Callback callback)
{
    auto client = new TestClient(this);
    Q_ASSERT(!client->isReady());

    connect(client, &TestClient::readyChanged, this, [=]()
    {
        Q_ASSERT(!client->isExecuting());
        callback(this, client);
        Q_ASSERT(client->isExecuting());
    });

    connect(client, &TestClient::executionFinished, this, [=]()
    {
        client->disconnect(this);
        client->deleteLater();

        emit testFinished();
    });
}

void Test::test_1(TestClient* client)
{
    for (int i = 0; i < 8; ++i)
    {
        client->sendRequest(QUrl("qt.io"));
    }
}

void Test::test_2(TestClient* client)
{
    NetworkTaskResult ping = client->pingServer(QUrl("google.com"));
    NetworkTaskResult request = client->sendRequest(QUrl("wikipedia.org"));

    // ping processing
    {
        connect(ping.task().data(), &Task::statusChanged, this, []()
        {
            qDebug() << "[ping] status changed";
        });

        connect(ping.task().data(), &Task::completed, this, []()
        {
            qDebug() << "[ping] finished";
        });

        ping.onResultReady([](bool result)
        {
            qDebug() << "[ping]" << (result ? "ok" : "failed");
        });
    }

    // request processing
    {
        request.onResultReady([request](const QString& data)
        {
            qDebug() << request.task()->name() << "[request]:" << data;
        });

        connect(request.task().data(), &Task::completed, this, [=]()
        {
            qDebug() << "=== The last request completed";

            if (auto task = ping.task())
            {
                qDebug() << "=== Cancelling ping task";
                task->cancel();
            }
        });
    }
}
