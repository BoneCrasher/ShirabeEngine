#ifndef __SHIRABE_PROXYTREECREATOR_H__
#define __SHIRABE_PROXYTREECREATOR_H__

#include "Resources/System/Core/ResourceProxyFactory.h"

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

			using resource_type    = typename TResource::resource_type;
			using resource_subtype = typename TResource::resource_subtype;
			using binding_type     = typename TResource::binding_type;

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
				const Ptr<ResourceProxyFactory>     &proxyFactory,
				const ResourceDescriptor<TResource> &desc,
				ResourceHandleList                  &inDependencyHandles,
				binding_type                        &outBinding,      
				ResourceProxyMap                    &outProxyMap,        
				DependerTreeNodeList                &outResourceHierarchy)
			{
				throw std::exception("Proxy creation undefined for unspecialized type and subtype.");
			}
		};

	}
}

#endif