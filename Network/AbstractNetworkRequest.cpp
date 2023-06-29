#include "AbstractNetworkRequest.h"

AbstractNetworkRequest::AbstractNetworkRequest(
        NetworkOutputDevice output)
    : m_output(output)
{}

AbstractNetworkRequest::~AbstractNetworkRequest()
{
    if (m_output)
    {
        m_output->close();
    }
}

NetworkOutputDevice AbstractNetworkRequest::output() const
{
    return m_output;
}
