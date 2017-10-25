#ifndef __SHIRABE_RESOURCESUBSYSTEMPROXY_H__
#define __SHIRABE_RESOURCESUBSYSTEMPROXY_H__

#include "Core/EngineTypeHelper.h"
#include "Core/EngineStatus.h"

#include "Resources/System/Core/IResourceProxy.h"
#include "Resources/System/Core/IResourceDescriptor.h"

namespace Engine {
	namespace Resources {

		template <typename TSubsystem, typename TResource>
		class ResourceSubsystemProxy
			: public GenericProxyBase<TResource>
		{
		public:
			inline ResourceSubsystemProxy(
				const EProxyType                    &proxyType,
				const Ptr<TSubsystem>               &subsystem,
				const ResourceDescriptor<TResource> &descriptor)
				: GenericProxyBase<TResource>(proxyType, descriptor)
				, _subsystem(subsystem)
			{ }

		protected:
			TSubsystem subsystem() { return _subsystem; }

		private:
			Ptr<TSubsystem> _subsystem;
		};

		// template <typename TSubsystem, EResourceType type, EResourceSubType subtype>
		// bool ResourceSubsystemProxy<TSubsystem, type, subtype>
		// 	::loadSync(
		// 		const ResourceHandle  &inHandle,
		// 		const ResourceProxyMap&inDependencies)
		// {
		// 	return true;
		// }
		// 
		// template <typename TSubsystem, EResourceType type, EResourceSubType subtype>
		// bool ResourceSubsystemProxy<TSubsystem, type, subtype>
		// 	::unloadSync()
		// {
		// }

	}
}

#endif