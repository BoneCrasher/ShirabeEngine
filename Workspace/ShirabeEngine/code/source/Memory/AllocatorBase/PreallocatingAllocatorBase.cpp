#include "Memory/AllocatorBase/PreallocatingAllocatorBase.h"

namespace Engine
{
	namespace Memory
	{

		PreallocatingAllocatorBase::PreallocatingAllocatorBase() 
			: AllocatorBase(), m_pStart(NULL), m_committed(0), m_capacity(0)
		{
			this->__prealloc(PREALLOC_BYTE_SIZE);
		}


		PreallocatingAllocatorBase::~PreallocatingAllocatorBase()
		{
			this->__free();
		}

#pragma region Internal Memory Management Functionality

		/* PREALLOCATION 
		 * 
		 * Preallocation requires to have a preallocation size when allocating the initial 
		 * memory block on allocator creation. This block can be committed to runtime objects 
		 * arbitrarily but there are two specific cases, when commitment can exceed the 
		 * available capacity:
		 *
		 *   1. The new runtime object memory should be allocated is larger than the remaining 
		 *      capacity.
		 *   2. Although the object fits into the remaining space, its alignment causes the 
		 *      object to exceed the current memory block boundaries.
		 * 
		 * To deal with reallocation should be the responsibility of derviates of this class
		 * since different allocator types have to deal differently with reallocation.
		 * In the best case reallocation is avoided entirely using sub-block allocations or similar techs.
		 */

		void PreallocatingAllocatorBase::__prealloc( size_t preallocSize)
		{
			this->m_pStart = malloc(preallocSize);
			if (this->m_pStart == 0)
				throw new std::bad_alloc();

			m_capacity = preallocSize;
			m_committed = 0;
		}


#ifndef SHIRABE_MEMORY__USE_REALLOC
#define SHIRABE_MEMORY__USE_REALLOC 0

#if SHIRABE_MEMORY__USE_REALLOC == 1

		// Grow the capacity of this allocator using realloc.
		// This implementation will require a pointer update mechanism.op_'Ü
		void PreallocatingAllocatorBase::__grow( size_t requiredSize)
		{
		}

		// Shrink the capacity of this allocator using realloc.
		// This implementation will require a pointer update mechanism.
		void PreallocatingAllocatorBase::__shrink( size_t keepSize)
		{
		}
#else 
		// Grow the capacity of this allocator using sub-block management.
		void PreallocatingAllocatorBase::__grow( size_t requiredSize)
		{
			
		}

		// Shrink the capacity of this allocator using sub-block management.
		void PreallocatingAllocatorBase::__shrink( size_t keepSize)
		{
		}
#endif
#endif

		void  PreallocatingAllocatorBase::__free()
		{
			if (this->m_pStart)
				free(m_pStart);

			m_committed = m_capacity = 0;
		}

#pragma region Alignment

		/* MEMORY (ADDRESS) ALIGNMENT 
		 *
		 * Memory alignment is important to ensure proper r/w-access to memory by the CPU.
		 * Depending on the CPU architecture or model different alignment base values are
		 * recommended depending on the actual data size to be allocated and stored.
		 * 
		 * For each case there are at least two methods to deal with memory address alignment:
		 *
		 * __align: Immediately aligns a passed memory address to a proper address corresponding to a 
		 *          passed alignment value. 
		 *
		 *			This method can be realized in at least two ways:
		 *			a) Adding (alignment - 1) to the address and masking of the least significant bits (~(alignment - 1))
		 *				-> aligned = (address + (alignment - 1)) & (~(alignment - 1))
		 *			b) Adding an alignment offset (address & (alignment - 1)) to the address
		 *				-> aligned = address + (address & (alignment - 1))
		 *     		
		 *			--> b) is only valid if alignment is a power of two value
		 *
		 * __getAlignmentOffset: 
	     *  	    Calculates an alignment offset value, which can be added to an memory address value to align it.
		 *          The calculation is equal to Method b of __align, except for the final addition:
		 *			-> offset = (address & (alignment - 1))
		 *			
		 *			--> Only valid if alignment is a power of two value.
		 */

#ifndef SHIRABE_MEMORY__USE_ALIGNMENT_OFFSET 
#define SHIRABE_MEMORY__USE_ALIGNMENT_OFFSET 1
#endif

		// Calculate the required offset to properly align <pAddr> to a desired <alignment>.
		size_t PreallocatingAllocatorBase::__getAlignmentOffset( void *pAddr,  size_t alignment)
		{
			return (reinterpret_cast<size_t>(pAddr) & static_cast<size_t>(alignment - 1));
		}

		
#if SHIRABE_MEMORY__USE_ALIGNMENT_OFFSET == 1
		// Align <pAddr> to a desired <alignment> using an alignment offset and return the new pointer!
		void* PreallocatingAllocatorBase::__align( void *pAddr,  size_t alignment)
		{
			return (void *)(reinterpret_cast<uintptr_t>(pAddr) + this->__getAlignmentOffset(pAddr, alignment));
		}
#else 
		// Align <pAddr> to a desired <alignment> by adding <alignment - 1> and masking of its least significant bits. 
		// The aligned pointer is returned.
		void* PreallocatingAllocatorBase::__align( void *pAddr,  size_t alignment)
		{
			return (void *)((reinterpret_cast<uintptr_t>(pAddr)+static_cast<uintptr_t>(alignment - 1)) & static_cast<uintptr_t>(~(alignment - 1)));
		}
#endif		

#pragma endregion
#pragma endregion

	}
}