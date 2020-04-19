//
// Created by dotti on 19.10.19.
//

#ifndef __SHIRABEDEVELOPMENT_CRESOURCEOBJECT_H__
#define __SHIRABEDEVELOPMENT_CRESOURCEOBJECT_H__

#include <platform/platform.h>
#include <core/patterns/observer.h>

class CResourceManager; // Declared/defined in resources/cresourcemanager.h

namespace engine
{
    namespace resources
    {
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TDescription>
        class
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT CResourceObject
        {
        public_typedefs:
            using Descriptor_t = TDescription;
        };
        //<-----------------------------------------------------------------------------
    }
}

#endif //__SHIRABEDEVELOPMENT_CRESOURCEOBJECT_H__
