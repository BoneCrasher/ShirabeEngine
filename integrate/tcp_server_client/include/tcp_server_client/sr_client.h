/*!
 * @file      sr_client.h
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
 * The CTCPClient class implements an I/O TCP client to communicate with a CTCPServer instance.
 */
class CTCPClient
{
public_constructors:
    /*!
     * Construct a new TCPClient from an io_context and a list of endpoints.
     *
     * @param [in] aAsioIOContext The underlying libASIO state the client is attached to.
     * @param [in] aEndPoints     List of available endpoints for the desired host and port.
     */
    CTCPClient(
            asio::io_context                   &aAsioIOContext,
            tcp::resolver::results_type const& aEndPoints);

public_methods:
    /*!
     * Start the client.
     */
    void start();

    /*!
     * Stop the client.
     */
    void stop();

    /*!
     * Get the currently attached session.
     *
     * @return See brief.
     */
    CStdSharedPtr_t<CTCPSession> getSession();

private_methods:
    /*!
     * Attempt to connect to a client in the end points list.
     *
     * @param [in] aEndPoints List of available endpoints for the desired host and port.
     */
    void connect(tcp::resolver::results_type const& aEndPoints);

private_members:
    asio::io_context                 &mAsioIOContext;
    tcp::resolver::results_type const mEndPoints;
    tcp::socket                       mSocket;
    CStdSharedPtr_t<CTCPSession>      mSession;
};
