#ifndef __SHIRABE_VULKAN_BUFFER_RESOURCE_H__
#define __SHIRABE_VULKAN_BUFFER_RESOURCE_H__

#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>

#include <base/declaration.h>
#include <resources/resourcetypes.h>
#include <resources/extensibility.h>

#include "vulkan_integration/vulkandevicecapabilities.h"
#include "vulkan_integration/vulkanenvironment.h"
#include "vulkan_integration/resources/ivkglobalcontext.h"

namespace engine
{
    namespace vulkan
    {
        struct SVulkanBufferResource;
    }

    namespace resources
    {
        template <>
        struct SLogicalToGpuApiResourceTypeMap<SBuffer> { using TGpuApiResource = struct vulkan::SVulkanBufferResource;  };
    }

    namespace vulkan
    {
        using namespace resources;

        namespace buffer_log
        {
            SHIRABE_DECLARE_LOG_TAG(SVulkanBufferResource);
        }

        struct
             [[nodiscard]]
             SHIRABE_TEST_EXPORT SVulkanBufferCreationResult
        {
            VkBuffer       buffer;
            VkDeviceMemory attachedMemory;
        };

        /**
         *
         * @param aPhysicalDevice
         * @param aLogicalDevice
         * @param aBufferSize
         * @param aBufferUsage
         * @param aBufferMemoryProperties
         * @return
         */
        CEngineResult<SVulkanBufferCreationResult> __createVkBuffer(  VkPhysicalDevice      const &aPhysicalDevice
                                                                    , VkDevice              const &aLogicalDevice
                                                                    , VkDeviceSize          const &aBufferSize
                                                                    , VkBufferUsageFlags    const &aBufferUsage
                                                                    , VkMemoryPropertyFlags const &aBufferMemoryProperties);

        /**
         * The SVulkanTextureResource struct describes the relevant data to deal
         * with textures inside the vulkan API.
         */
        struct SVulkanBufferResource
        {
            struct Handles_t {
                VkBuffer       handle;
                VkDeviceMemory attachedMemory;
            };

            template <typename TResourceManager>
            static EEngineStatus initialize(SBufferDescription  const &aDescription
                                          , Handles_t                 &aGpuApiHandles
                                          , IVkGlobalContext          *aVulkanEnvironment
                                          , TResourceManager          *aResourceManager);

            template <typename TResourceManager>
            static EEngineStatus load(SBufferDescription  const &aDescription
                                    , Handles_t                 &aGpuApiHandles
                                    , IVkGlobalContext          *aVulkanEnvironment
                                    , TResourceManager          *aResourceManager);

            template <typename TResourceManager>
            static EEngineStatus unload(SBufferDescription  const &aDescription
                                      , Handles_t                 &aGpuApiHandles
                                      , IVkGlobalContext          *aVulkanEnvironment
                                      , TResourceManager          *aResourceManager);

            template <typename TResourceManager>
            static EEngineStatus deinitialize(SBufferDescription  const &aDescription
                                            , Handles_t                 &aGpuApiHandles
                                            , IVkGlobalContext          *aVulkanEnvironment
                                            , TResourceManager          *aResourceManager);
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
        EEngineStatus SVulkanBufferResource::initialize(SBufferDescription  const &aDescription
                                                      , Handles_t                 &aGpuApiHandles
                                                      , IVkGlobalContext          *aVulkanEnvironment
                                                      , TResourceManager          *aResourceManager)
        {
            IVkGlobalContext *vkContext = aVulkanEnvironment;

            VkDevice         const &vkLogicalDevice  = vkContext->getLogicalDevice();
            VkPhysicalDevice const &vkPhysicalDevice = vkContext->getPhysicalDevice();

            VkBufferCreateInfo createInfo = aDescription.createInfo;
            createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            // createInfo.queueFamilyIndexCount = ...;
            // createInfo.pQueueFamilyIndices   = ...;

            VkBuffer buffer = VK_NULL_HANDLE;

            VkResult result = vkCreateBuffer(vkLogicalDevice, &createInfo, nullptr, &buffer);
            if(VkResult::VK_SUCCESS != result)
            {
                CLog::Error(buffer_log::logTag(), CString::format("Failed to create buffer. Vulkan result: {}", result));
                return { EEngineStatus::Error };
            }

            VkMemoryRequirements vkMemoryRequirements ={ };
            vkGetBufferMemoryRequirements(vkLogicalDevice, buffer, &vkMemoryRequirements);

            VkMemoryAllocateInfo vkMemoryAllocateInfo ={ };
            vkMemoryAllocateInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            vkMemoryAllocateInfo.allocationSize  = vkMemoryRequirements.size;

            CEngineResult<uint32_t> memoryTypeFetch =
                                            CVulkanDeviceCapsHelper::determineMemoryType(
                                                    vkPhysicalDevice,
                                                    vkMemoryRequirements.memoryTypeBits,
                                                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

            if(not memoryTypeFetch.successful())
            {
                CLog::Error(buffer_log::logTag(), "Could not determine memory type index.");
                return { EEngineStatus::Error };
            }

            vkMemoryAllocateInfo.memoryTypeIndex = memoryTypeFetch.data();

            VkDeviceMemory memory = VK_NULL_HANDLE;

            result = vkAllocateMemory(vkLogicalDevice, &vkMemoryAllocateInfo, nullptr, &memory);
            if(VkResult::VK_SUCCESS != result)
            {
                CLog::Error(buffer_log::logTag(), CString::format("Failed to allocate buffer memory on GPU. Vulkan error: {}", result));
                return { EEngineStatus::Error };
            }

            result = vkBindBufferMemory(vkLogicalDevice, buffer, memory, 0);
            if(VkResult::VK_SUCCESS != result)
            {
                CLog::Error(buffer_log::logTag(), CString::format("Failed to bind buffer memory on GPU. Vulkan error: {}", result));
                return { EEngineStatus::Error };
            }

            aGpuApiHandles.handle         = buffer;
            aGpuApiHandles.attachedMemory = memory;

            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TResourceManager>
        EEngineStatus SVulkanBufferResource::load(SBufferDescription  const &aDescription
                                                , Handles_t                 &aGpuApiHandles
                                                , IVkGlobalContext          *aVulkanEnvironment
                                                , TResourceManager          *aResourceManager)
        {
            if(nullptr != aDescription.dataSource)
            {
                VkDevice device = aVulkanEnvironment->getLogicalDevice();

                ByteBuffer const dataSource = aDescription.dataSource();

                void *mappedData = nullptr;
                vkMapMemory(device, aGpuApiHandles.attachedMemory, 0, dataSource.size(), 0, &mappedData);
                memcpy(mappedData, dataSource.data(), dataSource.size());
                vkUnmapMemory(device, aGpuApiHandles.attachedMemory);
            }

            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TResourceManager>
        EEngineStatus SVulkanBufferResource::unload(SBufferDescription  const &aDescription
                                                  , Handles_t                 &aGpuApiHandles
                                                  , IVkGlobalContext          *aVulkanEnvironment
                                                  , TResourceManager          *aResourceManager)
        {
            SHIRABE_UNUSED(aDescription);
            SHIRABE_UNUSED(aGpuApiHandles);
            SHIRABE_UNUSED(aVulkanEnvironment);

            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TResourceManager>
        EEngineStatus SVulkanBufferResource::deinitialize(SBufferDescription  const &aDescription
                                                        , Handles_t                 &aGpuApiHandles
                                                        , IVkGlobalContext          *aVulkanEnvironment
                                                        , TResourceManager          *aResourceManager)
        {
            VkBuffer       vkBuffer        = aGpuApiHandles.handle;
            VkDeviceMemory vkDeviceMemory  = aGpuApiHandles.attachedMemory;
            VkDevice       vkLogicalDevice = aVulkanEnvironment->getLogicalDevice();

            vkFreeMemory   (vkLogicalDevice, vkDeviceMemory, nullptr);
            vkDestroyBuffer(vkLogicalDevice, vkBuffer, nullptr);

            aGpuApiHandles.attachedMemory = VK_NULL_HANDLE;
            aGpuApiHandles.handle         = VK_NULL_HANDLE;

            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------
    }
}

#endif
