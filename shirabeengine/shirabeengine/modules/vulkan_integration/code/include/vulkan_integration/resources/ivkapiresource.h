//
// Created by dottideveloper on 20.10.19.
//

#ifndef SHIRABEDEVELOPMENT_IVKAPIRESOURCE_H
#define SHIRABEDEVELOPMENT_IVKAPIRESOURCE_H

#include <core/enginetypehelper.h>
#include "vulkan_integration/resources/ivkglobalcontext.h"

namespace engine
{
    namespace vulkan
    {
        /**
         * Declares an interface to a vulkan global context used to bind API resources
         * to the necessary data of the current vulkan environment.
         */
        class SHIRABE_TEST_EXPORT IVkApiResource
        {
            SHIRABE_DECLARE_INTERFACE(IVkApiResource);

        public_api:
            virtual Shared<IVkGlobalContext> getVkGlobalContext() = 0;
        };

    }
}

#endif //SHIRABEDEVELOPMENT_IVKAPIRESOURCE_H
