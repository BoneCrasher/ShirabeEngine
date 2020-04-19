/*!
 * @file      sr_session.cpp
 * @author    Marc-Anton Boehm-von Thenen
 * @date      02/08/2018
 * @copyright SmartRay GmbH (www.smartray.com)
 */

#include "sr_pch.h"

#include "tcp_server_client/shared/sr_session.h"

//<-----------------------------------------------------------------------------
//
//<-----------------------------------------------------------------------------
CStdSharedPtr_t<CTCPSession> CTCPSession::create(
        asio::io_context       &aAsioIOService,
        tcp::socket             aSocket,
        MessageHandler_t const &aIncomingMessageHandler)
{
    CStdSharedPtr_t<CTCPSession> ptr =
            makeStdSharedPtr<CTCPSession>(
                aAsioIOService,
                std::move(aSocket),
                aIncomingMessageHandler);

    return ptr;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
CTCPSession::CTCPSession(
        asio::io_context       &aAsioIOService,
        tcp::socket             aSocket,
        MessageHandler_t const &aIncomingMessageHandler)
    : mAsioIOService(aAsioIOService)
    , mSocket(std::move(aSocket))
    , mIncomingMessageHandler(aIncomingMessageHandler)
{
    SR_ASSERTTEXT(nullptr != mIncomingMessageHandler, "Incoming message handler is null.");
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
void CTCPSession::start()
{
    readMessageHeader();
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
void CTCPSession::stop()
{
    mSocket.close();
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
void CTCPSession::writeMessage(CTCPMessage const &aMessage)
{
    auto const fn = [=] ()
    {
        writeMessageHeader(aMessage);
    };
    asio::post(mAsioIOService, fn);
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
void CTCPSession::writeMessageHeader(CTCPMessage const &aMessage)
{
    auto const completionHandler = [this, message=aMessage] (
            std::error_code const &aErrorCode,
            std::size_t     const &aSize)
    {
        bool const error = static_cast<bool>(aErrorCode);
        if(!error)
        {
            writeMessageBody(message);
        }
        else
        {
            std::cout << "Write header failed. error code: " << aErrorCode.value() << "\n";
            stop();
        }
    };

    void const * const data = static_cast<void const*>(aMessage.getHeader().getData());
    asio::async_write(
                mSocket,
                asio::buffer(data, CTCPMessage::sMessageHeaderSize),
                completionHandler);
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
void CTCPSession::writeMessageBody(CTCPMessage const &aMessage)
{
    auto const completionHandler = [this] (
            std::error_code const &aErrorCode,
            std::size_t     const &aSize)
    {
        SR_UNUSED(aSize);

        bool const error = static_cast<bool>(aErrorCode);
        if(!error)
        {
            // Done
        }
        else
        {
            std::cout << "Write body failed. error code: " << aErrorCode.value() << "\n";
            stop();
        }
    };

    void const * const data = static_cast<void const*>(aMessage.getData().data());
    asio::async_write(
                mSocket,
                asio::buffer(data, aMessage.getMessageSize()),
                completionHandler);
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
void CTCPSession::readMessageHeader()
{
    mMessage.getData() = std::string("");
    auto const completionHandler = [this] (
            std::error_code const &aErrorCode,
            std::size_t     const &aHeaderSize)
    {
        SR_UNUSED(aHeaderSize);

        bool const error = static_cast<bool>(aErrorCode);
        if(!error)
        {
            mMessage.getData().resize(mMessage.getHeader().mMessageLength);
            readMessageBody();
        }
        else
        {
            std::cout << "Read header failed. error code: " << aErrorCode.value() << "\n";
            stop();
        }
    };

    void* const data = static_cast<void*>(mMessage.getHeader().getData());
    asio::async_read(
                mSocket,
                asio::buffer(data, CTCPMessage::sMessageHeaderSize),
                completionHandler);
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
void CTCPSession::readMessageBody()
{
    auto const completionHandler = [this](
            std::error_code const &aErrorCode,
            std::size_t     const &aMessageSize)
    {
        SR_UNUSED(aMessageSize);

        bool const error = static_cast<bool>(aErrorCode);
        if(!error)
        {
            mIncomingMessageHandler(mMessage);

            readMessageHeader();
        }
        else
        {
            std::cout << "read body failed. error code: " << aErrorCode.value() << "\n";
            mSocket.close();
        }
    };

    void *const data = static_cast<void*>(mMessage.getData().data());
    asio::async_read(
                mSocket,
                asio::buffer(data, mMessage.getHeader().mMessageLength),
                completionHandler);
}
//<-----------------------------------------------------------------------------
