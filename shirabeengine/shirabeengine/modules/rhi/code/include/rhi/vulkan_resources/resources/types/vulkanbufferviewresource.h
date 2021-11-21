#ifndef __SHIRABE_VULKAN_BUFFERVIEW_RESOURCE_H__
#define __SHIRABE_VULKAN_BUFFERVIEW_RESOURCE_H__

#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>

#include <base/declaration.h>

#include "rhi/resource_management/resourcetypes.h"
#include "rhi/resource_management/extensibility.h"
#include "rhi/vulkan_resources/resources/cvkapiresource.h"
#include "rhi/vulkan_resources/resources/ivkglobalcontext.h"
#include "rhi/vulkan_resources/resources/types/vulkanbufferresource.h"

namespace engine
{
    namespace vulkan
    {
        struct SVulkanRHIBufferView;
    }

    namespace rhi
    {
        template <> struct SRHIResourceMap<SRHIBufferView>  { using TMappedRHIResource = vulkan::SVulkanRHIBufferView;  };
    }

    namespace vulkan
    {
        using namespace rhi;

        namespace bufferview_log
        {
            SHIRABE_DECLARE_LOG_TAG(SVulkanBufferViewResource)
        }

        /**
         * The SVulkanRHIImageResource struct describes the relevant data to deal
         * with textures inside the vulkan API.
         */
        struct SVulkanRHIBufferView
        {
            struct Handles_t
            {
                VkBufferView handle;
            };

            template <typename TResourceManager>
            static EEngineStatus initialize(SRHIBufferViewDescription  const &aDescription
                                            , Handles_t                     &aGpuApiHandles
                                            , TResourceManager              *aResourceManager
                                            , IVkGlobalContext              *aVulkanEnvironment);

            template <typename TResourceManager>
            static EEngineStatus deinitialize(SRHIBufferViewDescription  const &aDescription
                                              , Handles_t                     &aGpuApiHandles
                                              , TResourceManager              *aResourceManager
                                              , IVkGlobalContext              *aVulkanEnvironment);
        };

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        using RHIBufferViewResourceState_t = SRHIResourceState<SRHIBufferView>;

        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TResourceManager>
        EEngineStatus SVulkanRHIBufferView::initialize(SRHIBufferViewDescription  const &aDescription
                                                       , Handles_t                      &aGpuApiHandles
                                                       , TResourceManager               *aResourceManager
                                                       , IVkGlobalContext               *aVulkanEnvironment)
        {
            OptionalRef_t<RHIBufferResourceState_t> const bufferOpt = aResourceManager->template getResource(aDescription.subjacentBufferId, aVulkanEnvironment);
            if(not bufferOpt.has_value())
            {
                return EEngineStatus::ResourceError_DependencyNotFound;
            }
            RHIBufferResourceState_t const &bufferState = *bufferOpt;

            VkBufferViewCreateInfo createInfo = aDescription.createInfo;
            createInfo.buffer = bufferState.rhiHandles.buffer;

            VkBufferView vkBufferView = VK_NULL_HANDLE;

            VkResult result = vkCreateBufferView(aVulkanEnvironment->getLogicalDevice(), &(createInfo), nullptr, &vkBufferView);
            if(VkResult::VK_SUCCESS != result)
            {
                CLog::Error(bufferview_log::logTag(), StaticStringHelpers::format("Failed to create buffer view. Vulkan error: {}", result));
                return EEngineStatus::Error;
            }

            aGpuApiHandles.handle = vkBufferView;

            // getVkContext()->registerDebugObjectName((uint64_t)this->handle, VK_OBJECT_TYPE_BUFFER_VIEW, aDescription.name);

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TResourceManager>
        EEngineStatus SVulkanRHIBufferView::deinitialize(SRHIBufferViewDescription  const &aDescription
                                                         , Handles_t                      &aGpuApiHandles
                                                         , TResourceManager               *aResourceManager
                                                         , IVkGlobalContext               *aVulkanEnvironment)
        {
            VkBufferView vkBufferView    = aGpuApiHandles.handle;
            VkDevice     vkLogicalDevice = aVulkanEnvironment->getLogicalDevice();

            vkDestroyBufferView(vkLogicalDevice, vkBufferView, nullptr);

            aGpuApiHandles.handle = VK_NULL_HANDLE;

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------
    }
}

#endif
