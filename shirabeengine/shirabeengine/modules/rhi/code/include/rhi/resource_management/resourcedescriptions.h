//
// Created by dottideveloper on 20.10.19.
//

#ifndef SHIRABEDEVELOPMENT_RESOURCEDESCRIPTIONS_H
#define SHIRABEDEVELOPMENT_RESOURCEDESCRIPTIONS_H

#include <vector>
#include <string>

#include <platform/platform.h>
#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>
#include <core/enginetypehelper.h>
#include <core/databuffer.h>
#include <core/bitfield.h>
#include <asset/assettypes.h>
#include <asset/assetstorage.h>
#include <graphicsapi/definitions.h>

#include "rhi/resource_management/resourcedatasource.h"

namespace engine
{
    namespace rhi
    {
        using namespace graphicsapi;

        using ResourceId_t = std::string;

        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT SRHIMemoryDescription
        {
            std::string           name;
            VkMemoryRequirements  memoryRequirements;
            VkMemoryPropertyFlags memoryProperties;
        };

        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT SRHIBufferDescription
        {
            std::string          name;
            VkBufferCreateInfo   createInfo;
            DataSourceAccessor_t dataSource;
        };

        struct
            [[nodiscard]]
            SHIRABE_TEST_EXPORT SRHIBufferViewDescription
        {
            std::string            name;
            ResourceId_t           subjacentBufferId;
            VkBufferViewCreateInfo createInfo;
        };

        struct
            [[nodiscard]]
            SHIRABE_TEST_EXPORT SRHIImageDescription
        {
        public_members:
            std::string                      name;
            STextureInfo                     imageInfo;
            EResourceUsage                   cpuGpuUsage;
            core::CBitField<EBufferBinding>  gpuBinding;
            DataSourceAccessor_t             initialData;
        };

        struct
            [[nodiscard]]
            SHIRABE_TEST_EXPORT SRHIImageViewDescription
        {
        public_members:
            std::string   name;
            ResourceId_t  subjacentImageId;
            EFormat       imageFormat;
            ArraySlices_t arraySlices;
            MipSlices_t   mipMapSlices;
            // TODO: Distinguish binding and read/write mode
        };

        struct
            [[nodiscard]]
            SHIRABE_TEST_EXPORT SRHIDepthStencilStateDescription
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
            SHIRABE_TEST_EXPORT SRHIRasterizerStateDescription
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

        struct SRHISubpassDependency
        {
            uint32_t             srcPass
                               , dstPass;
            VkPipelineStageFlags srcStage
                               , dstStage;
            VkAccessFlags        srcAccess
                               , dstAccess;
            VkDependencyFlags    dependencyFlags;

        };

        struct SRHIAttachmentDescription
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

        struct SRHIAttachmentReference
        {
            uint32_t     attachment;
            EImageLayout layout;
        };

        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT SRHISubpassDescription
        {
            std::vector<SRHIAttachmentReference> inputAttachments;
            std::vector<SRHIAttachmentReference> colorAttachments;
            std::vector<SRHIAttachmentReference> resolveAttachments;
            std::vector<SRHIAttachmentReference> depthStencilAttachments;
            std::vector<uint32_t>                preserveStencilAttachments;
        };

        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT SRHIRenderPassDescription
        {
        public_members:
            std::string                            name;
            VkExtent3D                             attachmentExtent;
            std::vector<SRHIAttachmentDescription> attachmentDescriptions;
            std::vector<ResourceId_t>              attachmentTextureViews;
            std::vector<SRHISubpassDescription>    subpassDescriptions;
            std::vector<SRHISubpassDependency>     subpassDependencies;
        };

        struct
            [[nodiscard]]
            SHIRABE_TEST_EXPORT SRHIFrameBufferDescription
        {
        public_members:
            std::string  name;
            ResourceId_t renderPassResourceId;
        };

        struct
            [[nodiscard]]
            SHIRABE_TEST_EXPORT SRHISwapChainBufferDescription
        {
            std::string          name;
            SRHIImageDescription texture;
            uint32_t             backBufferIndex;
        };

        struct
            [[nodiscard]]
            SHIRABE_TEST_EXPORT SRHISwapChainDescription
        {
            std::string          name;
            SRHIImageDescription texture;
            bool                 vsyncEnabled;
            bool                fullscreen;
            unsigned int        windowHandle;
            unsigned int        backBufferCount;
            unsigned int        refreshRateNumerator;
            unsigned int        refreshRateDenominator;
        };

        struct
            [[nodiscard]]
            SHIRABE_TEST_EXPORT SRHIShaderModuleDescriptor
        {
            std::string                                                  name;
            std::unordered_map<VkShaderStageFlags, DataSourceAccessor_t> shaderStages;
        };

        struct
            [[nodiscard]]
            SHIRABE_TEST_EXPORT SRHIPipelineLayoutDescriptor
        {
            std::string name;

            ResourceId_t systemBasePipelineId;
            ResourceId_t shaderModuleId;

            VkPipelineLayoutCreateInfo                             pipelineLayout;
            std::vector<VkDescriptorSetLayoutCreateInfo>           descriptorSetLayoutCreateInfos;
            std::vector<std::vector<VkDescriptorSetLayoutBinding>> descriptorSetLayoutBindings;
        };

        struct
            [[nodiscard]]
            SHIRABE_TEST_EXPORT SRHIPipelineDescriptor
        {
            std::string name;

            ResourceId_t pipelineLayoutResourceId;
            ResourceId_t shaderModuleResourceId;
            ResourceId_t referenceRenderPassId;
            uint32_t     subpass;

            VkViewport   viewPort;
            VkRect2D     scissor;
            bool         includesSystemBuffers;

            VkPipelineInputAssemblyStateCreateInfo           inputAssemblyState;
            std::vector<VkVertexInputBindingDescription>     vertexInputBindings;
            std::vector<VkVertexInputAttributeDescription>   vertexInputAttributes;

            VkPipelineRasterizationStateCreateInfo           rasterizerState;
            VkPipelineMultisampleStateCreateInfo             multiSampler;
            VkPipelineDepthStencilStateCreateInfo            depthStencilState;
            std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachmentStates;
            VkPipelineColorBlendStateCreateInfo              colorBlendState;
        };

        struct
            [[nodiscard]]
            SHIRABE_TEST_EXPORT SRHIDescriptorPoolDescription
        {
            std::string  name;
            ResourceId_t pipelineLayoutResourceId;
        };

        struct
            [[nodiscard]]
            SHIRABE_TEST_EXPORT SRHIMeshInfo
        {
            uint32_t                                  attributeCount;
            Vector<VkVertexInputBindingDescription>   bindingDescriptions;
            Vector<VkVertexInputAttributeDescription> attributeDescriptions;
            Vector<VkDeviceSize>                      offsets;
            VkDeviceSize                              firstIndexOffset;
            uint32_t                                  indexSampleCount;
        };

        /**
         * SRHIMeshDescriptor provides all information necessary in order to create mesh resource
         * and it's attribute and index buffer entries.
         */
        struct
            [[nodiscard]]
            SHIRABE_TEST_EXPORT SRHIMeshDescriptor
        {
            std::string  name;
            SRHIMeshInfo meshInfo;
            ResourceId_t attributeBufferId;
        };

        template<typename TResource, typename TResourceDesc>
        class CResourceCreator
        {
        public:
            static TResourceDesc adapt(Shared<asset::CAssetStorage> aAssetStorage, Shared<TResource> aTextureInstance);
        };
    }
}

#endif //SHIRABEDEVELOPMENT_RESOURCETYPES_H
