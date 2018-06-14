#include "Core/Memory/AllocatorBase/AllocatorBase.h"

namespace Engine
{
	namespace Memory
	{
		AllocatorBase::AllocatorBase()
		{

		}

		AllocatorBase::~AllocatorBase()
		{

		}

		void *AllocatorBase::allocate( const size_t size)
		{
			void *ptr = malloc(size);
			if (ptr == 0)
				throw new std::bad_alloc();

			return reinterpret_cast<void *>(ptr);
		}

		void AllocatorBase::deallocate( void *ptr)
		{
			if (ptr)
				free(ptr);
		}

		size_t AllocatorBase::max_size() const
		{
			return std::numeric_limits<size_t>::max();
		}
	}
}