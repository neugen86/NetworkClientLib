#include "NetworkRequest.h"

NetworkRequest::NetworkRequest(NetworkOutputDevice output)
    : m_output(output)
{}

NetworkRequest::~NetworkRequest()
{
    if (m_output)
    {
        m_output->close();
    }
}

NetworkOutputDevice NetworkRequest::output() const
{
    return m_output;
}
