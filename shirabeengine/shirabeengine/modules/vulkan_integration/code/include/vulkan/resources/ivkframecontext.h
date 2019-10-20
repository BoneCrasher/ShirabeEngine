//
// Created by dottideveloper on 20.10.19.
//

#ifndef SHIRABEDEVELOPMENT_IVKAPIFRAMECONTEXT_H
#define SHIRABEDEVELOPMENT_IVKAPIFRAMECONTEXT_H

#include <vulkan/vulkan.h>
#include <core/enginetypehelper.h>

namespace engine
{
    namespace vulkan
    {

        class SHIRABE_TEST_EXPORT IVkFrameContext
        {
            SHIRABE_DECLARE_INTERFACE(IVkFrameContext);

        public_api:
            virtual VkQueue getTransferQueue() = 0;
        };

    }
}

#endif //SHIRABEDEVELOPMENT_IVKAPIRESOURCE_H
