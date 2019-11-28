#ifndef __SHIRABE_VULKAN_TEXTUREVIEW_RESOURCE_H__
#define __SHIRABE_VULKAN_TEXTUREVIEW_RESOURCE_H__

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
        class CVulkanTextureViewResource
                : public CVkApiResource<STextureView>
        {
            SHIRABE_DECLARE_LOG_TAG(CVulkanBufferResource);

        public_constructors:
            using CVkApiResource<STextureView>::CVkApiResource;

        public_methods:
            CEngineResult<> create(  STextureViewDescription      const &aDescription
                                   , STextureViewDependencies     const &aDependencies
                                   , GpuApiResourceDependencies_t const &aResolvedDependencies) final;
            CEngineResult<> destroy() final;

        public_members:
            VkImageView handle;
        };

    }
}

#endif
