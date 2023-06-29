#include "Test.h"

#include <QDebug>

#include "Network/AbstractNetworkTask.h"

Test::Test(QObject* parent)
    : QObject(parent)
{}

void Test::run()
{
    for (int i = 0; i < 16; ++i)
    {
        m_client.callMicrosoft();
    }

    auto ping = m_client.pingGoogle();
    NetworkTaskResult call = m_client.callMicrosoft();

    // ping processing
    {
        connect(ping.task().data(), &AbstractTask::statusChanged, this, [ping]()
        {
            qDebug() << ping.task()->name() << "[ping] in progress";
        });

        connect(ping.task().data(), &AbstractTask::completed, this, []()
        {
            qDebug() << "[ping] finished";
        });

        ping.onResultReady([](bool result)
        {
            qDebug() << "[ping]" << (result ? "ok" : "failed");
        });
    }

    // call processing
    {
        connect(call.task().data(), &AbstractTask::completed, this, [ping]()
        {
            qDebug() << "Cancelling ping task";
            ping.task()->cancel();
        });

        call.onResultReady([call](const QString& data)
        {
            qDebug() << call.task()->name() << "[call]:" << data;
        });
    }
}
