#ifndef __SHIRABE_VULKAN_SHADERMODULE_RESOURCE_H__
#define __SHIRABE_VULKAN_SHADERMODULE_RESOURCE_H__

#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>

#include <base/declaration.h>
#include <resources/resourcetypes.h>
#include <resources/agpuapiresourceobject.h>
#include <resources/iloadablegpuapiresourceobject.h>
#include <resources/itransferrablegpuapiresourceobject.h>

#include "vulkan_integration/vulkandevicecapabilities.h"
#include "vulkan_integration/resources/cvkapiresource.h"

namespace engine
{
    namespace vulkan
    {
        using namespace resources;
        /**
         * The SVulkanTextureResource struct describes the relevant data to deal
         * with textures inside the vulkan API.
         */
        class CVulkanShaderModuleResource
            : public CVkApiResource<SShaderModule>
            , public ILoadableGpuApiResourceObject
        {
            SHIRABE_DECLARE_LOG_TAG(CVulkanShaderModuleResource);

        public_constructors:
            using CVkApiResource<SShaderModule>::CVkApiResource;

        public_methods:
            // AGpuApiResourceObject
            CEngineResult<> create(  SShaderModuleDescriptor      const &aDescription
                                   , SNoDependencies              const &aDependencies
                                   , GpuApiResourceDependencies_t const &aResolvedDependencies) final;
            CEngineResult<> destroy() final;

            // ILoadableGpuApiResourceObject
            CEngineResult<> load()   const final;
            CEngineResult<> unload() const final;

        public_members:
            std::unordered_map<VkShaderStageFlags, VkShaderModule> handles;
        };
    }
}

#endif
