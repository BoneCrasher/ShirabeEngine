#ifndef __SHIRABE_IRESOURCE_H__
#define __SHIRABE_IRESOURCE_H__

#include "core/enginetypehelper.h"
#include "core/enginestatus.h"
#include "core/patterns/observer.h"

namespace engine
{
    namespace resources
    {
        /**
         * Interface declaration for classes holding and exposing a graphics api resource.
         */
		template <typename TResource>
        class IGAPIResourceAdapter
        {
            SHIRABE_DECLARE_INTERFACE(IGAPIResourceAdapter);
		
        public_api:
            virtual CStdSharedPtr_t<TResource> const getGAPIResource() const = 0;

            virtual EEngineStatus releaseGAPIResource() = 0;

        };

        /**
         * Convenience: Declare a list of IGAPIResourceAdapter<TResource> to IGAPIResourceAdapterList
         */
		template <typename TResource>
        SHIRABE_DECLARE_LIST_OF_TYPE(CStdSharedPtr_t<IGAPIResourceAdapter<TResource>>, IGAPIResourceAdapter);
	}
}

#endif
