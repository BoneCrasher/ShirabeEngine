/*!
 * @file      sr_server.h
 * @author    Marc-Anton Boehm-von Thenen
 * @date      02/08/2018
 * @copyright SmartRay GmbH (www.smartray.com)
 */

#pragma once

#define ASIO_NO_DEPRECATED
#define USE_ASIO
#define ASIO_STANDALONE
#include <asio.hpp>

#include "tcp_server_client/shared/sr_session.h"

using tcp = asio::ip::tcp;

/*!
 * Simple TCPServer accepting a single tcp connection.
 */
class CTCPServer
{
public_typedefs:
    using ConnectionSuccessHandler_t = std::function<void(CStdSharedPtr_t<CTCPSession>)>;

public_constructors:
    /*!
     * Construct a new TCPServer from an io context and end point.
     *
     * @param [in] aAsioIOContext            The underlying libASIO state the server is attached to.
     * @param [in] aEndPoint                 The endpoint to bind the server to.
     * @param [in] aConnectionSuccessHandler Callback-function to be invoked on successful connection.
     */
    CTCPServer(
            asio::io_context                 &aAsioIOContext,
            tcp::endpoint                    &aEndPoint,
            ConnectionSuccessHandler_t const &aConnectionSuccessHandler);

public_methods:
    /*!
     * Start the server.
     */
    void start();

    /*!
     * Stop the server.
     */
    void stop();

    /*!
     * Get the current session attached after successful connection.
     *
     * @return See brief
     */
    CStdSharedPtr_t<CTCPSession> getSession();

private_methods:
    /*!
     * Start accepting new connections.
     */
    void accept();

private_members:
    asio::io_context             &mAsioIOService;
    tcp::acceptor                 mAcceptor;
    ConnectionSuccessHandler_t    mConnectionSuccessHandler;
    CStdSharedPtr_t<CTCPSession>  mSession;
};
