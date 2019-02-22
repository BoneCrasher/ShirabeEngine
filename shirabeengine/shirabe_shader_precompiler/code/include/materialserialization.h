#ifndef __SHIRABE_SHADERPRECOMP_MATERIAL_SERIALIZATION_H__
#define __SHIRABE_SHADERPRECOMP_MATERIAL_SERIALIZATION_H__

#include <sstream>
#include <optional>

#include <nlohmann/json.hpp>
#include <spirv_cross/spirv_cross.hpp>

#include <log/log.h>
#include <core/enginetypehelper.h>
#include <core/serialization/serialization.h>

#include "definition.h"

namespace shader_precompiler
{
    namespace serialization
    {
        using engine::serialization::ISerializer;
        using engine::serialization::ISerializable;

        /**
         * The IFrameGraphSerializer interface describes the basic requiremets
         * to serialize a framegraph instance.
         */
        class IMaterialSerializer
                : public ISerializer<SShaderCompilationUnit>
        {
            SHIRABE_DECLARE_INTERFACE(IMaterialSerializer);

        public_api:
            virtual bool writeProgramHeader(SShaderCompilationUnit const &aUnit) = 0;

            virtual bool writeInputDescriptionElement(std::vector<spirv_cross::Resource> const &aResource) = 0;

            virtual bool writeSubpassInputs(std::vector<spirv_cross::Resource> const &aResource) = 0;

            virtual bool serializeSubpassOutputs(std::vector<spirv_cross::Resource> const &aResource) = 0;

            virtual bool serializeUniformBuffers(std::vector<spirv_cross::Resource> const &aResource) = 0;

            virtual bool serializeSampledImages(std::vector<spirv_cross::Resource> const &aResource) = 0;
        };

        /**
         * The FrameGraphGraphVizSerializer class implements framegraph serialization
         * to the graphviz dot format.
         */
        class SHIRABE_TEST_EXPORT CMaterialSerializer
                : public IMaterialSerializer
        {
            SHIRABE_DECLARE_LOG_TAG(CMaterialSerializer);

        public_structs:
            /*!
             * The IResult interface of the ISerializer<T> interface declares required
             * signatures for result retrieval from a serialization process.
             */
            class CMaterialSerializationResult
                    : public ISerializer<SShaderCompilationUnit>::IResult
            {
            public_constructors:
                CMaterialSerializationResult(nlohmann::json const &aResult);

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
             * @param aUnit      Input data for serialization.
             * @param aOutResult Result-Instance holding the serialized data, providing access to
             *                   it in various output formats.
             * @return
             */
            bool serialize(SShaderCompilationUnit const &aUnit, CStdSharedPtr_t<IResult> &aOutResult);

            bool serializeUnit(SShaderCompilationUnit const &aUnit);

            bool serializeStage(SShaderCompilationElement const &aStage);

            bool serializeInputDescription(std::vector<spirv_cross::Resource> const &aResource);

            bool serializeSubpassInputs(std::vector<spirv_cross::Resource> const &aResource);

            bool serializeSubpassOutputs(std::vector<spirv_cross::Resource> const &aResource);

            bool serializeUniformBuffers(std::vector<spirv_cross::Resource> const &aResource);

            bool serializeSampledImages(std::vector<spirv_cross::Resource> const &aResource);

        private_methods:
            /**
             * Begin writing the framegraph, writing out some header information and style data for
             * the dot layout engine.
             */
            void beginGraph();
            /**
             * Finalize the dot graph.
             */
            void endGraph();

        private_members:
            std::stringstream mStream;
        };
    }
}

#endif
