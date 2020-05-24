//
// Created by dottideveloper on 20.10.19.
//

#ifndef SHIRABEDEVELOPMENT_IVKAPIGLOBALCONTEXT_H
#define SHIRABEDEVELOPMENT_IVKAPIGLOBALCONTEXT_H

#include <core/enginetypehelper.h>
#include "renderer/vulkan_core/vulkanenvironmenttypes.h"
#include "renderer/vulkan_resources/resources/ivkframecontext.h"

namespace engine
{
    namespace resources
    {
        class CGpuApiResourceStorage;
    }

    namespace vulkan
    {
        using resources::CGpuApiResourceStorage;

        class SHIRABE_TEST_EXPORT IVkGlobalContext
        {
            SHIRABE_DECLARE_INTERFACE(IVkGlobalContext)

        public_api:
            virtual VkDevice                       getLogicalDevice()         = 0;
            virtual VkPhysicalDevice               getPhysicalDevice()        = 0;

            virtual Shared<IVkFrameContext>        getVkCurrentFrameContext() = 0;

            virtual void registerDebugObjectName(uint64_t const &aHandle, VkObjectType const &aObjectType, std::string const &aObjectName) = 0;
        };

    }
}

#endif //SHIRABEDEVELOPMENT_IVKAPIRESOURCE_H
