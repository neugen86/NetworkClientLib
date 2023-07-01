#include "NetworkRequest.h"

#include <QBuffer>

namespace
{
QSharedPointer<QBuffer> MakeOutput()
{
    auto data = new QByteArray;

    return QSharedPointer<QBuffer>(
        new QBuffer(data), [data](auto ptr)
        {
            ptr->deleteLater();
            delete data;
        }
    );
}
} // anonymous namespace


NetworkRequest::NetworkRequest(IODevicePtr output)
    : m_output(output ? output : MakeOutput())
{}

QIODevice* NetworkRequest::output() const
{
    return m_output.data();
}
