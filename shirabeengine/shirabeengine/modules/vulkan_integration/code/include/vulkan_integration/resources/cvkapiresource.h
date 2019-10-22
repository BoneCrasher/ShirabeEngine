//
// Created by dottideveloper on 20.10.19.
//

#ifndef SHIRABEDEVELOPMENT_CVKAPIRESOURCE_H
#define SHIRABEDEVELOPMENT_CVKAPIRESOURCE_H

#include <platform/platform.h>
#include <base/declaration.h>
#include <core/enginetypehelper.h>
#include <resources/iresourceobjectprivate.h>
#include "vulkan_integration/resources/ivkapiresource.h"

namespace engine
{
    namespace vulkan
    {

        class
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT CVkApiResource
            : public resources::IResourceObjectPrivate
            , public IVkApiResource
        {
        public_constructors:
        public_destructors:
        private_members:
        };

    }
}

#endif //SHIRABEDEVELOPMENT_CVKAPIRESOURCE_H
