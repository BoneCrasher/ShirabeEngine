
//
// Created by dottideveloper on 20.10.19.
//
#ifndef __SHIRABEDEVELOPMENT_CVULKANPRIVATERESOURCEOBJECTFACTORY_H__
#define __SHIRABEDEVELOPMENT_CVULKANPRIVATERESOURCEOBJECTFACTORY_H__

#include <resources/agpuapiresourceobjectfactory.h>
#include "vulkan_integration/resources/ivkglobalcontext.h"

namespace engine
{
    namespace vulkan
    {
        class SHIRABE_LIBRARY_EXPORT CVulkanResourceObjectFactory
            : public CGpuApiResourceObjectFactory
        {
        public_constructors:
            SHIRABE_INLINE
            explicit CVulkanResourceObjectFactory(Shared<CGpuApiResourceStorage> const &aStorage)
                : CGpuApiResourceObjectFactory(aStorage)
            {}

        public_destructors:
            ~CVulkanResourceObjectFactory() final = default;

        public_methods:
            [[nodiscard]]
            CEngineResult<> initialize(Shared<IVkGlobalContext> aVkContext);

            [[nodiscard]]
            CEngineResult<> deinitialize();

        private_members:
            Shared<IVkGlobalContext> mVkContext;
        };
    }
}

#endif //__SHIRABEDEVELOPMENT_CVULKANPRIVATERESOURCEOBJECTFACTORY_H__
