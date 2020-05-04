#ifndef __SHIRABE_VULKAN_MATERIAL_RESOURCE_H__
#define __SHIRABE_VULKAN_MATERIAL_RESOURCE_H__

#include <vector>

#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>

#include <resources/resourcetypes.h>
#include <resources/extensibility.h>
#include <resources/cresourcemanager.h>

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

    namespace material_log
    {
        SHIRABE_DECLARE_LOG_TAG(SVulkanMaterialResource);
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
        struct SSampledImageBinding
        {
            OptionalRef_t<TextureViewResourceState_t> imageView;
            OptionalRef_t<TextureResourceState_t>     image;
        };
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        auto updateDescriptorSets(Shared<CVulkanEnvironment>                                      aVulkanEnvironment
                                  , Shared<CResourceManager>                                      aResourceManager
                                  , MaterialResourceState_t                                const &aMaterialHandle
                                  , std::vector<OptionalRef_t<BufferResourceState_t>>      const &aUniformBufferStates
                                  , std::vector<OptionalRef_t<TextureViewResourceState_t>> const &aInputAttachmentStates
                                  , std::vector<SSampledImageBinding>                      const &aTextureViewStates) -> EEngineStatus
        {
            VkDevice      device = aVulkanEnvironment->getLogicalDevice();
            SVulkanState &state  = aVulkanEnvironment->getState();

            auto const [success, resource] = aResourceManager->getResource<PipelineResourceState_t>(aMaterialHandle.description.basePipelineDescriptor.name, aVulkanEnvironment);
            if(CheckEngineError(success))
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
            VkDevice device = aVulkanEnvironment->getLogicalDevice();

            auto const findTypesAndSizes =
                           [&device] (SMaterialPipelineDescriptor const &aDescriptor) -> std::unordered_map<VkDescriptorType, uint32_t>
                               {
                                   std::unordered_map<VkDescriptorType, uint32_t> pipelineLayoutDescriptorTypesAndSizes;
                                   for(uint64_t k=0; k<aDescriptor.descriptorSetLayoutCreateInfos.size(); ++k)
                                   {
                                       std::vector<VkDescriptorSetLayoutBinding> bindings = aDescriptor.descriptorSetLayoutBindings[k];
                                       for( auto const &binding : bindings )
                                       {
                                           pipelineLayoutDescriptorTypesAndSizes[binding.descriptorType] += binding.descriptorCount;
                                       }
                                   }
                                   return pipelineLayoutDescriptorTypesAndSizes;
                               };

            std::vector<VkDescriptorPoolSize> poolSizes {};
            auto const addPoolSizeFn =
                           [&poolSizes] (VkDescriptorType const &aType, std::size_t const &aSize)
                               {
                                   VkDescriptorPoolSize poolSize = {};
                                   poolSize.type            = aType;
                                   poolSize.descriptorCount = static_cast<uint32_t>(aSize);

                                   poolSizes.push_back(poolSize);
                               };

            auto const [success, resource] = aResourceManager->template getResource<PipelineResourceState_t>(aDescription.basePipelineDescriptor.name, aVulkanEnvironment);
            if(CheckEngineError(success))
            {
                return EEngineStatus::Error;
            }
            PipelineResourceState_t &pipeline = *resource;

            for(auto const &[type, size] : findTypesAndSizes(aDescription))
            {
                addPoolSizeFn(type, size);
            }

            VkDeviceSize const descriptorSetCount = pipeline.gpuApiHandles.descriptorSetLayouts.size();

            VkDescriptorPoolCreateInfo vkDescriptorPoolCreateInfo = {};
            vkDescriptorPoolCreateInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            vkDescriptorPoolCreateInfo.pNext         = nullptr;
            vkDescriptorPoolCreateInfo.flags         = 0;
            vkDescriptorPoolCreateInfo.maxSets       = descriptorSetCount;
            vkDescriptorPoolCreateInfo.poolSizeCount = poolSizes.size();
            vkDescriptorPoolCreateInfo.pPoolSizes    = poolSizes.data();

            VkDescriptorPool vkDescriptorPool = VK_NULL_HANDLE;
            {
                VkResult const result = vkCreateDescriptorPool(device, &vkDescriptorPoolCreateInfo, nullptr, &vkDescriptorPool);
                if(VkResult::VK_SUCCESS != result)
                {
                    CLog::Error("SVulkanMaterialPipelineResource::initialize", "Could not create descriptor pool.");
                    return { EEngineStatus::Error };
                }
            }

            VkDescriptorSetAllocateInfo vkDescriptorSetAllocateInfo = {};
            vkDescriptorSetAllocateInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            vkDescriptorSetAllocateInfo.pNext              = nullptr;
            vkDescriptorSetAllocateInfo.descriptorPool     = vkDescriptorPool;
            vkDescriptorSetAllocateInfo.descriptorSetCount = descriptorSetCount;
            vkDescriptorSetAllocateInfo.pSetLayouts        = pipeline.gpuApiHandles.descriptorSetLayouts.data();

            std::vector<VkDescriptorSet> vkDescriptorSets {};
            {
                vkDescriptorSets.resize(descriptorSetCount);

                VkResult const result = vkAllocateDescriptorSets(device, &vkDescriptorSetAllocateInfo, vkDescriptorSets.data());
                if(VkResult::VK_SUCCESS != result)
                {
                    CLog::Error("SVulkanMaterialPipelineResource::initialize", "Could not create descriptor sets.");
                    return { EEngineStatus::Error };
                }
            }

            aGpuApiHandles.descriptorPool = vkDescriptorPool;
            aGpuApiHandles.descriptorSets = vkDescriptorSets;
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
            VkDevice device = aVulkanEnvironment->getLogicalDevice();

            auto [success, resource] = fetchResource<MaterialResourceState_t>(aResourceManager, aDescription.name);
            if(not success)
            {
                return EEngineStatus::Ok;
            }
            MaterialResourceState_t &material = *resource;

            for(auto const &bufferDesc : material.description.uniformBufferDescriptors)
            {
                auto [success, resource] = aResourceManager->template getResource<BufferResourceState_t>(bufferDesc.name, aVulkanEnvironment);
                if(not success)
                {
                    return EEngineStatus::Ok;
                }
                BufferResourceState_t &buffer = *resource;

                CEngineResult<> updateResult = aResourceManager->updateResource(buffer, aVulkanEnvironment);
                EngineStatusPrintOnError(updateResult.result(), material_log::logTag(), "Failed to update buffer.");
                SHIRABE_RETURN_RESULT_ON_ERROR(updateResult.result());
            }
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
            VkDevice device = aVulkanEnvironment->getLogicalDevice();

            auto [success, resource] = fetchResource<MaterialResourceState_t>(aResourceManager, aDescription.name);
            if(not success)
            {
                return EEngineStatus::Ok;
            }
            MaterialResourceState_t &material = *resource;

            for(auto const &bufferDesc : material.description.uniformBufferDescriptors)
            {
                auto [success, resource] = aResourceManager->template getResource<BufferResourceState_t>(bufferDesc.name, aVulkanEnvironment);
                if(not success)
                {
                    return EEngineStatus::Ok;
                }
                BufferResourceState_t &buffer = *resource;

                CEngineResult<> transferResult = aResourceManager->transferResource(buffer, aVulkanEnvironment);
                EngineStatusPrintOnError(transferResult.result(), material_log::logTag(), "Failed to transfer buffer.");
                SHIRABE_RETURN_RESULT_ON_ERROR(transferResult.result());
            }
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
            return EEngineStatus::Ok;
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
            VkDevice device = aVulkanEnvironment->getLogicalDevice();

            auto [success, resource] = aResourceManager->template getResource<MaterialResourceState_t>(aDescription.name, aVulkanEnvironment);
            if(not success)
            {
                return EEngineStatus::Ok;
            }
            MaterialResourceState_t &material = *resource;

            for(auto const &bufferDesc : material.description.uniformBufferDescriptors)
            {
                auto [success, resource] = aResourceManager->template getResource<BufferResourceState_t>(bufferDesc.name, aVulkanEnvironment);
                if(not success)
                {
                    return EEngineStatus::Ok;
                }
                BufferResourceState_t &buffer = *resource;

                CEngineResult<> deinitializationResult = aResourceManager->transferResource(buffer, aVulkanEnvironment);
                EngineStatusPrintOnError(deinitializationResult.result(), material_log::logTag(), "Failed to deinitialize buffer.");
                SHIRABE_RETURN_RESULT_ON_ERROR(deinitializationResult.result());
            }

            vkDestroyDescriptorPool(device, aGpuApiHandles.descriptorPool, nullptr);
        }
    }
}

#endif
