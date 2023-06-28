#pragma once

#include <QObject>

class DataProgress : public QObject
{
    Q_OBJECT

    Q_PROPERTY(float progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(qint64 total READ total WRITE setTotal NOTIFY totalChanged)
    Q_PROPERTY(qint64 current READ current WRITE setCurrent NOTIFY currentChanged)

public:
    explicit DataProgress(QObject* parent = nullptr);

    qint64 total() const;
    void setTotal(qint64 value);

    qint64 current() const;
    void setCurrent(qint64 value);

    float progress() const;

private:
    void updateProgress();

signals:
    void completed(QPrivateSignal = {});
    void totalChanged(QPrivateSignal = {});
    void currentChanged(QPrivateSignal = {});
    void progressChanged(QPrivateSignal = {});

private:
    qint64 m_total = 0;
    qint64 m_current = 0;
    float m_progress = 0;

};
