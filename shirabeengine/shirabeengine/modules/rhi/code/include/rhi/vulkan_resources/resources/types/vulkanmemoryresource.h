#ifndef __SHIRABE_VULKAN_MEMORY_RESOURCE_H__
#define __SHIRABE_VULKAN_MEMORY_RESOURCE_H__

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
        struct SVulkanRHIMemoryResource;
    }

    namespace rhi
    {
        template <>
        struct SRHIResourceMap<SRHIMemory> { using TMappedRHIResource = vulkan::SVulkanRHIMemoryResource;  };
    }

    namespace vulkan
    {
        using namespace rhi;

        namespace memory_log
        {
            SHIRABE_DECLARE_LOG_TAG(SVulkanMemoryResource)
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
        CEngineResult<VkDeviceMemory> __createVkMemory(  VkPhysicalDevice      const &aPhysicalDevice
                                                       , VkDevice              const &aLogicalDevice
                                                       , VkMemoryRequirements  const &aMemoryRequirements
                                                       , VkMemoryPropertyFlags const &aMemoryProperties);

        /**
         * The SVulkanRHIImageResource struct describes the relevant data to deal
         * with textures inside the vulkan API.
         */
        struct SVulkanRHIMemoryResource
        {
            struct Handles_t {
                VkDeviceMemory        handle;
                VkMemoryRequirements  requirements;
                VkMemoryPropertyFlags properties;
            };

            template <typename TResourceManager>
            static EEngineStatus initialize(SRHIMemoryDescription const &aDescription
                                            , Handles_t                 &aGpuApiHandles
                                            , TResourceManager          *aResourceManager
                                            , IVkGlobalContext          *aVulkanEnvironment);

            template <typename TResourceManager>
            static EEngineStatus deinitialize(SRHIMemoryDescription const &aDescription
                                              , Handles_t                 &aGpuApiHandles
                                              , TResourceManager          *aResourceManager
                                              , IVkGlobalContext          *aVulkanEnvironment);
        };

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        using RHIMemoryResourceState_t = SRHIResourceState<SRHIMemory>;

        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TResourceManager>
        EEngineStatus SVulkanRHIMemoryResource::initialize(SRHIMemoryDescription const &aDescription
                                                           , Handles_t                &aGpuApiHandles
                                                           , TResourceManager         *aResourceManager
                                                           , IVkGlobalContext         *aVulkanEnvironment)
        {
            SHIRABE_UNUSED(aResourceManager);

            IVkGlobalContext *vkContext = aVulkanEnvironment;

            VkDevice         const &vkLogicalDevice  = vkContext->getLogicalDevice();
            VkPhysicalDevice const &vkPhysicalDevice = vkContext->getPhysicalDevice();

            auto const [result, memory] = __createVkMemory(vkPhysicalDevice, vkLogicalDevice, aDescription.memoryRequirements, aDescription.memoryProperties);
            if(CheckEngineError(result))
            {
                CLog::Error(memory_log::logTag(), StaticStringHelpers::format("Failed to allocate buffer memory on GPU. Vulkan error: {}", result));
                return EEngineStatus::Error;
            }

            aGpuApiHandles.handle       = memory;
            aGpuApiHandles.requirements = aDescription.memoryRequirements;
            aGpuApiHandles.properties   = aDescription.memoryProperties;

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TResourceManager>
        EEngineStatus SVulkanRHIMemoryResource::deinitialize(SRHIMemoryDescription const &aDescription
                                                             , Handles_t                &aGpuApiHandles
                                                             , TResourceManager         *aResourceManager
                                                             , IVkGlobalContext         *aVulkanEnvironment)
        {
            SHIRABE_UNUSED(aDescription);
            SHIRABE_UNUSED(aResourceManager);

            VkDeviceMemory vkDeviceMemory  = aGpuApiHandles.handle;
            VkDevice       vkLogicalDevice = aVulkanEnvironment->getLogicalDevice();

            vkFreeMemory(vkLogicalDevice, vkDeviceMemory, nullptr);

            aGpuApiHandles.handle       = VK_NULL_HANDLE;
            aGpuApiHandles.requirements = {};
            aGpuApiHandles.properties   = {};

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------
    }
}

#endif
