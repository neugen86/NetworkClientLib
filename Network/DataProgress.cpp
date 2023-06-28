#include "DataProgress.h"

DataProgress::DataProgress(QObject* parent)
    : QObject(parent)
{}

qint64 DataProgress::total() const
{
    return m_total;
}

void DataProgress::setTotal(qint64 value)
{
    if (m_total != value)
    {
        m_total = value;
        emit totalChanged();

        updateProgress();
    }
}

qint64 DataProgress::current() const
{
    return m_current;
}

void DataProgress::setCurrent(qint64 value)
{
    if (m_current != value)
    {
        m_current = value;
        emit currentChanged();

        updateProgress();
    }
}

float DataProgress::progress() const
{
    return m_progress;
}

void DataProgress::updateProgress()
{
    const float value = float(m_current) / float(m_total);

    if (!qFuzzyCompare(m_progress, value))
    {
        m_progress = value;
        emit progressChanged();

        if (qFuzzyCompare(m_progress, 1.f))
        {
            emit completed();
        }
    }
}
