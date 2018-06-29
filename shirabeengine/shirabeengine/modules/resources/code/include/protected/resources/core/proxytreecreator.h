#ifndef __SHIRABE_PROXYTREECREATOR_H__
#define __SHIRABE_PROXYTREECREATOR_H__

#include "Resources/Core/ResourceProxyFactory.h"

namespace Engine {
	namespace Resources {


		/**********************************************************************************************//**
		 * \class	ProxyTreeCreator
		 *
		 * \brief	A proxy creator.
		 *
		 * \tparam	type				Type of the type.
		 * \tparam	subtype				Type of the subtype.
		 * \tparam	TResourceBinding	Type of the resource binding.
		 *
		 * ### tparam	type   	Type of the type.
		 * ### tparam	subtype	Type of the subtype.
		 **************************************************************************************************/
		template <typename TResource>
		class ProxyTreeCreator {
    public:

			/**********************************************************************************************//**
			 * \fn	static bool ProxyTreeCreator::create( const Ptr<ResourceProxyFactory> &proxyFactory, const ResourceDescriptor<type, subtype> &desc, ResourceHandleList &inDependencyHandles, TResourceBinding &outBinding, ResourceProxyMap &outProxyMap, DependerTreeNodeList &outResourceHierarchy)
			 *
			 * \brief	Creates a new bool
			 *
			 * \exception	std::exception	Thrown when an exception error condition occurs.
			 *
			 * \param 		  	proxyFactory			ProxyFactory-instance used to create proxies hooked up with their respective ResourceSubsystems
			 * \param 		  	desc					Type specific descriptor used by the proxy creator to create all structures.
			 * \param [in,out]	inDependencyHandles 	ResourceHandle-instances referring all pre-created dependency resources necessary.
			 * \param [in,out]	outBinding				The type specific binding object initialized with all necessary handles.
			 * \param [in,out]	outProxyMap				A generic container providing mappings from ResourceHandle -> AnyProxy for Resource Manager storage.
			 * \param [in,out]	outResourceHierarchy	Hierarchy of resources (root) and their dependers (children). Used to ensure proper destruction order.
			 *
			 * \return	True if it succeeds, false if it fails.
			 **************************************************************************************************/
			static bool create(
				//
				Ptr<ResourceProxyFactory>           const&proxyFactory,
				typename TResource::CreationRequest const&request,
        typename TResource::Binding              &outBinding,
				ResourceProxyMap                         &outProxyMap,
				DependerTreeNodeList                     &outResourceHierarchy)
			{
        static_assert(false, "Proxy creation undefined for unspecialized type and subtype.");
				// throw std::exception("Proxy creation undefined for unspecialized type and subtype.");
			}
		};

	}
}

#endif