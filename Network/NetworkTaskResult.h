#pragma once

#include <functional>

#include <QObject>
#include <QPointer>

#include "NetworkDefines.h"
#include "AbstractNetworkTask.h"

template <typename T = QVariant>
class NetworkTaskResult
{
    NetworkTaskRef m_ref;

public:
    explicit NetworkTaskResult(const NetworkTaskRef& ref)
        : m_ref(ref)
    {}

    NetworkTaskPtr task() const { return m_ref.lock(); }

    void onResultReady(std::function<void(const T&)> callback)
    {
        const auto task = this->task();

        if (!task)
        {
            return;
        }

        auto setValue = [=]()
        {
            const QVariant result = task->result();
            callback(result.value<T>());
        };

        if (task->isCompleted())
        {
            setValue();
            return;
        }

        QPointer<QObject> ctx(new QObject);
        ctx->connect(task.data(), &AbstractTask::resultChanged, [=]
        {
            if (ctx)
            {
                ctx->deleteLater();
            }

            setValue();
        });
    }

};
