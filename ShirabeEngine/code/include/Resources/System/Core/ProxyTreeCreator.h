#ifndef __SHIRABE_PROXYTREECREATOR_H__
#define __SHIRABE_PROXYTREECREATOR_H__

#include "Resources/System/Core/ResourceProxyFactory.h"

namespace Engine {
	namespace Resources {

		/**********************************************************************************************//**
		 * \class	ProxyCreator
		 *
		 * \brief	A proxy creator.
		 *
		 * \tparam	type   	Type of the type.
		 * \tparam	subtype	Type of the subtype.
		 **************************************************************************************************/
		template <EResourceType type, EResourceSubType subtype, typename TResourceBinding>
		class ProxyTreeCreator {
			static bool create(
				const Ptr<ResourceProxyFactory>         &proxyFactory,
				const ResourceDescriptor<type, subtype> &desc,
				ResourceHandleList                      &inDependencyHandles,
				TResourceBinding                        &outBinding,
				DependerTreeNodeList                    &outResourceHierarchy)
			{
				throw std::exception("Proxy creation undefined for unspecialized type and subtype.");
			}
		};

	}
}

#endif