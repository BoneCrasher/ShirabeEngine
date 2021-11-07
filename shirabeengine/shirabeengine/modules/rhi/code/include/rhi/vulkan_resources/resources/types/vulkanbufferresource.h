#ifndef __SHIRABE_VULKAN_BUFFER_RESOURCE_H__
#define __SHIRABE_VULKAN_BUFFER_RESOURCE_H__

#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>

#include <base/declaration.h>

#include "rhi/resource_management/resourcetypes.h"
#include "rhi/resource_management/extensibility.h"
#include "rhi/vulkan_core/vulkandevicecapabilities.h"
#include "rhi/vulkan_core/vulkanenvironment.h"
#include "rhi/vulkan_resources/resources/ivkglobalcontext.h"

namespace engine
{
    namespace vulkan
    {
        struct SVulkanRHIBuffer;
    }

    namespace rhi
    {
        template <>
        struct SRHIResourceMap<SRHIBuffer> { using TMappedRHIResource = vulkan::SVulkanRHIBuffer;  };
    }

    namespace vulkan
    {
        using namespace rhi;

        namespace buffer_log
        {
            SHIRABE_DECLARE_LOG_TAG(SVulkanRHIBuffer)
        }

        /**
         *
         * @param aPhysicalDevice
         * @param aLogicalDevice
         * @param aBufferSize
         * @param aBufferUsage
         * @param aBufferMemoryProperties
         * @return
         */
        CEngineResult<std::tuple<VkBuffer, VkMemoryRequirements>>
            __createVkBuffer(  VkPhysicalDevice   const &aPhysicalDevice
                             , VkDevice           const &aLogicalDevice
                             , VkDeviceSize       const &aBufferSize
                             , VkBufferUsageFlags const &aBufferUsage);

        /**
         * The SVulkanRHIImageResource struct describes the relevant data to deal
         * with textures inside the vulkan API.
         */
        struct SVulkanRHIBuffer
        {
            struct Handles_t {
                VkBuffer             buffer;
                VkMemoryRequirements memoryRequirements;
            };

            template <typename TResourceManager>
            static EEngineStatus initialize(SRHIBufferDescription  const &aDescription
                                            , Handles_t                 &aGpuApiHandles
                                            , TResourceManager          *aResourceManager
                                            , IVkGlobalContext          *aVulkanEnvironment);

            template <typename TResourceManager>
            static EEngineStatus deinitialize(SRHIBufferDescription  const &aDescription
                                              , Handles_t                 &aGpuApiHandles
                                              , TResourceManager          *aResourceManager
                                              , IVkGlobalContext          *aVulkanEnvironment);

            template <typename TResourceManager>
            static EEngineStatus bindBufferAndMemory(TResourceManager   *aResourceManager
                                                     , IVkGlobalContext *aVulkanEnvironment
                                                     , VkBuffer          aVkBuffer
                                                     , VkDeviceMemory    aVkMemory
                                                     , VkDeviceSize      aVkMemoryOffset = 0);
        };

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        using RHIBufferResourceState_t = SRHIResourceState<SRHIBuffer>;

        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TResourceManager>
        EEngineStatus SVulkanRHIBuffer::initialize(SRHIBufferDescription  const &aDescription
                                                   , Handles_t                 &aGpuApiHandles
                                                   , TResourceManager          *aResourceManager
                                                   , IVkGlobalContext          *aVulkanEnvironment)
        {
            SHIRABE_UNUSED(aResourceManager);

            IVkGlobalContext *vkContext = aVulkanEnvironment;

            VkDevice         const &vkLogicalDevice  = vkContext->getLogicalDevice();
            VkPhysicalDevice const &vkPhysicalDevice = vkContext->getPhysicalDevice();

            auto const [result, data] = __createVkBuffer(vkPhysicalDevice, vkLogicalDevice, aDescription.createInfo.size, aDescription.createInfo.usage);

            if(CheckEngineError(result))
            {
                CLog::Error(buffer_log::logTag(), CString::format("Failed to create buffer."));
                return EEngineStatus::Error;
            }

            auto const [buffer, requirements] = data;

            aGpuApiHandles.buffer             = buffer;
            aGpuApiHandles.memoryRequirements = requirements;

            // getVkContext()->registerDebugObjectName((uint64_t)this->stagingBuffer,       VK_OBJECT_TYPE_BUFFER,        std::string(aDescription.name) + "_StagingBuffer");

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TResourceManager>
        EEngineStatus SVulkanRHIBuffer::deinitialize(SRHIBufferDescription  const &aDescription
                                                     , Handles_t                 &aGpuApiHandles
                                                     , TResourceManager          *aResourceManager
                                                     , IVkGlobalContext          *aVulkanEnvironment)
        {
            SHIRABE_UNUSED(aDescription);
            SHIRABE_UNUSED(aResourceManager);

            VkBuffer vkBuffer        = aGpuApiHandles.buffer;
            VkDevice vkLogicalDevice = aVulkanEnvironment->getLogicalDevice();
            vkDestroyBuffer(vkLogicalDevice, vkBuffer, nullptr);

            aGpuApiHandles.buffer = VK_NULL_HANDLE;
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TResourceManager>
        EEngineStatus SVulkanRHIBuffer::bindBufferAndMemory(TResourceManager   *aResourceManager
                                                            , IVkGlobalContext *aVulkanEnvironment
                                                            , VkBuffer          aVkBuffer
                                                            , VkDeviceMemory    aVkMemory
                                                            , VkDeviceSize      aVkMemoryOffset)
        {
            VkDevice vkLogicalDevice = aVulkanEnvironment->getLogicalDevice();
            VkResult const result = vkBindBufferMemory(vkLogicalDevice, aVkBuffer, aVkMemory, aVkMemoryOffset);
            if(VkResult::VK_SUCCESS != result)
            {
                CLog::Error(buffer_log::logTag(), CString::format("Failed to bind buffer memory on GPU. Vulkan error: {}", result));
                return EEngineStatus::Error;
            }
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        //template <typename TResourceManager>
        //EEngineStatus SVulkanRHIBuffer::load(SRHIBufferDescription  const &aDescription
        //                                        , Handles_t                 &aGpuApiHandles
        //                                        , TResourceManager          *aResourceManager
        //                                        , IVkGlobalContext          *aVulkanEnvironment)
        //{
        //    SHIRABE_UNUSED(aResourceManager);
//
        //    if(nullptr != aDescription.dataSource)
        //    {
        //        VkDevice device = aVulkanEnvironment->getLogicalDevice();
//
        //        ByteBuffer const dataSource = aDescription.dataSource();
//
        //        void *mappedData = nullptr;
        //        vkMapMemory(device, aGpuApiHandles.attachedMemory, 0, dataSource.size(), 0, &mappedData);
        //        memcpy(mappedData, dataSource.data(), dataSource.size());
        //        vkUnmapMemory(device, aGpuApiHandles.attachedMemory);
        //    }
//
        //    return EEngineStatus::Ok;
        //}
        ////<-----------------------------------------------------------------------------
//
        ////<-----------------------------------------------------------------------------
        ////
        ////<-----------------------------------------------------------------------------
        //template <typename TResourceManager>
        //EEngineStatus SVulkanRHIBuffer::unload(SRHIBufferDescription  const &aDescription
        //                                          , Handles_t                 &aGpuApiHandles
        //                                          , TResourceManager          *aResourceManager
        //                                          , IVkGlobalContext          *aVulkanEnvironment)
        //{
        //    SHIRABE_UNUSED(aDescription);
        //    SHIRABE_UNUSED(aGpuApiHandles);
        //    SHIRABE_UNUSED(aVulkanEnvironment);
//
        //    return { EEngineStatus::Ok };
        //}
        //<-----------------------------------------------------------------------------
    }
}

#endif
