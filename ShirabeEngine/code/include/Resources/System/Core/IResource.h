#ifndef __SHIRABE_IRESOURCE_H__
#define __SHIRABE_IRESOURCE_H__

#include "Core/EngineTypeHelper.h"
#include "IOC/Observer.h"

namespace Engine {
	namespace Resources {

		template <typename TResource>
		using GAPIResourcePtr = std::shared_ptr<TResource>;

		template <typename TResource>
		DeclareInterface(IGAPIResourceAdapter);
		
		virtual const GAPIResourcePtr<TResource>
			getGAPIResource() const = 0;

		virtual EEngineStatus 
			releaseGAPIResource() = 0;

		DeclareInterfaceEnd(IGAPIResourceAdapter);

		template <typename TResource>
		// using IGAPIResourceAdapterPtr = std::shared_ptr<IGAPIResourceAdapter<TGAPIResource>>;
		DeclareTemplatedSharedPointerType(IGAPIResourceAdapter, typename IGAPIResourceAdapter<TResource>);

		template <typename TResource>
		DeclareListType(IGAPIResourceAdapterPtr<TResource>, IGAPIResourceAdapter);
	}
}

#endif