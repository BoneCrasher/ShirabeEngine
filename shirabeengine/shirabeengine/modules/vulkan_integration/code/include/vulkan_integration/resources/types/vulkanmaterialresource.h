#ifndef __SHIRABE_VULKAN_MATERIAL_RESOURCE_H__
#define __SHIRABE_VULKAN_MATERIAL_RESOURCE_H__

#include <vector>

#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>

#include <resources/resourcetypes.h>
#include <resources/extensibility.h>

#include <base/declaration.h>

namespace engine
{
    namespace vulkan
    {
        struct SVulkanMaterialResource;
    }

    namespace resources
    {
        template <> struct SLogicalToGpuApiResourceTypeMap<SMaterial> { using TGpuApiResource = vulkan::SVulkanMaterialResource;  };
    }

    namespace vulkan
    {
        /**
         * The SVulkanTextureResource struct describes the relevant data to deal
         * with textures inside the vulkan API.
         */
        struct SVulkanMaterialResource
        {
            static constexpr bool is_loadable      = true;
            static constexpr bool is_unloadable    = true;
            static constexpr bool is_transferrable = true;

            struct Handles_t
            {
                VkDescriptorPool             descriptorPool;
                std::vector<VkDescriptorSet> descriptorSets;
            };

            template <typename TResourceManager>
            static EEngineStatus initialize(SMaterialDescription  const &aDescription
                                            , Handles_t                 &aGpuApiHandles
                                            , TResourceManager          *aResourceManager
                                            , IVkGlobalContext          *aVulkanEnvironment);

            template <typename TResourceManager>
            static EEngineStatus load(SMaterialDescription  const &aDescription
                                      , Handles_t                 &aGpuApiHandles
                                      , TResourceManager          *aResourceManager
                                      , IVkGlobalContext          *aVulkanEnvironment);

            template <typename TResourceManager>
            static EEngineStatus transfer(SMaterialDescription  const &aDescription
                                          , Handles_t                 &aGpuApiHandles
                                          , TResourceManager          *aResourceManager
                                          , IVkGlobalContext          *aVulkanEnvironment);

            template <typename TResourceManager>
            static EEngineStatus unload(SMaterialDescription  const &aDescription
                                        , Handles_t                 &aGpuApiHandles
                                        , TResourceManager          *aResourceManager
                                        , IVkGlobalContext          *aVulkanEnvironment);

            template <typename TResourceManager>
            static EEngineStatus deinitialize(SMaterialDescription  const &aDescription
                                              , Handles_t                 &aGpuApiHandles
                                              , TResourceManager          *aResourceManager
                                              , IVkGlobalContext          *aVulkanEnvironment);
        };

        using MaterialResourceState_t = SResourceState<SMaterial>;

        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        auto updateDescriptorSets(Shared<CVulkanEnvironment>                                      aVulkanEnvironment
                                  , Shared<CResourceManager>                                      aResourceManager
                                  , Shared<asset::CAssetStorage>                                  aAssetStorage
                                  , SFrameGraphRenderContextState                          const &aState
                                  , MaterialResourceState_t                                const &aMaterialHandle
                                  , std::vector<OptionalRef_t<BufferResourceState_t>>      const &aUniformBufferStates
                                  , std::vector<OptionalRef_t<TextureViewResourceState_t>> const &aInputAttachmentStates
                                  , std::vector<SSampledImageBinding>                      const &aTextureViewStates) -> EEngineStatus
        {
            VkDevice      device = aVulkanEnvironment->getLogicalDevice();
            SVulkanState &state  = aVulkanEnvironment->getState();

            VkCommandBuffer commandBuffer  = aVulkanEnvironment->getVkCurrentFrameContext()->getGraphicsCommandBuffer();

            auto const [success, resource] = fetchResource<PipelineResourceState_t>(aMaterialHandle.description.pipelineDescriptor.name, aResourceManager, aVulkanEnvironment);
            if(not success)
            {
                return EEngineStatus::Error;
            }

            PipelineResourceState_t &pipeline = *resource;

            SMaterialPipelineDescriptor const &pipelineDescriptor = pipeline.description;

            std::vector<VkWriteDescriptorSet>   descriptorSetWrites {};
            std::vector<VkDescriptorBufferInfo> descriptorSetWriteBufferInfos {};
            std::vector<VkDescriptorImageInfo>  descriptorSetWriteAttachmentImageInfos {};
            std::vector<VkDescriptorImageInfo>  descriptorSetWriteImageInfos {};

            descriptorSetWriteBufferInfos         .resize(aUniformBufferStates.size());
            descriptorSetWriteAttachmentImageInfos.resize(aInputAttachmentStates.size());
            descriptorSetWriteImageInfos          .resize(aTextureViewStates.size());

            uint64_t        writeCounter           = 0;
            uint64_t        bufferCounter          = 0;
            uint64_t        inputAttachmentCounter = 0;
            uint64_t        inputImageCounter      = 0;
            uint64_t const startSetIndex = (pipelineDescriptor.includesSystemBuffers ? 0 : 2); // Set 0 and 1 are system buffers...

            for(std::size_t k=0; k<pipelineDescriptor.descriptorSetLayoutBindings.size(); ++k)
            {
                std::vector<VkDescriptorSetLayoutBinding> const setBindings  = pipelineDescriptor.descriptorSetLayoutBindings[k];
                for(std::size_t j=0; j<setBindings.size(); ++j)
                {
                    VkDescriptorSetLayoutBinding const binding = setBindings[j];

                    switch(binding.descriptorType)
                    {
                    case VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
                        {
                            BufferResourceState_t buffer = *(aUniformBufferStates[bufferCounter]);

                            VkDescriptorBufferInfo bufferInfo = {};
                            bufferInfo.buffer = buffer.gpuApiHandles.handle;
                            bufferInfo.offset = 0;
                            bufferInfo.range  = buffer.description.createInfo.size;
                            descriptorSetWriteBufferInfos[bufferCounter] = bufferInfo;

                            VkWriteDescriptorSet descriptorWrite = {};
                            descriptorWrite.sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                            descriptorWrite.pNext            = nullptr;
                            descriptorWrite.descriptorType   = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                            descriptorWrite.dstSet           = aMaterialHandle.gpuApiHandles.descriptorSets[startSetIndex + k];
                            descriptorWrite.dstBinding       = binding.binding;
                            descriptorWrite.dstArrayElement  = 0;
                            descriptorWrite.descriptorCount  = 1; // We only update one descriptor, i.e. pBufferInfo.count;
                            descriptorWrite.pBufferInfo      = &(descriptorSetWriteBufferInfos[bufferCounter++]);
                            descriptorWrite.pImageInfo       = nullptr; // Optional
                            descriptorWrite.pTexelBufferView = nullptr;

                            descriptorSetWrites.push_back(descriptorWrite);
                            //descriptorSetWrites[writeCounter++] = descriptorWrite;
                        }
                        break;
                    case VkDescriptorType::VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:
                        {
                            TextureViewResourceState_t &textureView = *(aInputAttachmentStates[inputAttachmentCounter]);

                            VkDescriptorImageInfo imageInfo {};
                            imageInfo.imageView   = textureView.gpuApiHandles.handle;
                            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                            imageInfo.sampler     = VK_NULL_HANDLE;
                            descriptorSetWriteAttachmentImageInfos[inputAttachmentCounter] = imageInfo;

                            VkWriteDescriptorSet descriptorWrite = {};
                            descriptorWrite.sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                            descriptorWrite.pNext            = nullptr;
                            descriptorWrite.descriptorType   = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
                            descriptorWrite.dstSet           = aMaterialHandle.gpuApiHandles.descriptorSets[startSetIndex + k];
                            descriptorWrite.dstBinding       = binding.binding;
                            descriptorWrite.dstArrayElement  = 0;
                            descriptorWrite.descriptorCount  = 1; // We only update one descriptor, i.e. pBufferInfo.count;
                            descriptorWrite.pBufferInfo      = nullptr;
                            descriptorWrite.pImageInfo       = &(descriptorSetWriteAttachmentImageInfos[inputAttachmentCounter++]); // Optional
                            descriptorWrite.pTexelBufferView = nullptr;
                            // descriptorSetWrites[writeCounter++] = descriptorWrite;
                            descriptorSetWrites.push_back(descriptorWrite);
                        }
                        break;
                    case VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
                        {
                            if(aTextureViewStates.size() <= inputImageCounter)
                            {
                                continue;
                            }

                            auto const &imageBinding = aTextureViewStates[inputImageCounter];

                            if(not (imageBinding.image.has_value() && imageBinding.imageView.has_value()))
                            {
                                continue;
                            }

                            TextureViewResourceState_t &textureView = *(imageBinding.imageView);
                            TextureResourceState_t     &texture     = *(imageBinding.image);

                            VkDescriptorImageInfo imageInfo {};
                            imageInfo.imageView   = textureView.gpuApiHandles.handle;
                            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                            imageInfo.sampler     = texture.gpuApiHandles.attachedSampler;
                            descriptorSetWriteImageInfos[inputImageCounter] = imageInfo;

                            VkWriteDescriptorSet descriptorWrite = {};
                            descriptorWrite.sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                            descriptorWrite.pNext            = nullptr;
                            descriptorWrite.descriptorType   = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                            descriptorWrite.dstSet           = aMaterialHandle.gpuApiHandles.descriptorSets[startSetIndex + k];
                            descriptorWrite.dstBinding       = binding.binding;
                            descriptorWrite.dstArrayElement  = 0;
                            descriptorWrite.descriptorCount  = 1; // We only update one descriptor, i.e. pBufferInfo.count;
                            descriptorWrite.pBufferInfo      = nullptr;
                            descriptorWrite.pImageInfo       = &(descriptorSetWriteImageInfos[inputImageCounter++]); // Optional
                            descriptorWrite.pTexelBufferView = nullptr;
                            // descriptorSetWrites[writeCounter++] = descriptorWrite;
                            descriptorSetWrites.push_back(descriptorWrite);
                        }
                        break;
                    default:
                        break;
                    }
                }
            }

            vkUpdateDescriptorSets(device, descriptorSetWrites.size(), descriptorSetWrites.data(), 0, nullptr);
            return EEngineStatus::Ok;
        }

        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TResourceManager>
        EEngineStatus SVulkanMaterialResource::initialize(SMaterialDescription  const &aDescription
                                                          , Handles_t                 &aGpuApiHandles
                                                          , TResourceManager          *aResourceManager
                                                          , IVkGlobalContext          *aVulkanEnvironment)
        {
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TResourceManager>
        EEngineStatus SVulkanMaterialResource::load(SMaterialDescription const &aDescription
                                                   , Handles_t                 &aGpuApiHandles
                                                   , TResourceManager          *aResourceManager
                                                   , IVkGlobalContext          *aVulkanEnvironment)
        {
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TResourceManager>
        EEngineStatus SVulkanMaterialResource::transfer(SMaterialDescription const &aDescription
                                                       , Handles_t                 &aGpuApiHandles
                                                       , TResourceManager          *aResourceManager
                                                       , IVkGlobalContext          *aVulkanEnvironment)
        {
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TResourceManager>
        EEngineStatus SVulkanMaterialResource::unload(SMaterialDescription const &aDescription
                                                     , Handles_t                 &aGpuApiHandles
                                                     , TResourceManager          *aResourceManager
                                                     , IVkGlobalContext          *aVulkanEnvironment)
        {
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TResourceManager>
        EEngineStatus SVulkanMaterialResource::deinitialize(SMaterialDescription const &aDescription
                                                           , Handles_t                 &aGpuApiHandles
                                                           , TResourceManager          *aResourceManager
                                                           , IVkGlobalContext          *aVulkanEnvironment)
        {

        }
    }
}

#endif
