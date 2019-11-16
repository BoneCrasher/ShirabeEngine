/*!
 * @file      sr_session.h
 * @author    Marc-Anton Boehm-von Thenen
 * @date      02/08/2018
 * @copyright SmartRay GmbH (www.smartray.com)
 */

#pragma once

#define ASIO_NO_DEPRECATED
#define USE_ASIO
#define ASIO_STANDALONE
#include <asio.hpp>

#include "tcp_server_client/shared/sr_message.h"

using tcp = asio::ip::tcp;

/*!
 * The CTCPSession class defines a libASIO session for I/O tcp communication.
 */
class CTCPSession
{
public_typedefs:
    using MessageHandler_t = std::function<void(CTCPMessage)>;

public_static_functions:
    /*!
     * Create a new CTCPSession from an asio io_context,
     * an attached socket and an incoming message handler.
     *
     * @param [in] aAsioIOService          The underlying libASIO service state.
     * @param [in] aSocket                 The socket to which the session is attached.
     * @param [in] aIncomingMessageHandler A message callback for incoming message fwd.
     * @return                             A pointer to a valid CTCPSession or nullptr on error.
     */
    static CStdSharedPtr_t<CTCPSession> create(
            asio::io_context       &aAsioIOService,
            tcp::socket             aSocket,
            MessageHandler_t const &aIncomingMessageHandler);

public_constructors:
    /*!
     * Create a new CTCPSession from an asio io_context,
     * an attached socket and an incoming message handler.
     *
     * @param [in] aAsioIOService          The underlying libASIO service state.
     * @param [in] aSocket                 The socket to which the session is attached.
     * @param [in] aIncomingMessageHandler A message callback for incoming message fwd.
     */
    CTCPSession(
            asio::io_context       &aAsioIOService,
            tcp::socket             aSocket,
            MessageHandler_t const &aIncomingMessageHandler);

public_methods:
    /*!
     * Start the session, attempting to read the first message.
     */
    void start();

    /*!
     * Stop and properly shutdown the session.
     */
    void stop();

    /*!
     * Asynchronously write a message to the peer(s).
     *
     * @param [in] aMessage The message to write.
     */
    void writeMessage(CTCPMessage const &aMessage);

private_methods:
    /*!
     * Asynchronously write out the message header to the peer(s).
     *
     * @param [in] aMessage The related message whose header is to written.
     */
    void writeMessageHeader(CTCPMessage const &aMessage);
    /*!
     * Asynchronously write out the message data to the peer(s).
     *
     * @param [in] aMessage The related message whose data is to be written.
     */
    void writeMessageBody(CTCPMessage const &aMessage);

    /*!
     * Asynchronously read the next message's header.
     */
    void readMessageHeader();
    /*!
     * Asynchronously read the next message's data.
     */
    void readMessageBody();

private_members:
    asio::io_context &mAsioIOService;
    tcp::socket       mSocket;

    CTCPMessage      mMessage;
    MessageHandler_t mIncomingMessageHandler;
};
