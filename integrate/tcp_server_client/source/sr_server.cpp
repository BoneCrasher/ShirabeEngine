/*!
 * @file      sr_server.cpp
 * @author    Marc-Anton Boehm-von Thenen
 * @date      02/08/2018
 * @copyright SmartRay GmbH (www.smartray.com)
 */
#include "sr_pch.h"

#include "tcp_server_client/shared/sr_session.h"
#include "tcp_server_client/sr_server.h"

//<-----------------------------------------------------------------------------
//
//<-----------------------------------------------------------------------------
CTCPServer::CTCPServer(
        asio::io_context                 &aAsioIOContext,
        tcp::endpoint                    &aEndPoint,
        ConnectionSuccessHandler_t const &aConnectionSuccessHandler)
    : mAsioIOService(aAsioIOContext)
    , mAcceptor(aAsioIOContext, aEndPoint)
    , mConnectionSuccessHandler(aConnectionSuccessHandler)
    , mSession(nullptr)
{
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
void CTCPServer::start()
{
    accept();
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
void CTCPServer::stop()
{
    if(mSession)
    {
        mSession->stop();
    }
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
CStdSharedPtr_t<CTCPSession> CTCPServer::getSession()
{
    return mSession;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
void CTCPServer::accept()
{
    auto acceptFn = [this](
            std::error_code const &aError,
            tcp::socket            aSocket)
    {
        if(!aError)
        {
            if(mSession)
            {
                std::cout << "Connection refused: There is already a connection and session available in the server.\n";
                aSocket.close();
                return;
            }

            auto incomingMessageHandler = [] (CTCPMessage message)
            {
                std::cout << message.getData() << "\n";
            };

            mSession = CTCPSession::create(mAsioIOService, std::move(aSocket), incomingMessageHandler);

            if(mConnectionSuccessHandler)
            {
                mConnectionSuccessHandler(mSession);
            }

            mSession->start();
        }

        // Accept more connections.
        accept();
    };

    mAcceptor.async_accept(acceptFn);
}
//<-----------------------------------------------------------------------------
