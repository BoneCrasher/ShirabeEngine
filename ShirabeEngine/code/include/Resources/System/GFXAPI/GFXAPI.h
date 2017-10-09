#ifndef __SHIRABE_GFXAPI_H__
#define __SHIRABE_GFXAPI_H__

#include <type_traits>
#include <typeinfo>
#include <optional>
#include <stdint.h>

#include "Core/EngineTypeHelper.h"
#include "Core/EngineStatus.h"
#include "IOC/Observer.h"

#include "Resources/System/Core/IResourceProxy.h"

namespace Engine
{
	namespace GFXAPI
	{

		/**********************************************************************************************//**
		 * \typedef	unsigned int GFXAPIResourceHandle_t
		 *
		 * \brief	Defines an alias representing the platform resource handle t
		 **************************************************************************************************/
		using GFXAPIResourceHandle_t = uint64_t;

		DeclareListType(GFXAPIResourceHandle_t, GFXAPIResourceHandle);

		/**********************************************************************************************//**
		 * \class	IGFXAPIResourceCallback
		 *
		 * \brief	Defines platform resource callback to handle life-cycle and perform events
		 * 			on load/unload or structure change.
		 **************************************************************************************************/
		DeclareInterface(IGFXAPIResourceCallback);

		virtual void onResourceAvailable()   = 0;
		virtual void onResourceUnavailable() = 0;

		virtual void onLoad()   = 0;
		virtual void onUnload() = 0;

		virtual void onSubresourceAdded(const GFXAPIResourceHandle_t&)   = 0;
		virtual void onSubresourceRemoved(const GFXAPIResourceHandle_t&) = 0;

		DeclareInterfaceEnd(IGFXAPIResourceCallback);

	#define DefineStorageCondition(sz)                            \
		template <typename T>	                                  \
		struct u##sz##StorageCondition	                          \
		{						                                  \
			static constexpr bool value                           \
                = std::is_integral<T>::value                      \
                  && sizeof(T) == sizeof(uint##sz##_t)            \
                  && std::is_convertible<T, uint##sz##_t>::value; \
		};     


	#define DefineSetter(sz)                                                              \
		template <typename T>												              \
		void setValue(const std::enable_if_t <u##sz##StorageCondition<T>::value, T >& in) \
		{	                                                                              \
		    const std::type_info& info = typeid(T);                                       \
			_typeInfo      = GFXAPIResourceHolder::RTTI(info);	    	 	              \
			_handle._u##sz = static_cast<uint##sz##_t>(in);					              \
		}

	#define DefineField(sz) \
		uint##sz##_t _u##sz;

		DefineStorageCondition(32);
		DefineStorageCondition(64);
		
		class GFXAPIResourceHolder
		{
		public:
			struct RTTI
			{
				RTTI(const std::type_info& in) : info(&in) {}

				const std::type_info *info;
			};

			GFXAPIResourceHolder()
				: _handle((uint64_t)0)
				, _typeInfo(typeid(uint8_t))
			{
			}

			DefineSetter(32);
			DefineSetter(64);

			template <typename T>
			void setValue(const std::enable_if_t<std::is_pointer_v<T>, T>& v)
			{
				setValue<uint64_t>(reinterpret_cast<uint64_t>(static_cast<void *>(v)));
			}

			template <typename T>
			std::optional<std::enable_if_t<std::is_pointer_v<T>, T>> getValue()
			{
				if( !strcmp(_typeInfo.info->name, typeid(T).name) == 0 )
					return std::optional<T>();

				return std::optional<T>(static_cast<T>(reinterpret_cast<void *>(_handle._u64)));
			}

			template <typename T>
			std::optional < std::enable_if_t < std::is_integral_v<T> && sizeof(uint32_t) == sizeof(T) , T >> getValue()
			{
				if( !strcmp(_typeInfo.info->name, typeid(T).name) == 0 )
					return std::optional<T>();

				return  std::optional<T>(static_cast<T>(_handle._u32));
			}

			template <typename T>
			std::optional < std::enable_if_t < std::is_integral_v<T> && sizeof(uint64_t) == sizeof(T), T >> getValue()
			{
				if( !strcmp(_typeInfo.info->name, typeid(T).name) == 0 )
					return std::optional<T>();

				return  std::optional<T>(static_cast<T>(_handle._u64));
			}

		private:
			union HandleStorage
			{
				DefineField(32); // GL-Handle type
				DefineField(64); // Non 64-bit Vk handle or 64-bit system pointer size type.

				HandleStorage()           : _u32(0) {}
				HandleStorage(uint32_t v) : _u32(v) {}
				HandleStorage(uint64_t v) : _u64(v) {}

			} _handle;

			RTTI _typeInfo;
		};

		struct test
		{

		};

		void foo()
		{
			test t;
			GFXAPIResourceHolder holder;
			holder.setValue<test *>(&t);
		}

		/**********************************************************************************************//**
		 * \class	GFXAPIResourceAdapter
		 *
		 * \brief	Simple storage base class to hold a GFXAPIResourceHandle_t and make it accessible.
		 **************************************************************************************************/
		class GFXAPIResourceAdapter
		{
		public:
			inline const GFXAPIResourceHandle_t& handle() const { return _handle; }

		protected:
			inline GFXAPIResourceAdapter(const GFXAPIResourceHandle_t& handle)
				: _handle(handle)
			{ }

		private:
			GFXAPIResourceHandle_t _handle;
		};

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
		 * \class	PlatformResourceProxy
		 *
		 * \brief	A platform resource wrapper.
		 **************************************************************************************************/
		template <EResourceType type, EResourceSubType subtype>
		class GFXAPIResourceProxy
			: public Engine::Resources::IResourceProxy<type, subtype>
		{
		public:
			inline GFXAPIResourceProxy()
				: Engine::Resources::IResourceProxy<type, subtype>()
				, _type(EProxyType::Unknown)
				, _loadState(ELoadState::UNKNOWN)
				, _dependencies()
			{
			}

			inline EProxyType type()      const { return _type;      }
			inline ELoadState loadState() const { return _loadState; }

			inline ResourceHandleList dependencies() const { return _dependencies; }

			bool loadSync(
				const ResourceHandle  &inHandle,
				const ResourceProxyMap&inDependencies);
			bool unloadSync();

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

			ResourceHandleList _dependencies;
		};

		template <EResourceType type, EResourceSubType subtype>
		bool GFXAPIResourceProxy<type, subtype>
			::loadSync(
				const ResourceHandle  &inHandle,
				const ResourceProxyMap&inDependencies)
		{
			return true;
		}

		template <EResourceType type, EResourceSubType subtype>
		bool GFXAPIResourceProxy<type, subtype>
			::unloadSync()
		{
			return true;
		}
	}
}

#endif