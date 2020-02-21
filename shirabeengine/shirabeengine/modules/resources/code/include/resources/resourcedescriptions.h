//
// Created by dottideveloper on 20.10.19.
//

#ifndef SHIRABEDEVELOPMENT_RESOURCEDESCRIPTIONS_H
#define SHIRABEDEVELOPMENT_RESOURCEDESCRIPTIONS_H

#include <vector>
#include <platform/platform.h>
#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>
#include <core/enginetypehelper.h>
#include <core/databuffer.h>
#include <core/bitfield.h>
#include <asset/assettypes.h>
#include <graphicsapi/definitions.h>
#include "resources/resourcedatasource.h"

namespace engine
{
    namespace resources
    {
        using namespace graphicsapi;

        using ResourceId_t = std::string;

        struct
            [[nodiscard]]
            SHIRABE_TEST_EXPORT SNoDependencies {

            SHIRABE_INLINE
            Vector<ResourceId_t> const resolve() const
            {
                return {};
            }
        };

        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT SBufferDescription
        {
            std::string                       name;
            VkBufferCreateInfo                createInfo;
            DataSourceAccessor_t              dataSource;
            std::vector<DataSourceAccessor_t> initialData; // Important: Just an accessor. Resource data is not in memory here.
        };

        struct
            [[nodiscard]]
            SHIRABE_TEST_EXPORT SBufferViewDescription
        {
            std::string            name;
            VkBufferViewCreateInfo createInfo;
        };

        struct
            [[nodiscard]]
            SHIRABE_TEST_EXPORT SBufferViewDependencies
        {
            ResourceId_t bufferId;

            SHIRABE_INLINE
            Vector<ResourceId_t> const resolve() const
            {
                return { bufferId };
            }
        };

        struct
            [[nodiscard]]
            SHIRABE_TEST_EXPORT STextureDescription
        {
        public_members:
            std::string                      name;
            STextureInfo                     textureInfo;
            EResourceUsage                   cpuGpuUsage;
            core::CBitField<EBufferBinding>  gpuBinding;
            Vector<DataSourceAccessor_t>     initialData;
        };

        struct
            [[nodiscard]]
            SHIRABE_TEST_EXPORT STextureViewDescription
        {
        public_members:
            std::string   name;
            STextureInfo  subjacentTextureInfo;
            EFormat       textureFormat;
            ArraySlices_t arraySlices;
            MipSlices_t   mipMapSlices;
            // TODO: Distinguish binding and read/write mode
        };

        struct
            [[nodiscard]]
            SHIRABE_TEST_EXPORT STextureViewDependencies
        {
            ResourceId_t  subjacentTextureId;

            SHIRABE_INLINE
            Vector<ResourceId_t> const resolve() const
            {
                return { subjacentTextureId };
            }
        };

        struct
            [[nodiscard]]
            SHIRABE_TEST_EXPORT SDepthStencilStateDescription
        {
        public_members:
            std::string      name;
            bool             enableDepth;
            EDepthWriteMask  depthMask;
            EComparison      depthFunc;
            bool             enableStencil;
            uint8_t          stencilReadMask;
            uint8_t          stencilWriteMask;
            SStencilCriteria stencilFrontfaceCriteria;
            SStencilCriteria stencilBackfaceCriteria;
        };

        struct
            [[nodiscard]]
            SHIRABE_TEST_EXPORT SRasterizerStateDescription
        {
        public_members:
            std::string name;
            EFillMode   fillMode;
            ECullMode   cullMode;
            bool        antialiasRasterLines;
            bool        multisamplingEnabled; // Requires respective format
            bool        scissorEnabled;
            bool        depthClipEnabled;
        };

        struct SSubpassDependency
        {
            uint32_t             srcPass
                               , dstPass;
            VkPipelineStageFlags srcStage
                               , dstStage;
            VkAccessFlags        srcAccess
                               , dstAccess;
            VkDependencyFlags    dependencyFlags;

        };

        struct SAttachmentDescription
        {
            EFormat            format;
            EAttachmentLoadOp  loadOp;
            EAttachmentStoreOp storeOp;
            EAttachmentLoadOp  stencilLoadOp;
            EAttachmentStoreOp stencilStoreOp;
            EImageLayout       initialLayout;
            EImageLayout       finalLayout;
            VkClearValue       clearColor;
            bool               isColorAttachment;
            bool               isDepthAttachment;
            bool               isInputAttachment;
        };

        struct SAttachmentReference
        {
            uint32_t     attachment;
            EImageLayout layout;
        };

        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT SSubpassDescription
        {
            std::vector<SAttachmentReference> inputAttachments;
            std::vector<SAttachmentReference> colorAttachments;
            std::vector<SAttachmentReference> resolveAttachments;
            std::vector<SAttachmentReference> depthStencilAttachments;
            std::vector<uint32_t>             preserveStencilAttachments;
        };

        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT SRenderPassDescription
        {
        public_members:
            std::string                         name;
            VkExtent3D                          attachmentExtent;
            std::vector<SAttachmentDescription> attachmentDescriptions;
            std::vector<SSubpassDescription>    subpassDescriptions;
            std::vector<SSubpassDependency>     subpassDependencies;
        };

        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT SRenderPassDependencies
        {
            std::vector<ResourceId_t> attachmentTextureViews;

            SHIRABE_INLINE
            Vector<ResourceId_t> const resolve() const
            {
                return std::vector<ResourceId_t>(attachmentTextureViews);
            }
        };

        struct
            [[nodiscard]]
            SHIRABE_TEST_EXPORT SFrameBufferDescription
        {

        public_members:
            std::string name;
        };

        struct
            [[nodiscard]]
            SHIRABE_TEST_EXPORT SFrameBufferDependencies
        {
            ResourceId_t              referenceRenderPassId; // Used as a template to create a compatible framebuffer
            VkExtent3D                attachmentExtent;
            std::vector<ResourceId_t> attachmentTextureViews;

            SHIRABE_INLINE
            Vector<ResourceId_t> const resolve() const
            {
                std::vector<ResourceId_t> dependencies {};
                dependencies.push_back(referenceRenderPassId);
                for(auto const &view : attachmentTextureViews)
                {
                    dependencies.push_back(view);
                }

                return dependencies;
            }
        };

        struct
            [[nodiscard]]
            SHIRABE_TEST_EXPORT SSwapChainBufferDescription
        {
            std::string         name;
            STextureDescription texture;
            uint32_t            backBufferIndex;
        };

        struct
            [[nodiscard]]
            SHIRABE_TEST_EXPORT SSwapChainDescription
        {
            std::string         name;
            STextureDescription texture;
            bool                vsyncEnabled;
            bool                fullscreen;
            unsigned int        windowHandle;
            unsigned int        backBufferCount;
            unsigned int        refreshRateNumerator;
            unsigned int        refreshRateDenominator;
        };

        struct
                [[nodiscard]]
                SHIRABE_TEST_EXPORT SShaderModuleDescriptor
        {
            std::string                                                  name;
            std::unordered_map<VkShaderStageFlags, DataSourceAccessor_t> shaderStages;
        };

        struct
            [[nodiscard]]
            SHIRABE_TEST_EXPORT SMaterialPipelineDescriptor
        {
            std::string                                                  name;

            VkViewport                                                   viewPort;
            VkRect2D                                                     scissor;

            bool                                                         includesSystemBuffers;

            VkPipelineInputAssemblyStateCreateInfo                       inputAssemblyState;
            std::vector<VkVertexInputBindingDescription>                 vertexInputBindings;
            std::vector<VkVertexInputAttributeDescription>               vertexInputAttributes;

            VkPipelineRasterizationStateCreateInfo                       rasterizerState;
            VkPipelineMultisampleStateCreateInfo                         multiSampler;
            VkPipelineDepthStencilStateCreateInfo                        depthStencilState;
            std::vector<VkPipelineColorBlendAttachmentState>             colorBlendAttachmentStates;
            VkPipelineColorBlendStateCreateInfo                          colorBlendState;

            VkPipelineLayoutCreateInfo                                   pipelineLayout;
            std::vector<VkDescriptorSetLayoutCreateInfo>                 descriptorSetLayoutCreateInfos;
            std::vector<std::vector<VkDescriptorSetLayoutBinding>>       descriptorSetLayoutBindings;
        };

        struct
            [[nodiscard]]
            SHIRABE_TEST_EXPORT SMaterialPipelineDependencies
        {
            ResourceId_t              systemUBOPipelineId;
            ResourceId_t              referenceRenderPassId;
            uint32_t                  subpass;
            ResourceId_t              shaderModuleId;
            std::vector<ResourceId_t> bufferViewIds;
            std::vector<ResourceId_t> textureViewIds;

            SHIRABE_INLINE
            Vector<ResourceId_t> const resolve() const
            {
                std::vector<ResourceId_t> dependencies {};
                dependencies.push_back(systemUBOPipelineId);
                dependencies.push_back(referenceRenderPassId);
                dependencies.push_back(shaderModuleId);

                for(auto const &buffer : bufferViewIds)
                {
                    dependencies.push_back(buffer);
                }
                for(auto const &texture : textureViewIds)
                {
                    dependencies.push_back(texture);
                }

                return dependencies;
            }
        };

        struct
            [[nodiscard]]
            SHIRABE_TEST_EXPORT SMaterialDescriptor
        {
            std::string                        name;
            SMaterialPipelineDescriptor        pipelineDescriptor;
            SShaderModuleDescriptor            shaderModuleDescriptor;
            Vector<SBufferDescription>         uniformBufferDescriptors;
            Vector<ResourceId_t>               sampledImages;
        };

        struct
            [[nodiscard]]
            SHIRABE_TEST_EXPORT SMaterialDependencies
        {
            SMaterialPipelineDependencies pipelineDependencies;

            SHIRABE_INLINE
            Vector<ResourceId_t> const resolve() const
            {
                return {};
            }
        };

        struct
            [[nodiscard]]
            SHIRABE_TEST_EXPORT SMeshDescriptor
        {
            std::string                               name;
            uint32_t                                  attributeCount;
            uint32_t                                  indexSampleCount;
            SBufferDescription                        dataBufferDescription;
            SBufferDescription                        indexBufferDescription;
            Vector<VkVertexInputBindingDescription>   bindingDescriptions;
            Vector<VkVertexInputAttributeDescription> attributeDescriptions;
            Vector<VkDeviceSize>                      offsets;
        };

        struct
            [[nodiscard]]
            SHIRABE_TEST_EXPORT SMeshDependencies
        {
            SHIRABE_INLINE
            Vector<ResourceId_t> const resolve() const
            {
                return {};
            }
        };

    }
}

#endif //SHIRABEDEVELOPMENT_RESOURCETYPES_H
