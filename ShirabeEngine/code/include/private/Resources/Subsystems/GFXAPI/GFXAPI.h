#ifndef __SHIRABE_GFXAPI_H__
#define __SHIRABE_GFXAPI_H__

#include <type_traits>
#include <typeinfo>
#include <optional>
#include <stdint.h>
#include <map>
#include <vector>

#include "Core/EngineTypeHelper.h"
#include "Core/EngineStatus.h"

#include "IOC/Observer.h"
#include "Resources/Core/ResourceDTO.h"

namespace Engine {
	namespace GFXAPI {

		using namespace Engine::Resources;

		/**********************************************************************************************//**
		 * \typedef	unsigned int GFXAPIResourceHandle_t
		 *
		 * \brief	Defines an alias representing the platform resource handle t
		 **************************************************************************************************/
		using GFXAPIResourceHandle_t = SubjacentResourceId_t;
		static const GFXAPIResourceHandle_t GFXAPIUninitializedResourceHandle = 0;

    DeclareListType(GFXAPIResourceHandle_t, GFXAPIResourceHandle);

    struct GFXAPIResourceHandleAssignment {
      GFXAPIResourceHandle_t publicHandle;
      Ptr<void>              internalHandle;

      inline GFXAPIResourceHandleAssignment() 
        : publicHandle(0)
        , internalHandle(0) 
      {}

      inline bool valid() const { return (publicHandle && internalHandle); }
    };

		DeclareListType(GFXAPIResourceHandle_t, GFXAPIResourceHandle);
		DeclareMapType(PublicResourceId_t, GFXAPIResourceHandle_t, GFXAPIResourceHandle);

    static bool CheckValidHandle(GFXAPIResourceHandle_t const& h) {
      return (h > 0); // For now this is the only condition...
    }
		
	#define DefineStorageCondition(sz)                              \
		template <typename T>	                                        \
		struct u##sz##StorageCondition	                              \
		{						                                                  \
			static constexpr bool value                                 \
                = std::is_integral<T>::value                      \
                  && sizeof(T) == sizeof(uint##sz##_t)            \
                  && std::is_convertible<T, uint##sz##_t>::value; \
		};     


	#define DefineSetter(sz)                                                            \
		template <typename T>												                                      \
		void setValue(const std::enable_if_t <u##sz##StorageCondition<T>::value, T >& in) \
		{	                                                                                \
		  const std::type_info& info = typeid(T);                                         \
			m_typeInfo      = GFXAPIResourceHolder::RTTI(info);	    	 	                    \
			m_handle.m_u##sz = static_cast<uint##sz##_t>(in);					                      \
		}

	#define DefineField(sz) \
		uint##sz##_t m_u##sz;

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
				: m_handle((uint64_t)0)
				, m_typeInfo(typeid(uint8_t))
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

				return std::optional<T>(static_cast<T>(reinterpret_cast<void *>(_handle.m_u64)));
			}

			template <typename T>
			std::optional < std::enable_if_t < std::is_integral_v<T> && sizeof(uint32_t) == sizeof(T) , T >> getValue()
			{
				if( !strcmp(_typeInfo.info->name, typeid(T).name) == 0 )
					return std::optional<T>();

				return  std::optional<T>(static_cast<T>(_handle.m_u32));
			}

			template <typename T>
			std::optional < std::enable_if_t < std::is_integral_v<T> && sizeof(uint64_t) == sizeof(T), T >> getValue()
			{
				if( !strcmp(_typeInfo.info->name, typeid(T).name) == 0 )
					return std::optional<T>();

				return  std::optional<T>(static_cast<T>(_handle.m_u64));
			}

		private:
			union HandleStorage
			{
				DefineField(32); // GL-Handle type
				DefineField(64); // Non 64-bit Vk handle or 64-bit system pointer size type.

				HandleStorage()           : m_u32(0) {}
				HandleStorage(uint32_t v) : m_u32(v) {}
				HandleStorage(uint64_t v) : m_u64(v) {}

			} m_handle;

			RTTI m_typeInfo;
		};		
	}
}

#endif