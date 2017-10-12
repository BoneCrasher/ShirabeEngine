#ifndef __SHIRABE_IRESOURCEPROXY_H__
#define __SHIRABE_IRESOURCEPROXY_H__

#include <any>
#include <map>
#include <vector>

#include "Core/EngineTypeHelper.h"
#include "Resources/System/Core/EResourceType.h"
#include "Resources/System/Core/Handle.h"
#include "Resources/System/Core/ResourceHierarchy.h"
#include "Resources/System/Core/IResourceDescriptor.h"

namespace Engine {
	namespace Resources {

		/**********************************************************************************************//**
		 * \class	AnyProxy
		 *
		 * \brief	The AnyProxy class is basically an std::any to allow any kind of proxy
		 * 			being provided in a map.
		 **************************************************************************************************/
		using AnyProxy          = std::any;
		using ResourceProxyList = std::vector<AnyProxy>;
		using ResourceProxyMap  = std::map<ResourceHandle, AnyProxy>;


		/**********************************************************************************************//**
		 * \enum	ELoadState
		 *
		 * \brief	Represents the current state of a platform resource.
		 **************************************************************************************************/
		enum class ELoadState
		{
			UNKNOWN  = 0,
			UNLOADED,
			UNLOADING,
			LOADED,
			LOADING,
			AVAILABLE,
			UNAVAILABLE
		};

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

		virtual EProxyType proxyType() const = 0;
		virtual ELoadState loadState() const = 0;

		virtual ResourceHandleList dependencies() const = 0;

		virtual bool loadSync(
			const ResourceHandle   &inHandle,
			const ResourceProxyMap &inDependencies) = 0;
		virtual bool unloadSync() = 0;

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
		DeclareDerivedTemplatedInterface(IResourceProxy, Template(IResourceProxy<type, subtype>), IResourceProxyBase);

			virtual bool destroy() = 0;

		private:
			// friend class ProxyTreeCreator<type, subtype>;

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


	template <EResourceType type, EResourceSubType subtype>
	class GenericProxyBase
		: public Engine::Resources::IResourceProxy<type, subtype>
	{
	public:
		inline GenericProxyBase(
			const EProxyType                        &proxyType,
			const ResourceDescriptor<type, subtype> &descriptor)
			: Engine::Resources::IResourceProxy<type, subtype>()
			, _type(proxyType)
			, _loadState(ELoadState::UNKNOWN)
			, _descriptor(descriptor)
			, _dependencies()
		{
		}

		inline EProxyType proxyType() const { return _type; }
		inline ELoadState loadState() const { return _loadState; }

		inline const ResourceDescriptor<type, subtype>& descriptor()   const { return _descriptor; }
		inline const ResourceHandleList&                dependencies() const { return _dependencies; }

		inline bool destroy() { return unloadSync(); }

	protected:
		inline void setLoadState(const ELoadState& newLoadState) { _loadState = newLoadState; }

		//
		// IOC::Subject<IGFXAPIResourceCallback>
		//
		// Nothing to specify, implemented as base-class.

	private:
		EProxyType _type;
		ELoadState _loadState;

		ResourceDescriptor<type, subtype> _descriptor;
		ResourceHandleList                _dependencies;
	};

	}
}

#endif