#ifndef __SHIRABE_VULKAN_MEMORY_RESOURCE_H__
#define __SHIRABE_VULKAN_MEMORY_RESOURCE_H__

#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>

#include <base/declaration.h>

#include "renderer/resource_management/resourcetypes.h"
#include "renderer/resource_management/extensibility.h"
#include "renderer/vulkan_core/vulkandevicecapabilities.h"
#include "renderer/vulkan_core/vulkanenvironment.h"
#include "renderer/vulkan_resources/resources/ivkglobalcontext.h"

namespace engine
{
    namespace vulkan
    {
        struct SVulkanMemoryResource;
    }

    namespace resources
    {
        template <>
        struct SLogicalToGpuApiResourceTypeMap<SMemory> { using TGpuApiResource = vulkan::SVulkanMemoryResource;  };
    }

    namespace vulkan
    {
        using namespace resources;

        namespace buffer_log
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
         * The SVulkanTextureResource struct describes the relevant data to deal
         * with textures inside the vulkan API.
         */
        struct SVulkanMemoryResource
        {
            static constexpr bool is_loadable      = false;
            static constexpr bool is_unloadable    = false;
            static constexpr bool is_transferrable = false;

            struct Handles_t {
                VkDeviceMemory        handle;
                VkMemoryRequirements  requirements;
                VkMemoryPropertyFlags properties;
            };

            template <typename TResourceManager>
            static EEngineStatus initialize(SMemoryDescription const &aDescription
                                          , Handles_t                &aGpuApiHandles
                                          , TResourceManager         *aResourceManager
                                          , IVkGlobalContext         *aVulkanEnvironment);

            template <typename TResourceManager>
            static EEngineStatus deinitialize(SMemoryDescription const &aDescription
                                            , Handles_t                &aGpuApiHandles
                                            , TResourceManager         *aResourceManager
                                            , IVkGlobalContext         *aVulkanEnvironment);
        };

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        using BufferResourceState_t = SResourceState<SBuffer>;

        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TResourceManager>
        EEngineStatus SVulkanMemoryResource::initialize(SMemoryDescription const &aDescription
                                                      , Handles_t                &aGpuApiHandles
                                                      , TResourceManager         *aResourceManager
                                                      , IVkGlobalContext         *aVulkanEnvironment)
        {
            IVkGlobalContext *vkContext = aVulkanEnvironment;

            VkDevice         const &vkLogicalDevice  = vkContext->getLogicalDevice();
            VkPhysicalDevice const &vkPhysicalDevice = vkContext->getPhysicalDevice();

            auto const [result, memory] = __createVkMemory(vkPhysicalDevice, vkLogicalDevice, aDescription.memoryRequirements, aDescription.memoryProperties);
            if(CheckEngineError(result))
            {
                CLog::Error(buffer_log::logTag(), CString::format("Failed to allocate buffer memory on GPU. Vulkan error: {}", result));
                return { EEngineStatus::Error };
            }

            aGpuApiHandles.handle       = memory;
            aGpuApiHandles.requirements = aDescription.memoryRequirements;
            aGpuApiHandles.properties   = aDescription.memoryProperties;

            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TResourceManager>
        EEngineStatus SVulkanMemoryResource::deinitialize(SMemoryDescription const &aDescription
                                                        , Handles_t                &aGpuApiHandles
                                                        , TResourceManager         *aResourceManager
                                                        , IVkGlobalContext         *aVulkanEnvironment)
        {
            VkDeviceMemory vkDeviceMemory  = aGpuApiHandles.handle;
            VkDevice       vkLogicalDevice = aVulkanEnvironment->getLogicalDevice();

            vkFreeMemory(vkLogicalDevice, vkDeviceMemory, nullptr);

            aGpuApiHandles.handle       = VK_NULL_HANDLE;
            aGpuApiHandles.requirements = {};
            aGpuApiHandles.properties   = {};

            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------
    }
}

#endif
