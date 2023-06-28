#pragma once

#include <QScopedPointer>

#include "Task/Task.h"
#include "DataProgress.h"

class NetworkTask : public Task
{
    Q_OBJECT

    Q_PROPERTY(DataProgress* upload READ upload CONSTANT)
    Q_PROPERTY(DataProgress* download READ download CONSTANT)
    Q_PROPERTY(DataDirection dataDirection READ dataDirection NOTIFY dataDirectionChanged)

public:
    enum class DataDirection
    {
        None,
        In,
        Out
    };
    Q_ENUM(DataDirection)

    explicit NetworkTask(QObject* parent = nullptr);
    ~NetworkTask();

    DataProgress* upload() const;
    DataProgress* download() const;

    DataDirection dataDirection() const;

protected:
    void setDataDirection(DataDirection value);

private:
    virtual void abortExecution() = 0;

signals:
    void dataDirectionChanged(QPrivateSignal = {});

private:
    QScopedPointer<DataProgress> m_upload;
    QScopedPointer<DataProgress> m_download;
    DataDirection m_dataDirection = DataDirection::None;

};
