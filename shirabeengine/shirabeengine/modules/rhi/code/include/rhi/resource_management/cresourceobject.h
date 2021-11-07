//
// Created by dotti on 19.10.19.
//

#ifndef __SHIRABEDEVELOPMENT_CRESOURCEOBJECT_H__
#define __SHIRABEDEVELOPMENT_CRESOURCEOBJECT_H__

#include <platform/platform.h>
#include <core/patterns/observer.h>

namespace engine
{
    namespace rhi
    {
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TDescription>
        class
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT CRHIResourceObject
        {
        public_typedefs:
            using Descriptor_t = TDescription;
        };
        //<-----------------------------------------------------------------------------
    }
}

#endif //__SHIRABEDEVELOPMENT_CRESOURCEOBJECT_H__
