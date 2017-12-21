#ifndef __SHIRABE_DX11SRVPROXYCREATOR_H__
#define __SHIRABE_DX11SRVPROXYCREATOR_H__

#include "Core/EngineStatus.h"
#include "Log/Log.h"

#include "Resources/System/Core/Handle.h"
#include "Resources/System/Core/IResourceProxy.h"
#include "Resources/System/Core/ResourceProxyFactory.h"
#include "Resources/System/Core/ProxyTreeCreator.h"

#include "GFXAPI/Types/ShaderResourceView.h"

namespace Engine {
	namespace Resources {

		template <>
		class ProxyTreeCreator<ShaderResourceView> {
		public:
			static const constexpr EResourceType    resource_type    = ShaderResourceView::resource_type;
			static const constexpr EResourceSubType resource_subtype = ShaderResourceView::resource_subtype;
      
			static bool create(
				const Ptr<ResourceProxyFactory>           &proxyFactory,
				const ShaderResourceView::CreationRequest &request,
				ResourceHandleList                        &inDependencyHandles,
        ShaderResourceView::Binding               &outBinding,
				ResourceProxyMap                          &outProxyMap,
				DependerTreeNodeList                      &outResourceHierarchy)
			{
				throw std::exception("Proxy creation undefined for unspecialized type and subtype.");
			}
		};

	}

}

#endif