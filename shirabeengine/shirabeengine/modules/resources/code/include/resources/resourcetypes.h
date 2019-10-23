//
// Created by dottideveloper on 20.10.19.
//

#ifndef SHIRABEDEVELOPMENT_RESOURCETYPES_H
#define SHIRABEDEVELOPMENT_RESOURCETYPES_H

#include <vector>
#include <vulkan/vulkan.h>
#include <core/enginetypehelper.h>
#include <core/databuffer.h>
#include <core/bitfield.h>
#include <graphicsapi/definitions.h>
#include "resources/resourcedatasource.h"

namespace engine
{
    namespace resources
    {
        using namespace graphicsapi;

        struct
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT SBufferDescription
        {
            std::string                       name;
            VkBufferCreateInfo                createInfo;
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
            SHIRABE_TEST_EXPORT STextureDescription
        {
        public_members:
            std::string                      name;
            STextureInfo                     textureInfo;
            EResourceUsage                   cpuGpuUsage;
            core::CBitField<EBufferBinding>  gpuBinding;
            Vector<CResourceDataSource>      initialData;
        };

        struct
            [[nodiscard]]
            SHIRABE_TEST_EXPORT STextureViewDescription
        {
        public_members:
            std::string   name;
            STextureInfo  subjacentTexture;
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

        struct SAttachmentDescription
        {
            EFormat            format;
            EAttachmentLoadOp  loadOp;
            EAttachmentStoreOp storeOp;
            EAttachmentLoadOp  stencilLoadOp;
            EAttachmentStoreOp stencilStoreOp;
            EImageLayout       initialLayout;
            EImageLayout       finalLayout;
        };

        struct SAttachmentReference
        {
            uint32_t     attachment;
            EImageLayout layout;
        };

        struct SSubpassDescription
        {
            std::vector<SAttachmentReference> inputAttachments;
            std::vector<SAttachmentReference> colorAttachments;
            std::vector<SAttachmentReference> resolveAttachments;
            std::vector<SAttachmentReference> depthStencilAttachments;
            std::vector<uint32_t>             preserveStencilAttachments;
        };

        struct SHIRABE_TEST_EXPORT SRenderPassDescription
        {
        public_members:
            std::string                         name;
            std::vector<SAttachmentDescription> attachmentDescriptions;
            std::vector<SSubpassDescription>    subpassDescriptions;
        };
        struct
            [[nodiscard]]
            SHIRABE_TEST_EXPORT SFrameBufferDescription
        {

        public_members:
            std::string name;
            uint32_t    width;
            uint32_t    height;
            uint32_t    layers;
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

        class
            [[nodiscard]]
            SHIRABE_TEST_EXPORT SPipelineDescription
        {
            std::string                                                  name;
            VkViewport                                                   viewPort;
            VkRect2D                                                     scissor;

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

            std::unordered_map<VkShaderStageFlags, DataSourceAccessor_t> shaderStages;

            uint32_t                                                     subpass;
        };

    }
}

#endif //SHIRABEDEVELOPMENT_RESOURCETYPES_H
