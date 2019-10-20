//
// Created by dottideveloper on 20.10.19.
//

#ifndef SHIRABEDEVELOPMENT_IVKAPIRESOURCE_H
#define SHIRABEDEVELOPMENT_IVKAPIRESOURCE_H

#include <core/enginetypehelper.h>
#include "vulkan/resources/ivkglobalcontext.h"

namespace engine
{
    namespace vulkan
    {

        class SHIRABE_TEST_EXPORT IVkApiResource
        {
            SHIRABE_DECLARE_INTERFACE(IVkApiResource);

        public_api:
            virtual Shared<IVkGlobalContext> getVkGlobalContext() = 0;
        };

    }
}

#endif //SHIRABEDEVELOPMENT_IVKAPIRESOURCE_H
