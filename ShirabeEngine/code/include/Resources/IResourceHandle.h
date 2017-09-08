#ifndef __SHIRABE_IRESOURCEHANDLE_H__
#define __SHIRABE_IRESOURCEHANDLE_H__

#include <stdint.h>
#include <string>
#include <map>

#include "Core/EngineTypeHelper.h"
#include "Core/EngineStatus.h"

#include "Resources/EResourceType.h"

namespace Engine {
	namespace Resources {

		/** \brief Unique numeric identifier of engine resources. */
		using IResourceID_t = uint64_t;

		DeclareInterface(IResourceHandle);

		virtual IResourceID_t    id()      const = 0;
		virtual std::string      name()    const = 0;
		virtual EResourceType    type()    const = 0;
		virtual EResourceSubType subtype() const = 0;

		virtual bool handleValid() const = 0;

		DeclareInterfaceEnd(IResourceHandle);
		DeclareSharedPointerType(IResourceHandle);

		static bool operator== (
			const IResourceHandlePtr& l,
			const IResourceHandlePtr& r)
		{
			return (l->id() == r->id());
		}
	}
}

namespace std {
	using namespace Engine::Resources;

	template <>
	struct hash<IResourceHandlePtr> {
		size_t operator()(const IResourceHandlePtr& k) const
		{
			typedef typename std::underlying_type<EResourceType>::type    resource_type;
			typedef typename std::underlying_type<EResourceSubType>::type resource_sub_type;
			// Compute individual hash values for two data members and combine them using XOR and bit shifting
			return (
				hash<IResourceID_t>()(k->id())
				^ (
					((uint32_t) hash<resource_type>()( ((resource_type) k->type()) ) << 16 )
					| hash<resource_sub_type>()( ((resource_sub_type)k->subtype()) ) 					
				  )
			    << 1
				);
		}
	};
}

#endif