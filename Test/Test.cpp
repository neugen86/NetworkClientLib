#include "Test.h"

#include <QDebug>
#include <QCoreApplication>

#include "Network/NetworkTask.h"

Test::Test(QObject* parent)
    : QObject(parent)
{}

void Test::run()
{
    for (int i = 0; i < 16; ++i)
    {
        m_client.call();
    }

    // status handling
    {
        SharedNetworkTask task = m_client.ping().lock();

        connect(task.data(), &Task::statusChanged, this, [task]()
        {
            qDebug() << task->name() << "[pingServer] status changed";
        });
    }

    // result handling
    {
        NetworkTaskResult<qint64> taskResult = m_client.call();
        SharedNetworkTask task = taskResult.ref().lock();

        taskResult.onResultReady([task](qint64 value)
        {
            qDebug() << task->name() << "[serverCall] data received:" << value;
        });

        connect(task.data(), &Task::completed, this, [task]()
        {
            qDebug() << task->name() << "[serverCall] the last task completed";

            QMetaObject::invokeMethod(
                qApp, [](){ qApp->exit(); },
                Qt::QueuedConnection
            );
        });
    }
}
