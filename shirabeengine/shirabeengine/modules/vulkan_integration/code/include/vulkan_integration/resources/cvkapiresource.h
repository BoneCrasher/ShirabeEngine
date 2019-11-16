//
// Created by dottideveloper on 20.10.19.
//

#ifndef SHIRABEDEVELOPMENT_CVKAPIRESOURCE_H
#define SHIRABEDEVELOPMENT_CVKAPIRESOURCE_H

#include <platform/platform.h>
#include <base/declaration.h>
#include <core/enginetypehelper.h>
#include <resources/agpuapiresourceobject.h>
#include "vulkan_integration/resources/ivkapiresource.h"

namespace engine
{
    namespace vulkan
    {
        template <typename TDescriptor>
        class
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT CVkApiResource
            : public resources::AGpuApiResourceObject<TDescriptor>
            , public IVkApiResource
        {
        public_constructors:
            explicit CVkApiResource(  Shared<IVkGlobalContext>         aVkContext
                                    , resources::GpuApiHandle_t const &aHandle
                                    , TDescriptor               const &aDescription)
                : resources::AGpuApiResourceObject<TDescriptor>::AGpuApiResourceObject(aDescription)
                , mVkContext(std::move(aVkContext))
                , mHandle   (aHandle)
            {}

        public_destructors:
            ~CVkApiResource() override = default;

        public_api:
            [[nodiscard]]
            SHIRABE_INLINE resources::GpuApiHandle_t const getHandle() final
            {
                return mHandle;
            }

        protected_methods:
            SHIRABE_INLINE void setHandle(resources::GpuApiHandle_t const &aHandle)
            {
                mHandle = aHandle;
            };

            [[nodiscard]]
            SHIRABE_INLINE Shared<IVkGlobalContext> getVkContext() const final
            {
                return mVkContext;
            }

        private_members:
            Shared<IVkGlobalContext>  mVkContext;
            resources::GpuApiHandle_t mHandle;
        };

    }
}

#endif //SHIRABEDEVELOPMENT_CVKAPIRESOURCE_H
