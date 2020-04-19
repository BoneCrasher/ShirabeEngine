#ifndef __SHIRABE__MEASUREMENT_CHUNK_H__
#define __SHIRABE__MEASUREMENT_CHUNK_H__

#include <iostream>
#include <wchar.h>

#include <base/declaration.h>

namespace engine
{
    namespace benchmarking
    {
        /**
         * A Measurement chunk is a two dimensional assignment of key to value as a
         * single object.
         *
         * @tparam TParameter
         * @tparam TValue
         */
        template <typename TParameter, typename TValue>
        class CMeasurementChunk
        {
        public_typedefs:
            using ParameterType_t = TParameter;
            using ValueType_t     = TValue    ;

        public_constructors:
            /**
             * Construct a chunk from a parameter and value.
             *
             * @param aParam
             * @param aValue
             */
            explicit CMeasurementChunk(
                    ParameterType_t const &aParam = 0,
                    ValueType_t     const &aValue = 0)
                : mParameter(aParam)
                , mValue(aValue)
            { }
            /**
             * Copy-Construct a chunk from another.
             *
             * @param aOther
             */
            explicit CMeasurementChunk(
                    CMeasurementChunk<TParameter, TValue> const& aOther)
                : mParameter(aOther.mParameter)
                , mValue(aOther.mValue)
            {}

        public_destructors:
            /**
             * Destroy and run...
             */
            ~CMeasurementChunk() = default;

        public_methods:
            /**
             * Return the stored parameter value.
             *
             * @return
             */
            ParameterType_t const &parameter() const { return mParameter; }

            /**
             * Return the stored value.
             * @return
             */
            ValueType_t     const &value()     const { return mValue;     }

        protected_members:
            ParameterType_t mParameter;
            ValueType_t     mValue;

        private_methods:
            /**
             * Permit printing the chunk to byte stream.
             *
             * @param strm
             * @param chunk
             * @return
             */
            friend std::basic_ostream<char> operator<<(
                    std::basic_ostream<char>                              &aStream,
                    CMeasurementChunk<ParameterType_t, ValueType_t> const &aChunk)
            {
                return (aStream << aChunk.parameter() << ": " << aChunk.value());
            }

            /**
             * Permit printing the chunk to 16-bit stream.
             *
             * @param strm
             * @param chunk
             * @return
             */
            friend std::basic_ostream<wchar_t> operator<<(
                    std::basic_ostream<wchar_t>                           &aStream,
                    CMeasurementChunk<ParameterType_t, ValueType_t> const &aChunk)
            {
                return (aStream << aChunk.parameter() << L": " << aChunk.value());
            }
        };
    }
}

#endif
