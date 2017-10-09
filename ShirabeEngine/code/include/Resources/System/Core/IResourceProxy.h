#ifndef __SHIRABE_IRESOURCEPROXY_H__
#define __SHIRABE_IRESOURCEPROXY_H__

#include <any>
#include <map>

#include "Core/EngineTypeHelper.h"
#include "Resources/System/Core/EResourceType.h"
#include "Resources/System/Core/Handle.h"
#include "Resources/System/Core/IResourceDescriptor.h"

namespace Engine {
	namespace Resources {		

		/**********************************************************************************************//**
		 * \class	AnyProxy
		 *
		 * \brief	The AnyProxy class is basically an std::any to allow any kind of proxy
		 * 			being provided in a map.
		 **************************************************************************************************/
		class AnyProxy
			: public std::any
		{ };

		/**********************************************************************************************//**
		 * \class	ProxyCreator
		 *
		 * \brief	A proxy creator.
		 *
		 * \tparam	type   	Type of the type.
		 * \tparam	subtype	Type of the subtype.
		 **************************************************************************************************/
		template <EResourceType type, EResourceSubType subtype>
		class ProxyCreator {
			static Ptr<IResourceProxy<type, subtype>> create(const ResourceDescriptor<type, subtype>& desc) {
				throw std::exception("Proxy creation undefined for unspecialized type and subtype.");
			}
		};

		using ResourceProxyMap = std::map<ResourceHandle, AnyProxy>;

		/**********************************************************************************************//**
		 * \enum	EProxyType
		 *
		 * \brief	Values that represent proxy types
		 **************************************************************************************************/
		enum class EProxyType {
			Unknown    = 0,
			Internal,
			Persistent,
			Dynamic
		};

		/**********************************************************************************************//**
		 * \fn	DeclareInterface(IResourceProxyBase);
		 *
		 * \brief	Constructor
		 *
		 * \param	parameter1	The first parameter.
		 **************************************************************************************************/
		DeclareInterface(IResourceProxyBase);

		virtual EProxyType         type()      const = 0;
		virtual ELoadState         loadState() const = 0;

		virtual ResourceHandleList dependencies() const = 0;

		virtual bool loadSync(
			const ResourceHandle  &inHandle, 
			const ResourceProxyMap&inDependencies) = 0;
		virtual bool unloadSync() = 0;
		// TODO: Consider loadAsync returning a future/promise.

		DeclareInterfaceEnd(IResourceProxyBase);
		DeclareSharedPointerType(IResourceProxyBase);

		/**********************************************************************************************//**
		 * \fn	template <EResourceType type, EResourceSubType subtype> DeclareInterface(IResourceProxy);
		 *
		 * \brief	Copy constructor
		 *
		 * \tparam	type   	Type of the type.
		 * \tparam	subtype	Type of the subtype.
		 * \param	parameter1	The first parameter.
		 **************************************************************************************************/
		template <EResourceType type, EResourceSubType subtype>
		DeclareDerivedInterface(IResourceProxy, IResourceProxyBase);

		virtual bool load(ResourceProxyMap& dependencies, ResourceProxyMap& outResources) = 0;
		virtual bool unload()                                                             = 0;
		virtual bool destroy()                                                            = 0;

	private:
		friend class ProxyCreator<type, subtype>;

		virtual bool create(const ResourceDescriptor<type, subtype>& desc) = 0;
		DeclareInterfaceEnd(IResourceProxy);

		/**********************************************************************************************//**
		 * \fn	template <EResourceType type, EResourceSubType subtype> DeclareTemplatedSharedPointerType(IResourceProxy, Template(IResourceProxy<type, subtype>));
		 *
		 * \brief	Constructor
		 *
		 * \tparam	type   	Type of the type.
		 * \tparam	subtype	Type of the subtype.
		 * \param	parameter1	The first parameter.
		 * \param	parameter2	The second parameter.
		 **************************************************************************************************/
		template <EResourceType type, EResourceSubType subtype>
		DeclareTemplatedSharedPointerType(IResourceProxy, Template(IResourceProxy<type, subtype>));

		static Ptr<IResourceProxyBase> BaseProxyCast(const AnyProxy& proxy) {
			return std::any_cast<Ptr<IResourceProxyBase>>(proxy);
		}

		/**********************************************************************************************//**
		 * \fn	template <EResourceType type, EResourceSubType subtype> static Ptr<IResourceProxy<type, subtype>> ProxyCast(Ptr<AnyProxy>& proxy)
		 *
		 * \brief	Proxy cast
		 *
		 * \tparam	type   	Type of the type.
		 * \tparam	subtype	Type of the subtype.
		 * \param [in,out]	proxy	The proxy.
		 *
		 * \return	A Ptr&lt;IResourceProxy&lt;type,subtype&gt;&gt;
		 **************************************************************************************************/
		template <EResourceType type, EResourceSubType subtype>
		static Ptr<IResourceProxy<type, subtype>> ProxyCast(const AnyProxy& proxy) {
			return std::any_cast<Ptr<IResourceProxy<type, subtype>>>(proxy);
		}

	}
}

#endif