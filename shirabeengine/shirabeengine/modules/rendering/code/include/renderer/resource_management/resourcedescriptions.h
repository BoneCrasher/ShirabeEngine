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

#include "renderer/resource_management/resourcedatasource.h"

namespace engine
{
    namespace resources
    {
        using namespace graphicsapi;

        using ResourceId_t = std::string;

        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT SBufferDescription
        {
            std::string          name;
            VkBufferCreateInfo   createInfo;
            DataSourceAccessor_t dataSource;
        };

        struct
            [[nodiscard]]
            SHIRABE_TEST_EXPORT SBufferViewDescription
        {
            std::string            name;
            ResourceId_t           subjacentBufferId;
            VkBufferViewCreateInfo createInfo;
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
            DataSourceAccessor_t             initialData;
        };

        struct
            [[nodiscard]]
            SHIRABE_TEST_EXPORT STextureViewDescription
        {
        public_members:
            std::string   name;
            ResourceId_t  subjacentTextureId;
            EFormat       textureFormat;
            ArraySlices_t arraySlices;
            MipSlices_t   mipMapSlices;
            // TODO: Distinguish binding and read/write mode
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
            std::vector<ResourceId_t>           attachmentTextureViews;
            std::vector<SSubpassDescription>    subpassDescriptions;
            std::vector<SSubpassDependency>     subpassDependencies;
        };

        struct
            [[nodiscard]]
            SHIRABE_TEST_EXPORT SFrameBufferDescription
        {
        public_members:
            std::string  name;
            ResourceId_t renderPassResourceId;
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
            SHIRABE_TEST_EXPORT SMaterialBasePipelineDescriptor
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
            SHIRABE_TEST_EXPORT SMaterialPipelineDescriptor
        {
            std::string name;

            ResourceId_t sharedMaterialId;
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
            SHIRABE_TEST_EXPORT SMaterialDescription
        {
            std::string  name;
            ResourceId_t systemMaterialId;
            ResourceId_t basePipelineId;
            ResourceId_t shaderModuleId;

            Vector<ResourceId_t> bufferIds;
            Vector<ResourceId_t> sampledImageIds;
        };

        struct
            [[nodiscard]]
            SHIRABE_TEST_EXPORT SMeshInfo
        {
            uint32_t                                  attributeCount;
            Vector<VkVertexInputBindingDescription>   bindingDescriptions;
            Vector<VkVertexInputAttributeDescription> attributeDescriptions;
            Vector<VkDeviceSize>                      offsets;
            VkDeviceSize                              firstIndexOffset;
            uint32_t                                  indexSampleCount;
        };

        /**
         * SMeshDescriptor provides all information necessary in order to create mesh resource
         * and it's attribute and index buffer entries.
         */
        struct
            [[nodiscard]]
            SHIRABE_TEST_EXPORT SMeshDescriptor
        {
            std::string  name;
            SMeshInfo    meshInfo;
            ResourceId_t attributeBufferId;
        };

        template<typename TResource, typename TResourceDesc>
        class CResourceAdapter
        {
        public:
            static TResourceDesc adapt(Shared<asset::CAssetStorage> aAssetStorage, Shared<TResource> aTextureInstance);
        };
    }
}

#endif //SHIRABEDEVELOPMENT_RESOURCETYPES_H
