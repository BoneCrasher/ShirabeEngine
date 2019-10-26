//
// Created by dottideveloper on 20.10.19.
//

#ifndef SHIRABEDEVELOPMENT_CVKAPIRESOURCE_Hinclude "vulkan/
#define SHIRABEDEVELOPMENT_CVKAPIRESOURCE_H

#include <platform/platform.h>
#include <base/declaration.h>
#include <core/enginetypehelper.h>
#include <resources/aprivateresourceobject.h>
#include "vulkan_integration/resources/ivkapiresource.h"

namespace engine
{
    namespace vulkan
    {
        template <typename TDescriptor>
        class
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT CVkApiResource
            : public resources::APrivateResourceObject<TDescriptor>
            , public IVkApiResource
        {
        public_constructors:
            using resources::APrivateResourceObject<TDescriptor>::APrivateResourceObject;

        public_destructors:
            ~CVkApiResource() override = default;

        private_members:
        };

    }
}

#endif //SHIRABEDEVELOPMENT_CVKAPIRESOURCE_H
