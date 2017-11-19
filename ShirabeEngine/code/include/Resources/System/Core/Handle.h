#ifndef __SHIRABE_RESOURCES_HANDLE_H__
#define __SHIRABE_RESOURCES_HANDLE_H__

#include "Core/EngineTypeHelper.h"
#include "Core/Random.h"

#include "Resources/System/Core/EResourceType.h"

namespace Engine {
	namespace Resources {

		class ResourceHandle {
		public:
			/** \brief Unique numeric identifier of engine resources. */
			using ResourceID_t = uint64_t;

			inline ResourceHandle()
				: _id(ResourceHandle::randomIdGenerator())
				, _name("")
				, _type(EResourceType::UNKNOWN)
				, _subtype(EResourceSubType::UNKNOWN)
			{}

			inline ResourceHandle(const ResourceHandle& other)
				: _id(other.id())
				, _name(other.name())
				, _type(other.type())
				, _subtype(other.subtype())
			{ }

			inline ResourceHandle(
				const std::string      &name,
				const EResourceType    &type,
				const EResourceSubType &subtype
			)
				: _id(ResourceHandle::randomIdGenerator())
				, _name(name)
				, _type(type)
				, _subtype(subtype)
			{ }

			virtual inline ~ResourceHandle() { }

			inline operator ResourceID_t() const {
				return id();
			}

			inline ResourceID_t id() const {
				return _id;
			}

			inline std::string name() const {
				return _name;
			}

			inline EResourceType type() const {
				return _type;
			}

			inline EResourceSubType subtype() const {
				return _subtype;
			}

      static const ResourceHandle Invalid() {
        static ResourceHandle g_nullHandle("NULL", EResourceType::UNKNOWN, EResourceSubType::UNKNOWN);

        return g_nullHandle;
      }

      static bool IsValid(const ResourceHandle& handle) {
        return !(handle == Invalid());
      }

      inline bool valid() const { return ResourceHandle::IsValid(*this); }

		private:
			inline static ResourceID_t randomIdGenerator() {
				static Random::RandomState _randomIdGenerator; // Todo: CRTP. dervie the random generator from something to make sure it is unique in a context, not system global.
				return _randomIdGenerator.next();
			}
  
			ResourceID_t     _id;
			std::string      _name;
			EResourceType    _type;
			EResourceSubType _subtype;
		};
		DeclareListType(ResourceHandle, ResourceHandle);
		
		static bool operator== (
			const ResourceHandle& l,
			const ResourceHandle& r)
		{
			return (l.id() == r.id());
		}
	}
}

namespace std {
	using namespace Engine::Resources;

	template <>
	struct hash<ResourceHandle> {
		size_t operator()(const ResourceHandle& k) const
		{
			typedef typename std::underlying_type<EResourceType>::type    resource_type;
			typedef typename std::underlying_type<EResourceSubType>::type resource_sub_type;
			// Compute individual hash values for two data members and combine them using XOR and bit shifting
			return (
				hash<ResourceHandle::ResourceID_t>()(k.id())
				^ (
				((uint32_t) hash<resource_type>()(((resource_type) k.type())) << 16)
					| hash<resource_sub_type>()(((resource_sub_type) k.subtype()))
					)
				<< 1
				);
		}
	};
}


#endif