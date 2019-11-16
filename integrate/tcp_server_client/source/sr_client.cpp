/*!
 * @file      sr_client.cpp
 * @author    Marc-Anton Boehm-von Thenen
 * @date      27/06/2018
 * @copyright SmartRay GmbH (www.smartray.com)
 */

#include "sr_pch.h"

#include "tcp_server_client/shared/sr_message.h"

#include "tcp_server_client/sr_client.h"

//<-----------------------------------------------------------------------------
//
//<-----------------------------------------------------------------------------
CTCPClient::CTCPClient(
        asio::io_context                   &aAsioIOContext,
        tcp::resolver::results_type const& aEndPoints)
    : mAsioIOContext(aAsioIOContext)
    , mEndPoints(aEndPoints)
    , mSocket(aAsioIOContext)
    , mSession(nullptr)
{
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
void CTCPClient::start()
{
    connect(mEndPoints);
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
void CTCPClient::stop()
{
    asio::post(mAsioIOContext, [this] () { mSocket.close(); });
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
CStdSharedPtr_t<CTCPSession> CTCPClient::getSession()
{
    return mSession;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
void CTCPClient::connect(tcp::resolver::results_type const& aEndPoints)
{
    auto const connectHandler = [this](std::error_code const &aErrorCode, tcp::endpoint aEndPoint)
    {
        SR_UNUSED(aEndPoint);

        bool const error = static_cast<bool>(aErrorCode);
        if(!error)
        {
            if(mSession)
            {
                std::cout << "Connection refused: There is already a connection and session available in the client.\n";
                return;
            }

            auto const incomingMessageHandler = [] (CTCPMessage message)
            {
                std::cout << message.getData() << "\n";
            };

            mSession = CTCPSession::create(mAsioIOContext, std::move(mSocket), incomingMessageHandler);
            mSession->start();
        }
    };

    asio::async_connect(mSocket, aEndPoints, connectHandler);
}
//<-----------------------------------------------------------------------------
