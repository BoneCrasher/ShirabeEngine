#ifndef __SHIRABE_VULKAN_RENDERPASS_RESOURCE_H__
#define __SHIRABE_VULKAN_RENDERPASS_RESOURCE_H__

#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>

#include <base/declaration.h>
#include <resources/resourcetypes.h>
#include <resources/agpuapiresourceobject.h>
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
        class CVulkanRenderPassResource
                : public CVkApiResource<SRenderPassDescription>
        {
            SHIRABE_DECLARE_LOG_TAG(CVulkanRenderPassResource);

        public_constructors:
            using CVkApiResource<SRenderPassDescription>::CVkApiResource;

        public_methods:
            CEngineResult<> create(CGpuApiDependencyCollection const &aDependencies) final;
            CEngineResult<> load()     final;
            CEngineResult<> transfer() final;
            CEngineResult<> unload()   final;
            CEngineResult<> destroy()  final;

        public_members:
            VkRenderPass handle;
        };
    }
}


#endif // VULKANRENDERPASSRESOURCE_H
