#ifndef __SHIRABE_IRESOURCEPOOL_H__
#define __SHIRABE_IRESOURCEPOOL_H__

#include <map>

#include "Core/EngineTypeHelper.h"

namespace Engine {
	namespace Resources {

		template <typename TIndex, typename TValue>
		DeclareTemplatedInterface(IIndexedResourcePool, Template(IIndexedResourcePool<TIndex, TValue>));

		typedef TIndex index_type;
		typedef TValue value_type;

		typedef std::map < index_type, value_type >            internal_storage_type;
		typedef typename internal_storage_type::const_iterator const_iterator;
		typedef typename internal_storage_type::iterator       iterator;

		virtual bool addResource(const TIndex& index, const TValue& resource) = 0;
		virtual bool removeResource(const TIndex& index)                      = 0;

		virtual bool          hasResource(const TIndex& index) const = 0;
		virtual const TValue& getResource(const TIndex& index) const = 0;
		virtual       TValue& getResource(const TIndex& index)       = 0;

		virtual const_iterator begin() const = 0;
		virtual iterator       begin()       = 0;

		virtual const_iterator end() const = 0;
		virtual const_iterator end()       = 0;

		DeclareInterfaceEnd(IIndexedResourcePool);

		template <typename TIndex, typename TValue>
		DeclareTemplatedSharedPointerType(IIndexedResourcePool, Template(IIndexedResourcePool<TIndex, TValue>));
	}
}

#endif