#pragma once

#include <property_system/serialization/sr_property_json_de_serializer.h>
#include <tcp_server_client/shared/sr_session.h>

/*!
 * Convert an arbitrary value to its corresponding string representation.
 *
 * @tparam TDataType   Input data type of the value to be converted to string.
 * @param  [in] aValue Value to convert to string.
 * @return             The string representation of the input value.
 */
template <typename TDataType>
static std::string toString(TDataType const &aValue)
{
    if constexpr (std::is_same_v<TDataType, CStdSharedPtr_t<CMetaObject>>)
    {
        return "Object";
    }
    else if constexpr (std::is_same_v<TDataType, std::wstring>)
    {
        std::string const narrowed = StaticStringHelpers::narrow(aValue);
        return narrowed;
    }
    else
    {
        std::stringstream ss;
        ss << aValue;
        return ss.str();
    }
}

/*!
 * The handler factory provides a statically typed replication handler for property value changes.
 */
class CHandlerFactory
{
public_constructors:

    // @Andras: This should be refactored to accepting an "IReplicationInterface" so that
    //          you can write out what ever you want to write out...
    //          Due to the time available on wednesday I directly used the network session.
    /*!
     * Construct a new replication handler attached to a specific tcp-session.
     *
     * @param [in] aSession The CTCPSession which will handle the property value changes.
     */
    CHandlerFactory(CStdSharedPtr_t<CTCPSession> aSession);

public_methods:
    /*!
     * Create a statically typed replication handler, which formats a message and writes it
     * out onto the network.
     *
     * @tparam TDataType The underlying data type of the property to be observed and replicated.
     * @return           A callable obeying the CMetaSystem::Handler_t<T> signature.
     */
    template <typename TDataType>
    CMetaSystem::Handler_t<TDataType> createHandler()
    {
        auto const fn = [this](
                CMetaProperty<TDataType> const &aProperty,
                TDataType                const &aValue,
                uint32_t                 const &aValueIndex)
        {
            std::string const valueString   = toString<TDataType>(aValue);
            std::string const messageString = StaticStringHelpers::formatString("writeProperty/%s/%" PRIu32 "/%s", aProperty.getPropertyPath().c_str(), aValueIndex, valueString.c_str());
            CTCPMessage const message       = CTCPMessage::create(messageString);

            mSession->writeMessage(message);
        };

        return fn;
    }

private_members:
    CStdSharedPtr_t<CTCPSession> mSession;
};
