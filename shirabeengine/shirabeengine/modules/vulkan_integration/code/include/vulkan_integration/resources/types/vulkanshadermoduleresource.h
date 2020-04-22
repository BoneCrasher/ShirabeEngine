#ifndef __SHIRABE_VULKAN_SHADERMODULE_RESOURCE_H__
#define __SHIRABE_VULKAN_SHADERMODULE_RESOURCE_H__

#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>

#include <base/declaration.h>
#include <resources/resourcetypes.h>
#include <resources/extensibility.h>

#include "vulkan_integration/vulkandevicecapabilities.h"
#include "vulkan_integration/resources/cvkapiresource.h"
#include "vulkan_integration/resources/ivkglobalcontext.h"

namespace engine
{
    namespace vulkan
    {
        struct SVulkanShaderModuleResource;
    }

    namespace resources
    {
        template <> struct SLogicalToGpuApiResourceTypeMap<SShaderModule>  { using TGpuApiResource = struct vulkan::SVulkanShaderModuleResource;  };
    }

    namespace vulkan
    {
        using namespace resources;

        namespace shadermodule_log
        {
            SHIRABE_DECLARE_LOG_TAG(SVulkanShaderModuleResource);
        }

        /**
         * The SVulkanTextureResource struct describes the relevant data to deal
         * with textures inside the vulkan API.
         */
        struct SVulkanShaderModuleResource
        {
            struct Handles_t
            {
                std::unordered_map<VkShaderStageFlags, VkShaderModule> handles;
            };

            template <typename TResourceManager>
            static EEngineStatus initialize(SShaderModuleDescriptor const &aDescription
                                          , Handles_t                     &aGpuApiHandles
                                          , IVkGlobalContext              *aVulkanEnvironment
                                          , TResourceManager              *aResourceManager);

            template <typename TResourceManager>
            static EEngineStatus deinitialize(SShaderModuleDescriptor const &aDescription
                                            , Handles_t                     &aGpuApiHandles
                                            , IVkGlobalContext              *aVulkanEnvironment
                                            , TResourceManager              *aResourceManager);
        };

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        using ShaderModuleResourceState_t = SResourceState<SShaderModule>;

        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TResourceManager>
        EEngineStatus SVulkanShaderModuleResource::initialize(SShaderModuleDescriptor const &aDescription
                                                            , Handles_t                     &aGpuApiHandles
                                                            , IVkGlobalContext              *aVulkanEnvironment
                                                            , TResourceManager              *aResourceManager)
        {
            VkDevice device = aVulkanEnvironment->getLogicalDevice();

            std::unordered_map<VkShaderStageFlags, VkShaderModule> vkShaderModules {};

            for(auto const &[stage, dataAccessor] : aDescription.shaderStages)
            {
                if(not dataAccessor)
                {
                    // If we don't have any data to access, then don't create a shader module.
                    continue;
                }

                ByteBuffer const data = dataAccessor();

                // We need to convert from a regular 8-bit data buffer to uint32 words of SPIR-V.
                // TODO: Refactor the asset system to permit loading 32-bit buffers...
                std::vector<uint8_t> const &srcData      = data.dataVector();
                uint32_t const              srcDataSize  = srcData.size();

                std::vector<uint32_t> convData {};
                convData.resize( srcDataSize / 4 );

                for(uint32_t k=0; k<srcDataSize; k += 4)
                {
                    uint32_t const value = *reinterpret_cast<uint32_t const*>( srcData.data() + k );
                    convData[ k / 4 ] = value;
                }

                VkShaderModuleCreateInfo shaderModuleCreateInfo {};
                shaderModuleCreateInfo.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
                shaderModuleCreateInfo.pNext    = nullptr;
                shaderModuleCreateInfo.flags    = 0;
                shaderModuleCreateInfo.pCode    = convData.data();
                shaderModuleCreateInfo.codeSize = srcData.size();

                VkShaderModule vkShaderModule = VK_NULL_HANDLE;
                VkResult const moduleCreationResult = vkCreateShaderModule(device, &shaderModuleCreateInfo, nullptr, &vkShaderModule);
                if(VkResult::VK_SUCCESS != moduleCreationResult)
                {
                    CLog::Error(shadermodule_log::logTag(), "Failed to create shader module for stage {}", stage);
                    continue;
                }
                vkShaderModules.insert({ stage, vkShaderModule });
            }

            aGpuApiHandles.handles = vkShaderModules;

            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TResourceManager>
        EEngineStatus SVulkanShaderModuleResource::deinitialize(SShaderModuleDescriptor const &aDescription
                                                              , Handles_t                     &aGpuApiHandles
                                                              , IVkGlobalContext              *aVulkanEnvironment
                                                              , TResourceManager              *aResourceManager)
        {
            VkDevice device = aVulkanEnvironment->getLogicalDevice();

            for(auto const &[stage, module] : aGpuApiHandles.handles)
            {
                vkDestroyShaderModule(device, module, nullptr);
            }
            aGpuApiHandles.handles.clear();

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------
    }
}

#endif
