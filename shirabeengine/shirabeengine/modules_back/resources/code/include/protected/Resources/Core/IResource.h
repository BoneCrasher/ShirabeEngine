#ifndef __SHIRABE_IRESOURCE_H__
#define __SHIRABE_IRESOURCE_H__

#include "core/enginetypehelper.h"
#include "core/enginestatus.h"
#include "Core/IOC/Observer.h"

namespace Engine {
	namespace Resources {

		template <typename TResource>
		using GAPIResourcePtr = CStdSharedPtr_t<TResource>;

		template <typename TResource>
		DeclareInterface(IGAPIResourceAdapter);
		
		virtual const GAPIResourceCStdSharedPtr_t<TResource>
			getGAPIResource() const = 0;

		virtual EEngineStatus 
			releaseGAPIResource() = 0;

		DeclareInterfaceEnd(IGAPIResourceAdapter);

		template <typename TResource>
		// using IGAPIResourceAdapterPtr = CStdSharedPtr_t<IGAPIResourceAdapter<TGAPIResource>>;
		DeclareTemplatedSharedPointerType(IGAPIResourceAdapter, typename IGAPIResourceAdapter<TResource>);

		template <typename TResource>
		DeclareListType(IGAPIResourceAdapterCStdSharedPtr_t<TResource>, IGAPIResourceAdapter);
	}
}

#endif