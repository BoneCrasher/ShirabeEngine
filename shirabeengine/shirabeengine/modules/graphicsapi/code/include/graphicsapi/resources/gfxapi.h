#ifndef __SHIRABE_GFXAPI_H__
#define __SHIRABE_GFXAPI_H__

#include <type_traits>
#include <typeinfo>
#include <optional>
#include <stdint.h>
#include <map>
#include <vector>

#include <core/enginetypehelper.h>
#include <core/enginestatus.h>
#include <core/patterns/observer.h>

#include "resources/core/resourcedomaintransfer.h"

namespace engine
{
    namespace gfxapi
    {

        using namespace engine::resources;

        /**
         * Binds a public resource handle to a pointer to backend resource as a single entity.
         */
        struct SGFXAPIResourceHandleAssignment
        {
        public_constructors:
            /**
             * Construct an empty assignment.
             */
            SHIRABE_INLINE SGFXAPIResourceHandleAssignment()
                : publicResourceHandle("")
                , internalResourceHandle(nullptr)
            {}

            /**
             * Construct a new assignment from a public and internal resource handle.
             *
             * @param aPublicResourceHandle   Public resource manager resource handle.
             * @param aInternalResourceHandle Internal backend resource handle.
             */
            SHIRABE_INLINE SGFXAPIResourceHandleAssignment(
                    PublicResourceId_t    const &aPublicResourceHandle,
                    CStdSharedPtr_t<void> const &aInternalResourceHandle)
                : publicResourceHandle(aPublicResourceHandle)
                , internalResourceHandle(aInternalResourceHandle)
            {}

            /**
             * Checks, whether a resource assignment is valid or not.
             *
             * @return
             */
            SHIRABE_INLINE bool valid() const
            {
                bool const isValid = (!publicResourceHandle.empty() && internalResourceHandle);
                return isValid;
            }

        public_members:
            PublicResourceId_t    publicResourceHandle;
            CStdSharedPtr_t<void> internalResourceHandle;
        };
    }
}

#endif
