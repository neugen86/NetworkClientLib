#include "NetworkRequest.h"

#include <QBuffer>

namespace
{
QSharedPointer<QBuffer> MakeOutput()
{
    QSharedPointer<QByteArray> data(
        new QByteArray, [](QByteArray* ptr)
        {
            delete ptr;
        }
    );

    return QSharedPointer<QBuffer>(
        new QBuffer(data.data()), [data](QBuffer* buf)
        {
            Q_ASSERT(buf);
            buf->deleteLater();
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

NetworkRequest::~NetworkRequest()
{}

QIODevice* NetworkRequest::output() const
{
    return m_output.data();
}
