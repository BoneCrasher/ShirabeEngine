#ifndef __SHIRABE_MATERIAL_SERIALIZATION_H__
#define __SHIRABE_MATERIAL_SERIALIZATION_H__

#include <sstream>
#include <optional>

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
             * Serialize a compilation element (implicit: a shader stage).
             *
             * @param aStage The stage to serialize.
             * @return       True, if successful.
             * @return       False, on error.
             */
            virtual bool serializeStage(SMaterialStage const &aStage) = 0;
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
             * Serialize a compilation element (implicit: a shader stage).
             *
             * @param aStage The stage to serialize.
             * @return       True, if successful.
             * @return       False, on error.
             */
            bool serializeStage(SMaterialStage const &aStage);

        private_members:
            std::stringstream mStream;
        };
    }
}

#endif
