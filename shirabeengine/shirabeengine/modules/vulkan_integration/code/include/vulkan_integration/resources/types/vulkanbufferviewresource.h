#ifndef __SHIRABE_VULKAN_BUFFERVIEW_RESOURCE_H__
#define __SHIRABE_VULKAN_BUFFERVIEW_RESOURCE_H__

#include <vulkan/vulkan.h>

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
        class CVulkanBufferViewResource
                : public CVkApiResource<SBufferViewDescription>
        {
            SHIRABE_DECLARE_LOG_TAG(CVulkanBufferResource);

        public_constructors:
            using CVkApiResource<SBufferViewDescription>::AGpuApiResourceObject;

        public_methods:
            CEngineResult<> create(CGpiApiDependencyCollection const &aDependencies)   final;
            CEngineResult<> load()     final;
            CEngineResult<> unload()   final;
            CEngineResult<> destroy()  final;
            CEngineResult<> bind()     final;
            CEngineResult<> transfer() final;
            CEngineResult<> unbind()   final;

        public_members:
            VkBufferView handle;
        };

    }
}

#endif
