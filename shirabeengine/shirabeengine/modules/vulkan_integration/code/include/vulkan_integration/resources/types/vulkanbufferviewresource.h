#ifndef __SHIRABE_VULKAN_BUFFERVIEW_RESOURCE_H__
#define __SHIRABE_VULKAN_BUFFERVIEW_RESOURCE_H__

#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>

#include <base/declaration.h>
#include <resources/resourcetypes.h>
#include "vulkan_integration/resources/cvkapiresource.h"
#include "vulkan_integration/rendering/vulkanrendercontext.h"

namespace engine
{
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
            struct Handles_t
            {
                VkBufferView handle;
            };

            static EEngineStatus initialize(SBufferViewDescription  const &aDescription
                                          , SBufferViewDependencies const &aDependencies
                                          , Handles_t                     &aGpuApiHandles
                                          , Shared<IVkGlobalContext>       aVulkanEnvironment);

            static EEngineStatus deinitialize(SBufferViewDescription  const &aDescription
                                            , SBufferViewDependencies const &aDependencies
                                            , Handles_t                     &aGpuApiHandles
                                            , Shared<IVkGlobalContext>       aVulkanEnvironment);
        };

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        static EEngineStatus SVulkanBufferViewResource::initialize(SBufferViewDescription  const &aDescription
                                                                 , SBufferViewDependencies const &aDependencies
                                                                 , Handles_t                     &aGpuApiHandles
                                                                 , Shared<IVkGlobalContext>       aVulkanEnvironment)
        {
            VkBufferViewCreateInfo createInfo = aDescription.createInfo;
            createInfo.buffer =  aDependencies.bufferHandle;

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
        static EEngineStatus SVulkanBufferViewResource::deinitialize(SBufferViewDescription  const &aDescription
                                                                   , SBufferViewDependencies const &aDependencies
                                                                   , Handles_t                     &aGpuApiHandles
                                                                   , Shared<IVkGlobalContext>       aVulkanEnvironment)
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
