#ifndef __SHIRABE_IRESOURCE_H__
#define __SHIRABE_IRESOURCE_H__

#include "Core/EngineTypeHelper.h"
#include "IOC/Observer.h"

namespace Engine {
	namespace Resources {

		template <typename TGAPIResource>
		using GAPIResourcePtr = std::shared_ptr<TGAPIResource>;

		template <typename TGAPIResource>
		DeclareInterface(IGAPIResourceAdapter);
		
		virtual const GAPIResourcePtr<TGAPIResource>
			getGAPIResource() const = 0;

		virtual EEngineStatus 
			releaseGAPIResource() = 0;

		DeclareInterfaceEnd(IGAPIResourceAdapter);

		template <typename TGAPIResource>
		// using IGAPIResourceAdapterPtr = std::shared_ptr<IGAPIResourceAdapter<TGAPIResource>>;
		DeclareTemplatedSharedPointerType(IGAPIResourceAdapter, typename IGAPIResourceAdapter<TGAPIResource>);

		template <typename TGAPIResource>
		DeclareListType(IGAPIResourceAdapterPtr<TGAPIResource>, IGAPIResourceAdapter);
	}
}

#endif