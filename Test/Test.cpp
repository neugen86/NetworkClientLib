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
        m_client.callQt();
    }

    // status handling
    auto ping = m_client.pingGoogle();
    {
        connect(ping.task().data(), &AbstractTask::statusChanged, this, [ping]()
        {
            qDebug() << ping.task()->name() << "[ping] status changed";
        });

        connect(ping.task().data(), &AbstractTask::completed, this, []()
        {
            qDebug() << "[ping] finishing...";
        });
    }

    // result handling
    {
        NetworkTaskResult call = m_client.callQt();

        call.onResultReady([call](const QVariant& value)
        {
            qDebug() << call.task()->name() << "[call] data received:" << value;
        });

        connect(call.task().data(), &AbstractTask::completed, this, [ping]()
        {
            qDebug() << "Cancelling ping task";
            ping.task()->cancel();
        });
    }
}
