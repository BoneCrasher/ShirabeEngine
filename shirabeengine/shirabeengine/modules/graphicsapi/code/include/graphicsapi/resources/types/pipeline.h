#ifndef __SHIRABE_GRAPHICSAPI_TYPES_PIPELINE_H__
#define __SHIRABE_GRAPHICSAPI_TYPES_PIPELINE_H__

#include <vector>
#include <stdint.h>

#include <vulkan/vulkan.h>

#include <base/declaration.h>
#include <math/geometric/rect.h>

#include <resources/core/eresourcetype.h>
#include <resources/core/iresource.h>
#include <resources/core/resourcedomaintransfer.h>
#include <resources/core/resourcetraits.h>
#include <resources/core/requestdefaultimplementation.h>

#include "graphicsapi/resources/types/definition.h"

namespace engine
{
    namespace gfxapi
    {
        using namespace math;
        using namespace resources;

        /**
         * The CFrameBufferDeclaration class describes a CFrameBuffer's integration into the resource/backend.
         */
        class SHIRABE_TEST_EXPORT CPipelineDeclaration
        {
        public_static_constants:
            static const constexpr EResourceType    sResourceType    = EResourceType   ::GAPI_COMPONENT;
            static const constexpr EResourceSubType sResourceSubtype = EResourceSubType::PIPELINE;

        public_structs:
            /**
             * The SDescriptor struct provides all necessary information about a texture view.
             */
            struct SHIRABE_TEST_EXPORT SDescriptor
                    : public SDescriptorImplBase<EResourceType::GAPI_COMPONENT, EResourceSubType::PIPELINE>
            {
            public_constructors:
                /**
                 * Default-Construct an empty SDescriptor.
                 */
                SDescriptor();

            public_methods:
                /**
                 * Describe this descriptor as string.
                 *
                 * @return See brief.
                 */
                std::string toString() const;

            public_members:
                std::string                                            name;
                uint32_t                                               subpass;
                VkViewport                                             viewPort;
                VkRect2D                                               scissor;

                VkPipelineInputAssemblyStateCreateInfo                 inputAssemblyState;
                std::vector<VkVertexInputBindingDescription>           vertexInputBindings;
                std::vector<VkVertexInputAttributeDescription>         vertexInputAttributes;

                VkPipelineRasterizationStateCreateInfo                 rasterizerState;
                VkPipelineMultisampleStateCreateInfo                   multiSampler;
                VkPipelineDepthStencilStateCreateInfo                  depthStencilState;
                std::vector<VkPipelineColorBlendAttachmentState>       colorBlendAttachmentStates;
                VkPipelineColorBlendStateCreateInfo                    colorBlendState;

                VkPipelineLayoutCreateInfo                             pipelineLayout;
                std::vector<VkDescriptorSetLayoutCreateInfo>           descriptorSetLayoutCreateInfos;
                std::vector<std::vector<VkDescriptorSetLayoutBinding>> descriptorSetLayoutBindings;

                std::unordered_map<VkShaderStageFlags, ByteBuffer>     shaderStages;
            };

            /**
             * The CreationRequest struct describes how a texture view should be created.
             */
            struct SHIRABE_TEST_EXPORT CCreationRequest
                    : public CBaseDeclaration::CCreationRequestBase<SDescriptor>
            {
            public_constructors:
                /**
                 * Construct a creation request from a descriptor, render pass and texture view handles.
                 *
                 * @param aDescriptor         The texture view descriptor to be used for creation.
                 * @param aRenderPassHandle   The render pass describing the usage of the frame buffer.
                 * @param aTextureViewHandles The texture handles of the texture views to be bound for the framebuffer.
                 */
                CCreationRequest(
                        SDescriptor            const &aDescriptor,
                        PublicResourceId_t     const &aRenderPassHandle,
                        PublicResourceIdList_t const &aTextureViewHandles,
                        PublicResourceIdList_t const &aBufferViewHandles);

            public_methods:
                /**
                 * Return the resource handle to the render pass this pipeline
                 * should be created against.
                 *
                 * @return See brief
                 */
                PublicResourceId_t const &renderPassHandle() const;

                /**
                 * Return the list of associated texture view handles to be bound.
                 *
                 * @return See brief.
                 */
                PublicResourceIdList_t const &textureViewHandles() const;

                /**
                 * Return the list of associated texture view handles to be bound.
                 *
                 * @return See brief.
                 */
                PublicResourceIdList_t const &bufferViewHandles() const;

                /**
                 * Describe this creation request as string.
                 *
                 * @return See brief.
                 */
                std::string toString() const;

            private_members:

                PublicResourceId_t     mRenderPassHandle;
                PublicResourceIdList_t mTextureViewHandles;
                PublicResourceIdList_t mBufferViewHandles;
            };

            /**
             * TBDone
             */
            using CUpdateRequest = CBaseDeclaration::CUpdateRequestBase;

            /**
             * TBDone
             */
            using CDestructionRequest = CBaseDeclaration::CDestructionRequestBase;

            /**
             * TBDone
             */
            using CQuery = CBaseDeclaration::CQueryBase;
        };


        /**
         * FrameBuffer implementation for all texture views in the engine
         */
        class SHIRABE_TEST_EXPORT CPipeline
                : public CPipelineDeclaration
                , public CResourceDescriptorAdapter<CPipelineDeclaration::SDescriptor>
        {
        public_typedefs:
            using MyType_t = CPipeline;

        public_constructors:
            /**
             * Construct a texture view from a texture view descriptor.
             * @param aDescriptor
             */
            CPipeline(CPipeline::SDescriptor const &aDescriptor);
        };

        /**
         *
         */
        SHIRABE_DEFINE_PUBLIC_TRAITS_TYPES(CPipeline);

    }
}


#endif
