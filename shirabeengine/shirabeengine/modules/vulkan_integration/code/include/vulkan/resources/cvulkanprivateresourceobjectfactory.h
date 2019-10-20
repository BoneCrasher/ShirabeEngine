
//
// Created by dottideveloper on 20.10.19.
//
#ifndef __SHIRABEDEVELOPMENT_CVULKANPRIVATERESOURCEOBJECTFACTORY_H__
#define __SHIRABEDEVELOPMENT_CVULKANPRIVATERESOURCEOBJECTFACTORY_H__

#include <resources/aresourceobjectfactory.h>

namespace engine
{
    namespace vulkan
    {
        class SHIRABE_LIBRARY_EXPORT CVulkanPrivateResourceObjectFactory
            : public resources::CResourceObjectFactory
        {
        public_constructors:
            CVulkanPrivateResourceObjectFactory() = default;

        private_destructors:
            ~CVulkanPrivateResourceObjectFactory() final = default;

        public_methods:
            [[nodiscard]]
            CEngineResult<> initialize();

            [[nodiscard]]
            CEngineResult<> deinitialize();

        private_members:
        };
    }
}

#endif //__SHIRABEDEVELOPMENT_CVULKANPRIVATERESOURCEOBJECTFACTORY_H__
