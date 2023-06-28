#pragma once

#include <functional>

#include <QObject>

#include "NetworkTask.h"
#include "NetworkDefines.h"

template <typename T>
class NetworkTaskResult
{
    WeakNetworkTask m_ref;

public:
    explicit NetworkTaskResult(const WeakNetworkTask& ref)
        : m_ref(ref)
    {}

    WeakNetworkTask ref() const { return m_ref; }

    void onResultReady(std::function<void(const T&)> callback)
    {
        const auto task = ref().lock();

        if (!task)
        {
            return;
        }

        auto setValue = [=]()
        {
            const QVariant result = task->result();

            if constexpr (std::is_same_v<T, QVariant>)
            {
                callback(result);
            }
            else
            {
                callback(result.value<T>());
            }
        };

        if (task->isCompleted())
        {
            setValue();
            return;
        }

        auto ctx = new QObject;
        ctx->connect(task.data(), &Task::resultChanged, [=]
        {
            ctx->deleteLater();
            setValue();
        });
    }

};
