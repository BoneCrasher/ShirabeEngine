//
// Created by dottideveloper on 29.10.19.
//
#include <material/serialization.h>
#include "vulkan_integration/resources/types/vulkanshadermoduleresource.h"
#include "vulkan_integration/vulkandevicecapabilities.h"

namespace engine::vulkan
{
    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
   
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CEngineResult<> CVulkanShaderModuleResource::create(  SShaderModuleDescriptor      const &aDescription
                                                        , SNoDependencies              const &aDependencies
                                                        , GpuApiResourceDependencies_t const &aResolvedDependencies)

    {
        SHIRABE_UNUSED(aResolvedDependencies);

        if(EGpuApiResourceState::Loaded == getResourceState()
           || EGpuApiResourceState::Loading == getResourceState())
        {
            return EEngineStatus::Ok;
        }

        setResourceState(EGpuApiResourceState::Loading);

        CVkApiResource<SShaderModule>::create(aDescription, aDependencies, aResolvedDependencies);

        Shared<IVkGlobalContext> vkContext = getVkContext();
        VkDevice                 device    = vkContext->getLogicalDevice();

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
            std::vector<signed char> const &srcData      = data.dataVector();
            uint32_t const                  srcDataSize  = srcData.size();

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
                CLog::Error(logTag(), "Failed to create shader module for stage {}", stage);
                continue;
            }
            vkShaderModules.insert({ stage, vkShaderModule });
        }

        this->handles = vkShaderModules;

        setResourceState(EGpuApiResourceState::Loaded);

        return { EEngineStatus::Ok };
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CEngineResult<> CVulkanShaderModuleResource::load()
    {
        return { EEngineStatus::Ok };
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CEngineResult<> CVulkanShaderModuleResource::unload()
    {
        return { EEngineStatus::Ok };
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CEngineResult<> CVulkanShaderModuleResource::destroy()
    {
        VkDevice device = getVkContext()->getLogicalDevice();

        for(auto const &[stage, module] : this->handles)
        {
            vkDestroyShaderModule(device, module, nullptr);
        }
        this->handles.clear();

        setResourceState(EGpuApiResourceState::Discarded);

        return EEngineStatus::Ok;
    }
    //<-----------------------------------------------------------------------------
}
