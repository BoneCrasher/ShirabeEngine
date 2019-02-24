#ifndef __SHIRABE_MATERIAL_SERIALIZATION_H__
#define __SHIRABE_MATERIAL_SERIALIZATION_H__

#include <sstream>
#include <optional>
#include <stack>

#include <nlohmann/json.hpp>

#include <log/log.h>
#include <core/enginetypehelper.h>
#include <core/serialization/serialization.h>

#include "material/material_declaration.h"

namespace engine
{
    namespace serialization
    {
        using namespace engine::material;

        using engine::serialization::ISerializer;
        using engine::serialization::ISerializable;

        /**
         * The IFrameGraphSerializer interface describes the basic requiremets
         * to serialize a framegraph instance.
         */
        class IMaterialSerializer
                : public ISerializer<SMaterial>
        {
            SHIRABE_DECLARE_INTERFACE(IMaterialSerializer);

        public_api:
            /**
             * Begin a JSON array, to which the upcoming objects will be added.
             *
             * @param aName Name of the array to begin.
             * @return
             */
            virtual bool beginArray(std::string const &aName) = 0;

            /**
             * End the current array. If any.
             * @return
             */
            virtual bool endArray() = 0;

            /**
             * Begin a new object w/ the given name.
             *
             * @param aName
             * @return
             */
            virtual bool beginObject(std::string const &aName) = 0;

            /**
             * End the current object, if any.
             *
             * @return
             */
            virtual bool endObject() = 0;

            /**
             * Write a string value to the current object/array.
             *
             * @param aKey
             * @param aValue
             * @return
             */
            virtual bool writeValue(std::string const &aKey, std::string const &aValue) = 0;

            /**
             * Write a numeric value to the current object/array by converting
             * it to string beforehand.
             *
             * @param aKey
             * @param aValue
             * @return
             */
            template <typename TValue>
            bool writeNumericValue(std::string const &aKey, TValue const &aValue)
            {
                std::string const value = CString::toString<TValue>(aValue);
                return writeValue(aKey, value);
            }
        };

        /**
         * The FrameGraphGraphVizSerializer class implements framegraph serialization
         * to the graphviz dot format.
         */
        class SHIRABE_TEST_EXPORT CMaterialSerializer
                : public IMaterialSerializer
        {
            SHIRABE_DECLARE_LOG_TAG(CMaterialSerializer);

        public_destructors:
            virtual ~CMaterialSerializer() = default;

        public_structs:
            /*!
             * The IResult interface of the ISerializer<T> interface declares required
             * signatures for result retrieval from a serialization process.
             */
            class SHIRABE_TEST_EXPORT CMaterialSerializationResult
                    : public ISerializer<SMaterial>::IResult
            {
            public_constructors:
                CMaterialSerializationResult();

                CMaterialSerializationResult(nlohmann::json const &aResult);

            public_destructors:
                virtual ~CMaterialSerializationResult() = default;

            public_methods:
                bool asString      (std::string          &aOutString) const;
                bool asBinaryBuffer(std::vector<uint8_t> &aOutBuffer) const;

            private_members:
                nlohmann::json const mResult;
            };

        public_methods:
            /**
             * Initialize the serializer and prepare for serialization calls.
             *
             * @return True, if successful. False otherwise.
             */
            bool initialize();

            /**
             * Cleanup and shutdown...
             *
             * @return True, if successful. False otherwise.
             */
            bool deinitialize();            

            /*!
             * Serialize an instance of type SShaderCompilationUnit into whichever internal representation and
             * provide it using a pointer to IResult.
             *
             * @param aMaterial  Input data for serialization.
             * @param aOutResult Result-Instance holding the serialized data, providing access to
             *                   it in various output formats.
             * @return
             */
            bool serialize(SMaterial const &aMaterial, CStdSharedPtr_t<IResult> &aOutResult);

            /**
             * Begin a JSON array, to which the upcoming objects will be added.
             *
             * @param aName Name of the array to begin.
             * @return
             */
            bool beginArray(std::string const &aName);

            /**
             * End the current array. If any.
             * @return
             */
            bool endArray();

            /**
             * Begin a new object w/ the given name.
             *
             * @param aName
             * @return
             */
            bool beginObject(std::string const &aName);

            /**
             * End the current object, if any.
             *
             * @return
             */
            bool endObject();

            /**
             * Write a string value to the current object/array.
             *
             * @param aKey
             * @param aValue
             * @return
             */
            bool writeValue(std::string const &aKey, std::string const &aValue);

        private_methods:

            /**
             * Check, whether there's a current top level item.
             *
             * @return
             */
            bool hasCurrentItem() const
            {
                return (not mCurrentJSONState.empty());
            }

            /**
             * Return a reference to the current top level item.
             *
             * @return
             */
            SHIRABE_INLINE
            nlohmann::json &getCurrentItem()
            {
                return mCurrentJSONState.top().get();
            }

        private_members:

            nlohmann::json                                     mRoot;
            std::stack<std::reference_wrapper<nlohmann::json>> mCurrentJSONState;
        };
    }
}

#endif
