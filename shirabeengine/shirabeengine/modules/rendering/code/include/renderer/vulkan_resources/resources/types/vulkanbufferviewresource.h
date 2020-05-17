#ifndef __SHIRABE_VULKAN_BUFFERVIEW_RESOURCE_H__
#define __SHIRABE_VULKAN_BUFFERVIEW_RESOURCE_H__

#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>

#include <base/declaration.h>

#include "renderer/resource_management/resourcetypes.h"
#include "renderer/resource_management/extensibility.h"
#include "renderer/vulkan_resources/resources/cvkapiresource.h"
#include "renderer/vulkan_resources/resources/ivkglobalcontext.h"
#include "renderer/vulkan_resources/resources/types/vulkanbufferresource.h"

namespace engine
{
    namespace vulkan
    {
        struct SVulkanBufferViewResource;
    }

    namespace resources
    {
        template <> struct SLogicalToGpuApiResourceTypeMap<SBufferView>  { using TGpuApiResource = vulkan::SVulkanBufferViewResource;  };
    }

    namespace vulkan
    {
        using namespace resources;

        namespace bufferview_log
        {
            SHIRABE_DECLARE_LOG_TAG(SVulkanBufferViewResource);
        }

        /**
         * The SVulkanTextureResource struct describes the relevant data to deal
         * with textures inside the vulkan API.
         */
        struct SVulkanBufferViewResource
        {
            static constexpr bool is_loadable      = false;
            static constexpr bool is_unloadable    = false;
            static constexpr bool is_transferrable = false;

            struct Handles_t
            {
                VkBufferView handle;
            };

            template <typename TResourceManager>
            static EEngineStatus initialize(SBufferViewDescription  const &aDescription
                                          , Handles_t                     &aGpuApiHandles
                                          , TResourceManager              *aResourceManager
                                          , IVkGlobalContext              *aVulkanEnvironment);

            template <typename TResourceManager>
            static EEngineStatus deinitialize(SBufferViewDescription  const &aDescription
                                            , Handles_t                     &aGpuApiHandles
                                            , TResourceManager              *aResourceManager
                                            , IVkGlobalContext              *aVulkanEnvironment);
        };

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        using BufferViewResourceState_t = SResourceState<SBufferView>;

        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TResourceManager>
        EEngineStatus SVulkanBufferViewResource::initialize(SBufferViewDescription  const &aDescription
                                                          , Handles_t                     &aGpuApiHandles
                                                          , TResourceManager              *aResourceManager
                                                          , IVkGlobalContext              *aVulkanEnvironment)
        {
            OptionalRef_t<BufferResourceState_t> const bufferOpt = aResourceManager->template getResource(aDescription.subjacentBufferId, aVulkanEnvironment);
            if(not bufferOpt.has_value())
            {
                return EEngineStatus::ResourceError_DependencyNotFound;
            }
            BufferResourceState_t const &bufferState = *bufferOpt;

            VkBufferViewCreateInfo createInfo = aDescription.createInfo;
            createInfo.buffer = bufferState.gpuApiHandles.handle;

            VkBufferView vkBufferView = VK_NULL_HANDLE;

            VkResult result = vkCreateBufferView(aVulkanEnvironment->getLogicalDevice(), &(createInfo), nullptr, &vkBufferView);
            if(VkResult::VK_SUCCESS != result)
            {
                CLog::Error(bufferview_log::logTag(), CString::format("Failed to create buffer view. Vulkan error: {}", result));
                return { EEngineStatus::Error };
            }

            aGpuApiHandles.handle = vkBufferView;

            // getVkContext()->registerDebugObjectName((uint64_t)this->handle, VK_OBJECT_TYPE_BUFFER_VIEW, aDescription.name);

            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TResourceManager>
        EEngineStatus SVulkanBufferViewResource::deinitialize(SBufferViewDescription  const &aDescription
                                                            , Handles_t                     &aGpuApiHandles
                                                            , TResourceManager              *aResourceManager
                                                            , IVkGlobalContext              *aVulkanEnvironment)
        {
            VkBufferView vkBufferView    = aGpuApiHandles.handle;
            VkDevice     vkLogicalDevice = aVulkanEnvironment->getLogicalDevice();

            vkDestroyBufferView(vkLogicalDevice, vkBufferView, nullptr);

            aGpuApiHandles.handle = VK_NULL_HANDLE;

            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------
    }
}

#endif
