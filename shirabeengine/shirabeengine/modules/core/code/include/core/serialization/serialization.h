/*!
 * @file      sr_serialization.h
 * @author    Marc-Anton Boehm-von Thenen
 * @date      23/07/2018
 * @copyright SmartRay GmbH (www.smartray.com)
 */
#ifndef __SHIRABE_SERIALIZATION_GRAPHVIZ_DOT_SERIALIZER_H__
#define __SHIRABE_SERIALIZATION_GRAPHVIZ_DOT_SERIALIZER_H__

#include <string>
#include <vector>

#include <base/declaration.h>
#include "core/enginetypehelper.h"

namespace engine
{
    namespace serialization
    {
        /*!
         * The ISerializable interface declares the two basic methods required for
         * serializable objects to become compatible with a specific serializer of
         * type TInterfaceSerialization and deserializer of type TInterfaceDeserialization.
         *
         * @tparam TInterfaceSerialization   The serializer type to support interaction with.
         * @tparam TInterfaceDeserialization The deserializer type to support interaction with.
         */
        template <
                typename TInterfaceSerialization
                >
        class ISerializable
        {
            SHIRABE_DECLARE_INTERFACE(ISerializable)

            public_api:
                /*!
                 * Accept a specific serializer and conduct the type serialization process
                 * with it.
                 *
                 * @param aSerializer The serializer instance to perform serialization with.
                 * @return            True, if serialization was successful. False otherwise.
                 */
                virtual bool acceptSerializer(TInterfaceSerialization &aSerializer) const = 0;
        };

        /*!
         * The ISerializable interface declares the two basic methods required for
         * serializable objects to become compatible with a specific serializer of
         * type TInterfaceSerialization and deserializer of type TInterfaceDeserialization.
         *
         * @tparam TInterfaceSerialization   The serializer type to support interaction with.
         * @tparam TInterfaceDeserialization The deserializer type to support interaction with.
         */
        template <
                typename TInterfaceDeserialization
                >
        class IDeserializable
        {
            SHIRABE_DECLARE_INTERFACE(IDeserializable)

            public_api:

                /*!
                 * Accept a specific serializer and conduct the type serialization process
                 * with it.
                 *
                 * @param aDeserializer The deserializer instance to perform deserialization with.
                 * @return              True, if deserialization was successful. False otherwise.
                 * @return
                 */
                virtual bool acceptDeserializer(TInterfaceDeserialization &aDeserializer) = 0;
        };

        /*!
         * The ISerializer<T> interface, provides declarations and interfaces required for a
         * compatible implementation of a serializer.
         * This interface also provides an IResult-interface, to be returned on successful serialization.
         *
         * @tparam T The input type for serialization.
         */
        template <typename T>
        class ISerializer
        {
            SHIRABE_DECLARE_INTERFACE(ISerializer)

        public_classes:
            /*!
             * The IResult interface of the ISerializer<T> interface declares required
             * signatures for result retrieval from a serialization process.
             */
            class IResult
            {
                SHIRABE_DECLARE_INTERFACE(IResult)

            public_api:
                virtual bool asString      (std::string          &aOutString) const = 0;
                virtual bool asBinaryBuffer(std::vector<uint8_t> &aOutBuffer) const = 0;
           };

        public_api:
            /*!
             * Make the serializer ready for processing.
             *
             * @return True if successful. False otherwise.
             */
            virtual bool initialize()   = 0;

            /*!
             * Stop, Cleanup and Destroy.
             *
             * @return True if successful. False otherwise.
             */
            virtual bool deinitialize() = 0;

            /*!
             * Serialize an instance of type T into whichever internal representation and
             * provide it using a pointer to IResult.
             *
             * @param aSource    Input data for serialization.
             * @param aOutResult Result-Instance holding the serialized data, providing access to
             *                   it in various output formats.
             * @return
             */
            virtual bool serialize(T const&aSource, CStdSharedPtr_t<IResult> &aOutResult) = 0;
        };

        /*!
         * The IDeserializer<T> interface, provides declarations and interfaces required for a
         * compatible implementation of a deserializer.
         * This interface also provides an IResult-interface, to be returned on successful deserialization.
         *
         * @tparam T The output type of serialization.
         */
        template <typename T>
        class IDeserializer
        {
            SHIRABE_DECLARE_INTERFACE(IDeserializer)

        public_classes:
            /*!
                 * The IResult interface of the IDeserializer<T> interface declares required
                 * signatures for result retrieval from a deserialization process.
                 */
            class IResult
            {
                SHIRABE_DECLARE_INTERFACE(IResult)

            public_api:
                virtual bool asT(CStdSharedPtr_t<T> &aOutResult) const = 0;
            };

        public_api:
            /*!
             * Make the deserializer ready for processing.
             *
             * @return True if successful. False otherwise.
             */
            virtual bool initialize()   = 0;

            /*!
             * Stop, Cleanup and Destroy.
             *
             * @return True if successful. False otherwise.
             */
            virtual bool deinitialize() = 0;

            /*!
             * Accept a string input of serialized data and invoke the deserialization process,
             * returning a result instance containing the deserialized result of type T.
             *
             * @param aSource    Serialized string representation of an instance of type T.
             * @param aOutResult Result-Instance containing the deserialization result.
             * @return           True if successful, false otherwise.
             */
            virtual bool deserialize(
                    std::string              const &aSource,
                    CStdSharedPtr_t<IResult>       &aOutResult) = 0;

            /*!
             * Accept a byte buffer input of serialized data and invoke the deserialization process,
             * returning a result instance containing the deserialized result of type T.
             *
             * @param aSource    Serialized byte-buffer representation of an instance of type T.
             * @param aOutResult Result-Instance containing the deserialization result.
             * @return           True if successful, false otherwise.
             */
            virtual bool deserialize(
                    std::vector<uint8_t>     const &aSource,
                    CStdSharedPtr_t<IResult>       &aOutResult) = 0;
        };
    }
}

#endif
