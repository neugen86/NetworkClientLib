#include "NetworkRequest.h"

NetworkRequest::NetworkRequest(OutputDevice output)
    : m_output(output)
{}

NetworkRequest::~NetworkRequest()
{
    if (m_output)
    {
        m_output->close();
    }
}

OutputDevice NetworkRequest::output() const
{
    return m_output;
}
