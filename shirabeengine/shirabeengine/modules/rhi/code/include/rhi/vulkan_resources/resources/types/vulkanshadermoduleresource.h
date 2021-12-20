#ifndef __SHIRABE_VULKAN_SHADERMODULE_RESOURCE_H__
#define __SHIRABE_VULKAN_SHADERMODULE_RESOURCE_H__

#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>

#include <base/declaration.h>
#include "rhi/resource_management/resourcetypes.h"
#include "rhi/resource_management/extensibility.h"
#include "rhi/vulkan_resources/resources/cvkapiresource.h"
#include "rhi/vulkan_resources/resources/ivkglobalcontext.h"
#include "rhi/vulkan_core/vulkandevicecapabilities.h"

namespace engine
{
    SHIRABE_DECLARE_VULKAN_RHI_RESOURCE(ShaderModule)

    namespace vulkan
    {
        using namespace rhi;

        /**
         * The SVulkanRHIImageResource struct describes the relevant data to deal
         * with textures inside the vulkan API.
         */
        struct SVulkanRHIShaderModule
        {
            struct Handles_t
            {
                std::unordered_map<VkShaderStageFlags, VkShaderModule> handles;
            };

            template <typename TResourceManager>
            static EEngineStatus initialize(SRHIShaderModuleDescriptor const &aDescription
                                            , Handles_t                      &aGpuApiHandles
                                            , TResourceManager               *aResourceManager
                                            , IVkGlobalContext               *aVulkanEnvironment);

            template <typename TResourceManager>
            static EEngineStatus deinitialize(SRHIShaderModuleDescriptor const &aDescription
                                              , Handles_t                      &aGpuApiHandles
                                              , TResourceManager               *aResourceManager
                                              , IVkGlobalContext               *aVulkanEnvironment);
        };

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        using RHIShaderModuleResourceState_t = SRHIResourceState<SRHIShaderModule>;

        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TResourceManager>
        EEngineStatus SVulkanRHIShaderModule::initialize(SRHIShaderModuleDescriptor const &aDescription
                                                         , Handles_t                      &aGpuApiHandles
                                                         , TResourceManager               *aResourceManager
                                                         , IVkGlobalContext               *aVulkanEnvironment)
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
                    CLog::Error(ShaderModule_log::logTag(), "Failed to create shader module for stage {}", stage);
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
        EEngineStatus SVulkanRHIShaderModule::deinitialize(SRHIShaderModuleDescriptor const &aDescription
                                                           , Handles_t                      &aGpuApiHandles
                                                           , TResourceManager               *aResourceManager
                                                           , IVkGlobalContext               *aVulkanEnvironment)
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
