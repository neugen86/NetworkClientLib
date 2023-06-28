#include "Test.h"

#include <QDebug>

#include "Network/NetworkTask.h"

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
    NetworkTaskPtr pingTask = m_client.pingGoogle().lock();
    {
        connect(pingTask.data(), &Task::statusChanged, this, [pingTask]()
        {
            qDebug() << pingTask->name() << "[ping] status changed";
        });

        connect(pingTask.data(), &Task::completed, this, [pingTask]()
        {
            qDebug() << "[ping] finishing...";
        });
    }

    // result handling
    {
        NetworkTaskResult<qint64> taskResult = m_client.callQt();
        NetworkTaskPtr task = taskResult.ref().lock();

        taskResult.onResultReady([task](qint64 value)
        {
            qDebug() << task->name() << "[call] data received:" << value;
        });

        connect(task.data(), &Task::completed, this, [task, pingTask]()
        {
            qDebug() << "Cancelling ping task";
            pingTask->cancel();
        });
    }
}
