/*
	Copyright 2010 - 2017 PureDev Software Ltd. All Rights Reserved.

	This file is part of VMem.

	VMem is dual licensed. For use in open source software VMem can
	be used under the GNU license. For use in commercial applications a
	license can be purchased from PureDev Software.

	If used under the GNU license VMem is free software: you can redistribute
	it and/or modify it under the terms of the GNU General Public License as
	published by the Free Software Foundation, either version 3 of the License,
	or (at your option) any later version.

	Under the GNU Public License VMem is distributed in the hope that it will
	be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with VMem. If not, see <http://www.gnu.org/licenses/>.

	VMem can only be used in commercial products if a commercial license has been purchased
	from PureDev Software. Please see http://www.puredevsoftware.com/vmem/License.htm
*/
//------------------------------------------------------------------------
#include "VMem_PCH.hpp"
#include "PhysicalPageMapper.hpp"
#include "VMemCore.hpp"

#ifdef VMEM_PLATFORM_PS4
	#include <kernel.h>
	#include <scebase_common.h>
#endif

//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

#define VMEM_CHECK_FOR_MAPPED_RANGE_DUPLICATES

//------------------------------------------------------------------------
#ifdef VMEM_SIMULATE_PS4
	namespace PS4Simulator { typedef long long off_t; }
	#define off_t PS4Simulator::off_t
	int sceKernelReserveVirtualRange(void**, size_t, int, int);
	int sceKernelAllocateMainDirectMemory(size_t, size_t, int, off_t*);
	int sceKernelCheckedReleaseDirectMemory(off_t, size_t);
	int sceKernelMapDirectMemory(void**, size_t, int, int, off_t, size_t);
	#define SCE_OK						0
	#define SCE_KERNEL_WB_ONION			0
	#define SCE_KERNEL_WC_GARLIC		1
	#define SCE_KERNEL_MAP_FIXED		(1<<1)
	#define SCE_KERNEL_MAP_NO_COALESCE	(1<<2)
	#define SCE_KERNEL_MAP_NO_OVERWRITE	(1<<3)
	#define SCE_KERNEL_PROT_CPU_RW		(1<<4)
	#define SCE_KERNEL_PROT_GPU_RW		(1<<5)
#endif

//-----------------------------------------------------------------------------
#if defined(VMEM_CUSTOM_PHYSICAL_ALLOCATOR)
	// if VMEM_CUSTOM_PHYSICAL_ALLOCATOR is defined then you must implement these 2 functions:
	//
	// VMem_AllocatePhysical must return a pointer to physical memory (for example, using 
	// sceKernelAllocateMainDirectMemory on PS4). The memory can be mapped or unmapped.
	//
	// If the memory is mapped p_virtual must be set to the mapped virtual address. VMem
	// will unmap the memory before it starts using it, and re-map it back to this address
	// when the memory is given back. If the memory is not mapped p_virtual must be set to NULL.
	// 
	// if VMEM_CUSTOM_PHYSICAL_ALLOCATOR is not defined then VMem will allocate the
	// physical memory itself from the OS (using sceKernelAllocateMainDirectMemory on PS4).
	//
	// You can return more memory than VMem asked for and VMem will use this memory. Make
	// sure that you modify the size reference passed in.
	//
	// VMEM_CUSTOM_PHYSICAL_ALLOCATOR can be useful when you already allocate memory up-front
	// for your application, and you want VMem to use that memory.
	void* VMem_AllocatePhysical(size_t& size, size_t alignment, int commit_flags, void*& p_virtual);
	void VMem_FreePhysical(void* p_physical, void* p_virtual, size_t size);
#else
	void* VMem_AllocatePhysical(size_t& size, size_t alignment, int commit_flags, void*& p_virtual)
	{
		p_virtual = NULL;

		#if defined(VMEM_PLATFORM_PS4) || defined(VMEM_SIMULATE_PS4)
			int ps4_flags = commit_flags ? commit_flags : SCE_KERNEL_WB_ONION;

			off_t offset = 0;
			if(sceKernelAllocateMainDirectMemory(size, alignment, ps4_flags, &offset) != SCE_OK)
				return NULL;

			return (void*)offset;
		#else
			VMEM_UNREFERENCED_PARAM(size);
			VMEM_UNREFERENCED_PARAM(alignment);
			VMEM_UNREFERENCED_PARAM(commit_flags);

			VMEM_BREAK("Physical page mapping not supported on this platform");
			return NULL;
		#endif
	}

	void VMem_FreePhysical(void* p_physical, void* p_virtual, size_t size)
	{
		VMEM_UNREFERENCED_PARAM(p_virtual);

		#if defined(VMEM_PLATFORM_PS4) || defined(VMEM_SIMULATE_PS4)
			sceKernelCheckedReleaseDirectMemory((off_t)p_physical, size);
		#else
			VMEM_UNREFERENCED_PARAM(p_physical);
			VMEM_UNREFERENCED_PARAM(size);

			VMEM_BREAK("Physical page mapping not supported on this platform");
		#endif
	}
#endif

//-----------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	size_t g_PhysicalBytes = 0;

	//------------------------------------------------------------------------
#if VMEM_SIMULATE_OOM
	bool SimulateOOM()
	{
		int simulate_oom_count = GetPhysicalOOMCount();
		if (simulate_oom_count > 0)
		{
			if (simulate_oom_count == 1)
				return true;
			else
				SetPhysicalOOMCount(simulate_oom_count - 1);
		}

		return false;
	}
#endif

	//------------------------------------------------------------------------
	//-------------------------- OS Specific START ---------------------------
	//------------------------------------------------------------------------

	//------------------------------------------------------------------------
	// *pp_virtual can be NULL to let the OS choose where to map to. pp_virtual will
	// then be changed to the mapping address.
	void OS_MapPages(void** pp_virtual, size_t size, void* p_physical, int page_size)
	{
		VMEM_ASSERT(size && p_physical, "VMem internal error. Bad args passed to OS_MapPages");
		VMEM_ASSERT((((uint64)*pp_virtual) % page_size) == 0, "p_virtual not aligned to page boundary");

		#if defined(VMEM_PLATFORM_PS4) || defined(VMEM_SIMULATE_PS4)

			// ====================================================
			//					PS4 Map Pages
			// ====================================================
			int protection = SCE_KERNEL_PROT_CPU_RW | SCE_KERNEL_PROT_GPU_RW;

			int flags = 0;
			if(*pp_virtual)
				flags |= SCE_KERNEL_MAP_FIXED;

			size_t alignment = *pp_virtual ? 0 : page_size;

			int result = sceKernelMapDirectMemory(pp_virtual, size, protection, flags, (off_t)p_physical, alignment);

			VMEM_ASSERT(result == SCE_OK, "must be bad args or someone else already mapped this memory (should never fail due to OOM)");
			VMEM_UNREFERENCED_PARAM(result);

		#else

			VMEM_UNREFERENCED_PARAM(pp_virtual);
			VMEM_UNREFERENCED_PARAM(size);
			VMEM_UNREFERENCED_PARAM(p_physical);
			VMEM_UNREFERENCED_PARAM(page_size);

			VMEM_BREAK("page mapping not supported on this platform");

		#endif
	}

	//------------------------------------------------------------------------
	void OS_UnmapPages(void* p_virtual, size_t size, int page_size)
	{
		VMEM_ASSERT(p_virtual && size, "PhysicalPageMapper or PhysicalPageHeap has been corrupted");
		VMEM_ASSERT((size % page_size) == 0, "PhysicalPageMapper or PhysicalPageHeap has been corrupted");

		#if defined(VMEM_PLATFORM_PS4) || defined(VMEM_SIMULATE_PS4)

			// ====================================================
			//					PS4 Unmap Pages
			// ====================================================
			unsigned int flags = SCE_KERNEL_MAP_NO_OVERWRITE | SCE_KERNEL_MAP_FIXED;
			int result = sceKernelReserveVirtualRange(&p_virtual, size, flags, page_size);
			VMEM_ASSERT(result == SCE_OK, "sceKernelReserveVirtualRange failed");
			VMEM_UNREFERENCED_PARAM(result);

		#else

			VMEM_UNREFERENCED_PARAM(p_virtual);
			VMEM_UNREFERENCED_PARAM(size);
			VMEM_UNREFERENCED_PARAM(page_size);

			VMEM_BREAK("page mapping not supported on this platform");

		#endif
	}

	//------------------------------------------------------------------------
	//--------------------------- OS Specific End ----------------------------
	//------------------------------------------------------------------------

	//------------------------------------------------------------------------
	PhysicalAlloc OS_AllocatePhysical(size_t size, size_t alignment, int page_size, int commit_flags)
	{
		VMEM_UNREFERENCED_PARAM(page_size);
		VMEM_ASSERT((size % page_size) == 0, "PhysicalPageMapper or PhysicalPageHeap has been corrupted");

		PhysicalAlloc alloc;

		#if VMEM_SIMULATE_OOM
			if(SimulateOOM())
				return alloc;
		#endif

		alloc.m_Size = size;
		alloc.mp_Physical = VMem_AllocatePhysical(alloc.m_Size, alignment, commit_flags, alloc.mp_OriginalMappedAddr);

		if(alloc.mp_Physical)
		{
			VMEM_ASSERT(alloc.m_Size >= size, "AllocatePhysical must return at least size bytes");
			VMEM_ASSERT((alloc.m_Size % page_size) == 0, "AllocatePhysical must return at least size bytes");
		}

		g_PhysicalBytes += alloc.m_Size;

		return alloc;
	}

	//------------------------------------------------------------------------
	void OS_FreePhysical(void* p_physical, void* p_virtual, size_t size)
	{
		VMem_FreePhysical(p_physical, p_virtual, size);

		g_PhysicalBytes -= size;
	}

	//------------------------------------------------------------------------
	size_t GetPhysicalBytes()
	{
		return g_PhysicalBytes;
	}

	//------------------------------------------------------------------------
	template<class T>
	void TrimPhysicalArray(T& physical_array, int page_size)
	{
		typename T::Page* p_unused_pages = NULL;
		physical_array.Trim(p_unused_pages);

		typename T::Page* p_page = p_unused_pages;
		while (p_page)
		{
			typename T::Page* p_next = p_page->mp_Next;
			OS_UnmapPages(p_page, page_size, page_size);
			p_page = p_next;
		}
	}

	//------------------------------------------------------------------------
	PhysicalPageMapper::PhysicalPageMapper(int page_size)
	:	m_PageSize(page_size),
		m_AllocatedRanges(page_size),
		m_ExcessPhysicalMemory(page_size),
		mp_InternalPages(NULL)
	{
	}

	//------------------------------------------------------------------------
	PhysicalPageMapper::~PhysicalPageMapper()
	{
		m_MappedRangeMap.Clear();
		for(int i=0; i<m_MaxPhysicalPageHeaps; ++i)
			m_PhysicalPageHeaps[i].Clear();

		VMEM_ASSERT(!m_ExcessPhysicalMemory.GetCount(), "m_ExcessPhysicalMemory has been corrupted");
		TrimPhysicalArray(m_ExcessPhysicalMemory, m_PageSize);

		void* p_internal_page = mp_InternalPages;
		while(p_internal_page)
		{
			void* p_next = *(void**)p_internal_page;
			OS_UnmapPages(p_internal_page, m_PageSize, m_PageSize);
			p_internal_page = p_next;
		}

		// this is a slightly tricky operation. Some of the pointers stored
		// in the array actually point to the arrays own memory. We need to
		// cleanly delete all of this memory, but we need to do it without
		// allocating memory ourselves. The idea is to move all pointers that
		// point to the arrays memory to the start of the list and then we
		// can safely remove the pages at the end of the array. We keep
		// doing this until there is only one page left, which can be dealt
		// with simply.
		int count = m_AllocatedRanges.GetCount();
		while(count > 1)
		{
			// trim and unmap unused pages
			TrimPhysicalArray(m_AllocatedRanges, m_PageSize);

			// free all allocations that are not being used by the array
			// the allocations that are being used move to the start of the array
			int new_count = 0;
			for(int i=0; i<count; ++i)
			{
				PhysicalAlloc alloc = m_AllocatedRanges[i];
				if(m_AllocatedRanges.PhysicalMemoryInUse(alloc))
					m_AllocatedRanges[new_count++] = alloc;
				else
					InternalFree(alloc);
			}

			count = new_count;
			m_AllocatedRanges.SetCount(count);
		}

		VMEM_ASSERT(m_AllocatedRanges.GetCount() == count, "PhysicalPageMapper has been corrupted");
		VMEM_ASSERT(count <= 1, "PhysicalPageMapper has been corrupted");

		if(count == 1)
		{
			PhysicalAlloc alloc = m_AllocatedRanges[0];

			m_AllocatedRanges.SetCount(0);
			TrimPhysicalArray(m_AllocatedRanges, m_PageSize);

			InternalFree(alloc);
		}
	}

	//------------------------------------------------------------------------
	PhysicalPageHeap* PhysicalPageMapper::GetPhysicalPageHeap(int commit_flags)
	{
		for (int i = 0; i < m_MaxPhysicalPageHeaps; ++i)
		{
			PhysicalPageHeap* p_physical_page_heap = &m_PhysicalPageHeaps[i];

			if (p_physical_page_heap->Initialised() &&
				p_physical_page_heap->GetCommitFlags() == commit_flags)
			{
				return p_physical_page_heap;
			}
		}

		for (int i = 0; i < m_MaxPhysicalPageHeaps; ++i)
		{
			PhysicalPageHeap* p_physical_page_heap = &m_PhysicalPageHeaps[i];
			if (!p_physical_page_heap->Initialised())
			{
				p_physical_page_heap->Initialise(m_PageSize, commit_flags);

#ifdef VMEM_PRE_ALLOCATE_PHYSICAL_PAGES
				PhysicalAlloc alloc = AllocatePhysical(VMEM_PRE_ALLOCATE_PHYSICAL_PAGES, false);
				VMEM_ASSERT(alloc.mp_Physical, "OOM initialising VMem!");
				FreePhysical(alloc.mp_Physical, alloc.m_Size);
#endif
				return p_physical_page_heap;
			}
		}

		VMEM_ASSERT(false, "Run out of physical page heaps. Please increase m_MaxPhysicalPageHeaps");
		return NULL;
	}

	//------------------------------------------------------------------------
	PhysicalAlloc PhysicalPageMapper::InternalAlloc(size_t size, int commit_flags)
	{
		VMEM_ASSERT((size % m_PageSize) == 0, "PhysicalPageMapper or PhysicalPageHeap has been corrupted");

		size_t alloc_size = size;

		PhysicalAlloc alloc;

		PhysicalAlloc internal_alloc;
		void* p_internal_mem = NULL;
		size_t intermal_mem_size = 0;
		if(commit_flags == VMEM_DEFAULT_COMMIT_FLAGS)
		{
			if(m_AllocatedRanges.GetCapacity() - m_AllocatedRanges.GetCount() < 2)
				alloc_size += m_PageSize;

			if(m_ExcessPhysicalMemory.GetCapacity() - m_ExcessPhysicalMemory.GetCount() < 2)
				alloc_size += m_PageSize;
		}
		else
		{
			size_t internal_size = 0;
			
			if(m_AllocatedRanges.GetCapacity() - m_AllocatedRanges.GetCount() < 2)
				internal_size += m_PageSize;

			if(m_ExcessPhysicalMemory.GetCapacity() - m_ExcessPhysicalMemory.GetCount() < 2)
				internal_size += m_PageSize;

			if(internal_size)
			{
				internal_alloc = OS_AllocatePhysical(alloc_size, m_PageSize, m_PageSize, VMEM_DEFAULT_COMMIT_FLAGS);
				if(internal_alloc.mp_Physical)
				{
					// if the memory we got back was mapped unmap it
					if (internal_alloc.mp_OriginalMappedAddr)
						OS_UnmapPages(alloc.mp_OriginalMappedAddr, internal_alloc.m_Size, m_PageSize);

					p_internal_mem = internal_alloc.mp_Physical;
					intermal_mem_size = internal_alloc.m_Size;
				}
				else
				{
					return alloc;
				}
			}
		}

		alloc = OS_AllocatePhysical(alloc_size, m_PageSize, m_PageSize, commit_flags);

		if (alloc.mp_Physical)
		{
			VMEM_ASSERT(alloc.m_Size >= size, "AllocatePhysical must return at least size bytes");
			VMEM_ASSERT((alloc.m_Size % m_PageSize) == 0, "AllocatePhysical must return at least size bytes");

			// if the memory we got back was mapped unmap it
			if (alloc.mp_OriginalMappedAddr)
				OS_UnmapPages(alloc.mp_OriginalMappedAddr, alloc.m_Size, m_PageSize);

			size_t new_alloc_size = alloc.m_Size;

			// take off the allocated ranges size
			if(m_AllocatedRanges.GetCapacity() - m_AllocatedRanges.GetCount() < 2)
			{
				if(commit_flags == VMEM_DEFAULT_COMMIT_FLAGS)
				{
					void* p_physical_array_mem = (byte*)alloc.mp_Physical + new_alloc_size - m_PageSize;
					void* p_array_mem = NULL;
					OS_MapPages(&p_array_mem, m_PageSize, p_physical_array_mem, m_PageSize);

					PhysicalAlloc array_alloc(p_physical_array_mem, m_PageSize, p_array_mem, NULL);
					m_AllocatedRanges.GiveMemory(array_alloc);

					new_alloc_size -= m_PageSize;
				}
				else
				{
					void* p_array_mem = NULL;
					OS_MapPages(&p_array_mem, m_PageSize, p_internal_mem, m_PageSize);
					
					PhysicalAlloc array_alloc(p_internal_mem, m_PageSize, p_array_mem, NULL);
					m_AllocatedRanges.GiveMemory(array_alloc);
					
					p_internal_mem = (byte*)p_internal_mem + m_PageSize;
					intermal_mem_size -= m_PageSize;
				}
			}

			// take off the excess array size
			if(m_ExcessPhysicalMemory.GetCapacity() - m_ExcessPhysicalMemory.GetCount() < 2)
			{
				if(commit_flags == VMEM_DEFAULT_COMMIT_FLAGS)
				{
					void* p_physical_array_mem = (byte*)alloc.mp_Physical + new_alloc_size - m_PageSize;
					void* p_array_mem = NULL;
					OS_MapPages(&p_array_mem, m_PageSize, p_physical_array_mem, m_PageSize);
					
					PhysicalAlloc array_alloc(p_physical_array_mem, m_PageSize, p_array_mem, NULL);
					m_ExcessPhysicalMemory.GiveMemory(array_alloc);

					new_alloc_size -= m_PageSize;
				}
				else
				{
					void* p_array_mem = NULL;
					OS_MapPages(&p_array_mem, m_PageSize, p_internal_mem, m_PageSize);

					PhysicalAlloc array_alloc(p_internal_mem, m_PageSize, p_array_mem, NULL);
					m_ExcessPhysicalMemory.GiveMemory(array_alloc);

					p_internal_mem = (byte*)p_internal_mem + m_PageSize;
					intermal_mem_size -= m_PageSize;
				}
			}

			// take off the excess memory
			size_t excess_size = new_alloc_size - size;
			if (excess_size)
			{
				ExcessRange excess_range((byte*)alloc.mp_Physical + size, excess_size, commit_flags);
				m_ExcessPhysicalMemory.Add(excess_range);
				new_alloc_size -= excess_size;
				VMEM_STATS(m_Stats.m_Unused += excess_size);
			}

			if(intermal_mem_size)
			{
				ExcessRange excess_range(p_internal_mem, intermal_mem_size, VMEM_DEFAULT_COMMIT_FLAGS);
				m_ExcessPhysicalMemory.Add(excess_range);
				VMEM_STATS(m_Stats.m_Unused += intermal_mem_size);
			}

			m_AllocatedRanges.Add(alloc);

			if(internal_alloc.mp_Physical)
				m_AllocatedRanges.Add(internal_alloc);

			VMEM_ASSERT(new_alloc_size == size, "something went wrong");
			alloc.m_Size = new_alloc_size;
		}
		else
		{
			if(internal_alloc.mp_Physical)
				InternalFree(internal_alloc);
		}

		return alloc;
	}

	//------------------------------------------------------------------------
	void PhysicalPageMapper::InternalFree(PhysicalAlloc alloc)
	{
		if (alloc.mp_Virtual != alloc.mp_OriginalMappedAddr)
		{
			if (alloc.mp_Virtual)
				OS_UnmapPages(alloc.mp_Virtual, alloc.m_Size, m_PageSize);

			if (alloc.mp_OriginalMappedAddr)
				OS_MapPages(&alloc.mp_OriginalMappedAddr, alloc.m_Size, alloc.mp_Physical, m_PageSize);
		}

		OS_FreePhysical(alloc.mp_Physical, alloc.mp_OriginalMappedAddr, alloc.m_Size);
	}

	//------------------------------------------------------------------------
	bool PhysicalPageMapper::MapPhysicalPages(void* p_virtual, size_t size, int commit_flags)
	{
		RangeMap<void*>::Range check_range;
		VMEM_ASSERT(!m_MappedRangeMap.TryGetRange(p_virtual, check_range), "overlapping range");
		VMEM_ASSERT(!m_MappedRangeMap.TryGetRange((byte*)p_virtual + size - 1, check_range), "overlapping range");
		VMEM_UNREFERENCED_PARAM(check_range);

		PhysicalPageHeap* p_physical_page_heap = GetPhysicalPageHeap(commit_flags);

		size_t max_free_size = p_physical_page_heap->GetMaxFreeRangeSize();

		size_t size_remaining = size;
		void* p = p_virtual;
		size_t alloc_size = max_free_size ? VMin(max_free_size, size) : size;
		bool alloc_from_os = max_free_size == 0;

		while (size_remaining)
		{
			void* p_physical = NULL;
			PhysicalAlloc physical_alloc;

			size_t actual_alloc_size = alloc_size;
			if(m_MappedRangeMap.IsFull())
			{
				if(commit_flags == VMEM_DEFAULT_COMMIT_FLAGS)
				{
					actual_alloc_size += m_PageSize;
				}
				else
				{
					PhysicalAlloc internal_alloc = InternalAlloc(m_PageSize, VMEM_DEFAULT_COMMIT_FLAGS);
					void* p_internal_page_physical = internal_alloc.mp_Physical;
					if(!p_internal_page_physical)
						return false;

					void* p_internal_page = NULL;
					OS_MapPages(&p_internal_page, m_PageSize, p_internal_page_physical, m_PageSize);

					AddInternalPage(p_internal_page);

					m_MappedRangeMap.GiveMemory((byte*)p_internal_page + sizeof(void*), m_PageSize - sizeof(void*));
				}
			}

			if(alloc_from_os)
			{
				physical_alloc = InternalAlloc(actual_alloc_size, VMEM_DEFAULT_COMMIT_FLAGS);
				p_physical = physical_alloc.mp_Physical;
			}
			else
			{
				p_physical = p_physical_page_heap->Alloc(actual_alloc_size);
				if(p_physical)
					VMEM_STATS(m_Stats.m_Unused -= actual_alloc_size);
			}

			if (p_physical)
			{
				VMEM_STATS(m_Stats.m_Used += alloc_size);

				if (m_MappedRangeMap.IsFull())
				{
					VMEM_ASSERT(commit_flags == VMEM_DEFAULT_COMMIT_FLAGS, "Should have been allocated from CPU physical memory");

					void* p_internal_page_physical = p_physical;
					p_physical = (byte*)p_physical + m_PageSize;

					void* p_internal_page = NULL;
					OS_MapPages(&p_internal_page, m_PageSize, p_internal_page_physical, m_PageSize);

					AddInternalPage(p_internal_page);

					m_MappedRangeMap.GiveMemory((byte*)p_internal_page + sizeof(void*), m_PageSize - sizeof(void*));
				}

				RangeMap<void*>::Range range(p, (byte*)p + alloc_size, p_physical);
				bool add_result = m_MappedRangeMap.Add(range);
				VMEM_ASSERT(add_result, "m_MappedRangeMap has been corrupted");
				VMEM_UNREFERENCED_PARAM(add_result);

				OS_MapPages(&p, alloc_size, p_physical, m_PageSize);

				size_remaining -= alloc_size;
				p = (byte*)p + alloc_size;

				alloc_size = VMin(alloc_size, size_remaining);
			}
			else
			{
				// try a smaller size
				VMEM_ASSERT(alloc_size > 0 && (alloc_size % m_PageSize) == 0, "PhysicalPageMapper has been corrupted");
				alloc_size = VMin(alloc_size - m_PageSize, size_remaining);

				// if PhysicalPageHeap has totally run out of memory allocate from the system
				if(!alloc_size && size_remaining)
				{
					if(alloc_from_os)
					{
						size_t mapped_size = size - size_remaining;
						if (mapped_size)
							UnMapPhysicalPages(p_virtual, mapped_size, commit_flags);
						return false;
					}
					else
					{
						alloc_from_os = true;
						alloc_size = size_remaining;
					}
				}
			}
		}

		AddExcessMemoryToHeap();

		return true;
	}

	//------------------------------------------------------------------------
	void PhysicalPageMapper::UnMapPhysicalPages(void* p_virtual, size_t size, int commit_flags)
	{
		OS_UnmapPages(p_virtual, size, m_PageSize);

		VMEM_STATS(VMEM_ASSERT(size <= m_Stats.m_Used, "PhysicalPageMapper has been corrupted"));
		VMEM_STATS(m_Stats.m_Used -= size);
		VMEM_STATS(m_Stats.m_Unused += size);

		void* p = p_virtual;
		void* p_virtual_end = (byte*)p_virtual + size;

		size_t size_remaining = size;
		while (size_remaining)
		{
			RangeMap<void*>::Range range;
			bool found_range = m_MappedRangeMap.TryGetRange(p, range);
			VMEM_ASSERT(found_range, "m_MappedRangeMap has been corrupted");
			VMEM_UNREFERENCED_PARAM(found_range);

			void* p_start = VMax(p, range.mp_Start);
			void* p_end = VMin(p_virtual_end, range.mp_End);
			size_t size_unmapped = (byte*)p_end - (byte*)p_start;

			m_MappedRangeMap.Remove(range);

			void* p_physical = range.m_Value;

			size_t prev_offcut_size = (byte*)p_start - (byte*)range.mp_Start;
			VMEM_ASSERT(prev_offcut_size >= 0, "PhysicalPageMapper has been corrupted");
			if (prev_offcut_size)
			{
				RangeMap<void*>::Range offcut_range(range.mp_Start, p_start, p_physical);
				bool offcut_add_result = m_MappedRangeMap.Add(offcut_range);
				VMEM_ASSERT(offcut_add_result, "shouldn't fail here due to oom becase we just removed an item");
				VMEM_UNREFERENCED_PARAM(offcut_add_result);
			}

			void* p_physical_start = (byte*)p_physical + prev_offcut_size;
			size_t physical_size = size_unmapped;

			size_t next_offcut_size = (byte*)range.mp_End - (byte*)p_end;
			VMEM_ASSERT(next_offcut_size >= 0, "PhysicalPageMapper has been corrupted");
			if (next_offcut_size)
			{
				void* p_internal_page = p_physical_start;
				bool used_internal_page = false;
				AddMappedRange(p_end, next_offcut_size, (byte*)p_physical + ((byte*)p_end - (byte*)range.mp_Start), p_internal_page, used_internal_page);

				if(used_internal_page)
				{
					p_physical_start = (byte*)p_physical_start + m_PageSize;
					physical_size -= m_PageSize;
					VMEM_STATS(m_Stats.m_Unused -= m_PageSize);
				}
			}

			// free the physical range (minus the internal page if used)
			if(physical_size)
				FreePhysical(p_physical_start, physical_size, commit_flags);

			size_remaining -= size_unmapped;
			p = (byte*)p + size_unmapped;
		}

		VMEM_ASSERT(!m_ExcessPhysicalMemory.GetCount(), "m_ExcessPhysicalMemory has been corrupted");
	}

	//------------------------------------------------------------------------
	void PhysicalPageMapper::AddMappedRange(void* p_virtual, size_t size, void* p_physical, void* p_internal_page_physical, bool& used_internal_page)
	{
		RangeMap<void*>::Range range(p_virtual, (byte*)p_virtual + size, p_physical);
		if (!m_MappedRangeMap.Add(range))
		{
			used_internal_page = true;

			void* p_internal_page = NULL;
			OS_MapPages(&p_internal_page, m_PageSize, p_internal_page_physical, m_PageSize);

			AddInternalPage(p_internal_page);

			m_MappedRangeMap.GiveMemory((byte*)p_internal_page + sizeof(void*), m_PageSize - sizeof(void*));

			bool result = m_MappedRangeMap.Add(range);
			VMEM_ASSERT(result, "m_MappedRangeMap.Add failed even after giving memory!");
			VMEM_UNREFERENCED_PARAM(result);
		}
	}

	//------------------------------------------------------------------------
	void PhysicalPageMapper::AddInternalPage(void* p_internal_page)
	{
		*(void**)p_internal_page = mp_InternalPages;
		mp_InternalPages = p_internal_page;
	}

	//------------------------------------------------------------------------
	#ifdef _WIN32
		#pragma warning(push)
		#pragma warning(disable : 4133)
	#endif

	//------------------------------------------------------------------------
	void PhysicalPageMapper::FreePhysical(void* p_physical, size_t size, int commit_flags)
	{
		PhysicalPageHeap* p_physical_page_heap = GetPhysicalPageHeap(commit_flags);

		if(!p_physical_page_heap->Free(p_physical, size))
		{
			// give PhysicalPageHeap some memory and try again
			void* p_virtual = NULL;
			OS_MapPages(&p_virtual, m_PageSize, p_physical, m_PageSize);

			AddInternalPage(p_virtual);

			VMEM_STATS(m_Stats.m_Unused -= m_PageSize);
			p_physical_page_heap->GiveMemory((byte*)p_virtual + sizeof(void*), m_PageSize - sizeof(void*));

			size_t new_size = size - m_PageSize;
			if (new_size)
			{
				bool result = p_physical_page_heap->Free((byte*)p_physical + m_PageSize, new_size);
				VMEM_ASSERT(result, "PhysicalPageHeap has been corrupted");
				VMEM_UNREFERENCED_PARAM(result);
			}
		}
	}

	//------------------------------------------------------------------------
	void PhysicalPageMapper::AddExcessMemoryToHeap()
	{
		int count = m_ExcessPhysicalMemory.GetCount();
		while(count)
		{
			ExcessRange range = m_ExcessPhysicalMemory[count - 1];
			m_ExcessPhysicalMemory.RemoveLast();

			FreePhysical(range.mp_Mem, range.m_Size, range.m_CommitFlags);

			count = m_ExcessPhysicalMemory.GetCount();
		}
	}
	
	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	Stats PhysicalPageMapper::GetStats() const
	{
		Stats stats = m_Stats;
		
		stats.m_Overhead += m_MappedRangeMap.GetMemorySize();

		for(int i=0; i<m_MaxPhysicalPageHeaps; ++i)
			stats.m_Overhead += m_PhysicalPageHeaps[i].GetMemorySize();

		stats.m_Overhead += m_AllocatedRanges.GetMemorySize();
		stats.m_Overhead += m_ExcessPhysicalMemory.GetMemorySize();

		for(void* p_internal_page = mp_InternalPages; p_internal_page != NULL; p_internal_page = *(void**)p_internal_page)
			stats.m_Overhead += m_PageSize;

		return stats;
	}
#endif

	//------------------------------------------------------------------------
	void PhysicalPageMapper::CheckIntegrity()
	{
#if defined(VMEM_ENABLE_STATS) && defined(VMEM_ASSERTS)
		Stats stats = GetStats();
		VMEM_ASSERT(g_PhysicalBytes == stats.GetCommittedBytes(), "Stats doesn't match g_PhysicalBytes");
#endif
	}

	//------------------------------------------------------------------------
	#ifdef _WIN32
		#pragma warning(pop)
	#endif
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

