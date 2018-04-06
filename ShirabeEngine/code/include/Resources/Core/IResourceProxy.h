#ifndef __SHIRABE_IRESOURCEPROXY_H__
#define __SHIRABE_IRESOURCEPROXY_H__

#include <any>
#include <map>
#include <vector>

#include "Core/EngineTypeHelper.h"
#include "Core/EngineStatus.h"

#include "Resources/Core/EResourceType.h"
#include "Resources/Core/ResourceDTO.h"
#include "Resources/Core/ResourceDomainTransfer.h"

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
		using ResourceProxyMap  = std::map<PublicResourceId_t, AnyProxy>;


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

    virtual SubjacentResourceId_t subjacentResourceId() const = 0;
    
		virtual EEngineStatus loadSync(
      SubjacentResourceIdList const&inResolvedDependencies) = 0;
		virtual EEngineStatus unloadSync() = 0;

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
		template <typename TResource>
		DeclareDerivedTemplatedInterface(IResourceProxy, Template(IResourceProxy<TResource>), IResourceProxyBase);

			virtual bool destroy() = 0;

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
		template <typename TResource>
		DeclareTemplatedSharedPointerType(IResourceProxy, Template(IResourceProxy<TResource>));

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
		template <typename TResource>
		static Ptr<IResourceProxy<TResource>> ProxyCast(const AnyProxy& proxy) {
			try {
				Ptr<IResourceProxy<TResource>> tmp = std::any_cast<Ptr<IResourceProxy<TResource>>>(proxy);
				return tmp;
			} catch( std::bad_any_cast& ) {
				return nullptr;
			}
		}


		template <typename TResource>
		class GenericProxyBase
			: public Engine::Resources::IResourceProxy<TResource>
			, public ResourceCreationRequestAdapter<typename TResource::CreationRequest>
		{
		public:
			inline GenericProxyBase(
				EProxyType                          const&proxyType,
				typename TResource::CreationRequest const&request)
				: Engine::Resources::IResourceProxy<TResource>()
				, ResourceCreationRequestAdapter<typename TResource::CreationRequest>(request)
				, m_type(proxyType)
				, m_loadState(ELoadState::UNKNOWN)
			{
			}

      virtual ~GenericProxyBase() = default;

			inline EProxyType proxyType() const { return m_type; }
			inline ELoadState loadState() const { return m_loadState; }

			inline bool destroy() { return !CheckEngineError(unloadSync()); }

		protected:
			inline void setLoadState(const ELoadState& newLoadState) { m_loadState = newLoadState; }

		private:
			EProxyType m_type;
			ELoadState m_loadState;
		};

    template <typename TResource>
    static Ptr<GenericProxyBase<TResource>> GenericProxyBaseCast(const AnyProxy& proxy) {
      try {
        Ptr<GenericProxyBase<TResource>> tmp = std::any_cast<Ptr<GenericProxyBase<TResource>>>(proxy);
        return tmp;
      } catch( std::bad_any_cast& ) {
        return nullptr;
      }
    }

	}
}

#endif