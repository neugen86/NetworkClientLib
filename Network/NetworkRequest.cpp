#include "NetworkRequest.h"

#include <QBuffer>

namespace
{
QSharedPointer<QBuffer> MakeOutput()
{
    QSharedPointer<QByteArray> data(
        new QByteArray, [](auto ptr)
        {
            delete ptr;
        }
    );

    return QSharedPointer<QBuffer>(
        new QBuffer(data.data()), [data](auto ptr)
        {
            ptr->deleteLater();
        }
    );
}
} // anonymous namespace


NetworkRequest::NetworkRequest(IODevicePtr output)
{
    if (!output)
    {
        m_output = MakeOutput();
    }
}

QIODevice* NetworkRequest::output() const
{
    return m_output.data();
}
