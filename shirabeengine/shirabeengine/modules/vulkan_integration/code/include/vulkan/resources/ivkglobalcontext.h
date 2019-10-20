//
// Created by dottideveloper on 20.10.19.
//

#ifndef SHIRABEDEVELOPMENT_IVKAPIGLOBALCONTEXT_H
#define SHIRABEDEVELOPMENT_IVKAPIGLOBALCONTEXT_H

#include <core/enginetypehelper.h>
#include "vulkan/resources/ivkframecontext.h"

namespace engine
{
    namespace vulkan
    {

        class SHIRABE_TEST_EXPORT IVkGlobalContext
        {
            SHIRABE_DECLARE_INTERFACE(IVkGlobalContext);

        public_api:
            virtual Shared<IVkFrameContext> getVkCurrentFrameContext() = 0;
        };

    }
}

#endif //SHIRABEDEVELOPMENT_IVKAPIRESOURCE_H
