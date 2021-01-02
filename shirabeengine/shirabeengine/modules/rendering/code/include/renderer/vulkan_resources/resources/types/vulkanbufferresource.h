#ifndef __SHIRABE_VULKAN_BUFFER_RESOURCE_H__
#define __SHIRABE_VULKAN_BUFFER_RESOURCE_H__

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
        struct SVulkanBufferResource;
    }

    namespace resources
    {
        template <>
        struct SLogicalToGpuApiResourceTypeMap<SBuffer> { using TGpuApiResource = vulkan::SVulkanBufferResource;  };
    }

    namespace vulkan
    {
        using namespace resources;

        namespace buffer_log
        {
            SHIRABE_DECLARE_LOG_TAG(SVulkanBufferResource)
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
         * The SVulkanTextureResource struct describes the relevant data to deal
         * with textures inside the vulkan API.
         */
        struct SVulkanBufferResource
        {
            static constexpr bool is_loadable      = false;
            static constexpr bool is_unloadable    = false;
            static constexpr bool is_transferrable = false;

            struct Handles_t {
                VkBuffer             handle;
                VkMemoryRequirements memoryRequirements;
            };

            template <typename TResourceManager>
            static EEngineStatus initialize(SBufferDescription  const &aDescription
                                          , Handles_t                 &aGpuApiHandles
                                          , TResourceManager          *aResourceManager
                                          , IVkGlobalContext          *aVulkanEnvironment);

            template <typename TResourceManager>
            static EEngineStatus deinitialize(SBufferDescription  const &aDescription
                                            , Handles_t                 &aGpuApiHandles
                                            , TResourceManager          *aResourceManager
                                            , IVkGlobalContext          *aVulkanEnvironment);

            template <typename TResourceManager>
            static EEngineStatus bindBufferAndMemory(TResourceManager   *aResourceManager
                                                     , IVkGlobalContext *aVulkanEnvironment
                                                     , VkBuffer          aVkBuffer
                                                     , VkDeviceMemory    aVkMemory);
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

            aGpuApiHandles.handle             = buffer;
            aGpuApiHandles.memoryRequirements = requirements;

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TResourceManager>
        EEngineStatus SVulkanBufferResource::load(SBufferDescription  const &aDescription
                                                , Handles_t                 &aGpuApiHandles
                                                , TResourceManager          *aResourceManager
                                                , IVkGlobalContext          *aVulkanEnvironment)
        {
            SHIRABE_UNUSED(aResourceManager);

            if(nullptr != aDescription.dataSource)
            {
                VkDevice device = aVulkanEnvironment->getLogicalDevice();

                ByteBuffer const dataSource = aDescription.dataSource();

                void *mappedData = nullptr;
                vkMapMemory(device, aGpuApiHandles.attachedMemory, 0, dataSource.size(), 0, &mappedData);
                memcpy(mappedData, dataSource.data(), dataSource.size());
                vkUnmapMemory(device, aGpuApiHandles.attachedMemory);
            }

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TResourceManager>
        EEngineStatus SVulkanBufferResource::unload(SBufferDescription  const &aDescription
                                                  , Handles_t                 &aGpuApiHandles
                                                  , TResourceManager          *aResourceManager
                                                  , IVkGlobalContext          *aVulkanEnvironment)
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
                                                        , TResourceManager          *aResourceManager
                                                        , IVkGlobalContext          *aVulkanEnvironment)
        {
            SHIRABE_UNUSED(aDescription);
            SHIRABE_UNUSED(aResourceManager);

            VkBuffer vkBuffer        = aGpuApiHandles.handle;
            VkDevice vkLogicalDevice = aVulkanEnvironment->getLogicalDevice();
            vkDestroyBuffer(vkLogicalDevice, vkBuffer, nullptr);

            aGpuApiHandles.handle = VK_NULL_HANDLE;
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TResourceManager>
        EEngineStatus SVulkanBufferResource::bindBufferAndMemory(TResourceManager   *aResourceManager
                                                                 , IVkGlobalContext *aVulkanEnvironment
                                                                 , VkBuffer          aVkBuffer
                                                                 , VkDeviceMemory    aVkMemory)
        {
            VkDevice vkLogicalDevice = aVulkanEnvironment->getLogicalDevice();
            vkBindBufferMemory(vkLogicalDevice, aVkBuffer, aVkMemory, 0);

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------
    }
}

#endif
