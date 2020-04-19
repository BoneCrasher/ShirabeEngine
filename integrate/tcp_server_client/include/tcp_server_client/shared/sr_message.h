/*!
 * @file      sr_message.h
 * @author    Marc-Anton Boehm-von Thenen
 * @date      02/08/2018
 * @copyright SmartRay GmbH (www.smartray.com)
 */

#pragma once

#include <cstring>
#include <functional>
#include <iostream>
#include <deque>

#include <core/base/defines/sr_defines.h>
#include <core/kernel/sr_kernel.h>
#include <core/base/types/smartptr/sr_cppstd_ptr.h>

/*!
 * The CTCPMessage encapsulates a plain header and data section of specific length.
 */
class CTCPMessage
{
private_structs:
    /*!
     * Definition of a test mesasge header.
     */
    struct SHeader
    {
        uint32_t mMessageLength;

        SR_INLINE SHeader const *getData() const { return this; }
        SR_INLINE SHeader       *getData()       { return this; }
    };

public_static_functions:
    /*!
     * Create a new message from a string value.
     *
     * @param [in] aMessage The string to store in the internal data buffer.
     * @return              An instance of CTCPMessage initialized with the string data.
     */
    static CTCPMessage create(std::string const &aMessage);

public_static_constants:
    static constexpr std::size_t const sMessageHeaderSize  = sizeof(SHeader);
    static constexpr std::size_t const sMessageBodyMaxSize = std::numeric_limits<uint32_t>::max();

    /*!
     * Default construct and empty message
     */
    CTCPMessage();

    /*!
     * Provide the current header immutably.
     *
     * @return See brief.
     */
    SHeader const &getHeader() const;

    /*!
     * Provide the current header mutably.
     *
     * @return See brief.
     */
    SHeader &getHeader();

    /*!
     * Provide the internal data immutably.
     *
     * @return See brief.
     */
    std::string const &getData() const;

    /*!
     * Provide the internal data mutably.
     *
     * @return See brief.
     */
    std::string &getData();

    /*!
     * Return the total current size of the message (Header + Message)
     *
     * @return See brief.
     */
    std::size_t getLength() const;

    /*!
     * Return the current size of the data-component.
     *
     * @return See brief.
     */
    std::size_t getMessageSize() const;

private_methods:
    /*!
     * Construct a new message from a message string.
     *
     * @param [in] aMessage Data to initialize the message with.
     */
    CTCPMessage(std::string const &aMessage);

private_members:
    SHeader     mHeader;
    std::string mData;
};

using CTCPMessageQueue = std::deque<CTCPMessage>;
