#ifndef __SHIRABE_RESOURCESUBSYSTEMPROXY_H__
#define __SHIRABE_RESOURCESUBSYSTEMPROXY_H__

#include "Core/EngineTypeHelper.h"
#include "Core/EngineStatus.h"

#include "Resources/Core/IResourceProxy.h"
#include "Resources/Core/ResourceDomainTransfer.h"

namespace engine {
	namespace Resources {

		template <typename TBackend, typename TResource>
		class ResourceBackendProxy
			: public GenericProxyBase<TResource>
		{
		public:
			inline ResourceBackendProxy(
				const EProxyType                          &proxyType,
				const CStdSharedPtr_t<TBackend>                       &resourceBackend,
				const typename TResource::CreationRequest &request)
				: GenericProxyBase<TResource>(proxyType, request)
				, m_backend(resourceBackend)
			{ }

		protected:
			CStdSharedPtr_t<TBackend> resourceBackend() { return m_backend; }

		private:
			CStdSharedPtr_t<TBackend> m_backend;
		};

		// template <typename TBackend, EResourceType type, EResourceSubType subtype>
		// bool ResourceBackendProxy<TBackend, type, subtype>
		// 	::loadSync(
		// 		const ResourceHandle  &inHandle,
		// 		const ResourceProxyMap&inDependencies)
		// {
		// 	return true;
		// }
		// 
		// template <typename TBackend, EResourceType type, EResourceSubType subtype>
		// bool ResourceBackendProxy<TBackend, type, subtype>
		// 	::unloadSync()
		// {
		// }

	}
}

#endif