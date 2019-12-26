//
// Created by dottideveloper on 20.10.19.
//

#ifndef SHIRABEDEVELOPMENT_IVKAPIFRAMECONTEXT_H
#define SHIRABEDEVELOPMENT_IVKAPIFRAMECONTEXT_H

#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>
#include <core/enginetypehelper.h>

namespace engine
{
    namespace vulkan
    {

        class SHIRABE_TEST_EXPORT IVkFrameContext
        {
            SHIRABE_DECLARE_INTERFACE(IVkFrameContext);

        public_api:
            virtual VkQueue getGraphicsQueue() = 0;
            virtual VkQueue getTransferQueue() = 0;
            virtual VkQueue getPresentQueue()  = 0;

            virtual VkCommandBuffer getGraphicsCommandBuffer() = 0;
            virtual VkCommandBuffer getTransferCommandBuffer() = 0;
        };

    }
}

#endif //SHIRABEDEVELOPMENT_IVKAPIRESOURCE_H
