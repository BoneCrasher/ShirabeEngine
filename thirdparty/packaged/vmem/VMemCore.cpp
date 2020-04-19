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
#include "VMemCore.hpp"
#include "VMemSys.hpp"
#include "VMemCriticalSection.hpp"
#include "VirtualMem.hpp"
#include "RelaxedAtomic.hpp"
#include <memory.h>

//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
namespace VMem
{
	void (*g_DebugBreakFn)(const _TCHAR* p_message) = NULL;

	void (*g_MemProDebugBreakFn)() = NULL;

	//------------------------------------------------------------------------
	void DebugWriteMem(size_t size)
	{
		float mb = (float)(size / 1024.0 / 1024.0);
#ifdef VMEM_X64
		DebugWrite(_T("%10lld (%0.1fMB)"), size, mb);
#else
		DebugWrite(_T("%10d (%0.1fMB)"), size, mb);
#endif

		if (mb < 10.0f)			DebugWrite(_T("   "));
		else if (mb < 100.0f)	DebugWrite(_T("  "));
		else if (mb < 1000.0f)	DebugWrite(_T(" "));
	}

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	void WriteStats(const VMem::Stats& stats)
	{
		size_t committed_bytes = stats.GetCommittedBytes();
		int usage_percent = committed_bytes ? (int)((100 * (long long)stats.m_Used) / committed_bytes) : 0;
		DebugWrite(_T("%3d%% "), usage_percent);

		DebugWriteMem(stats.m_Used);
		DebugWrite(_T("  "));
		DebugWriteMem(stats.m_Unused);
		DebugWrite(_T("  "));
		DebugWriteMem(stats.m_Overhead);
		DebugWrite(_T("  "));
		DebugWriteMem(committed_bytes);
		DebugWrite(_T("  "));
		DebugWriteMem(stats.m_Reserved);
		DebugWrite(_T("\n"));
	}
#endif

	//------------------------------------------------------------------------
#ifdef VMEM_ENABLE_STATS
	void WriteStats(const VMemHeapStats& stats)
	{
		DebugWrite(_T("                       Used                 Unused               Overhead                  Total               Reserved\n"));
		DebugWrite(_T("     FSA1:  "));	VMem::WriteStats(stats.m_FSAHeap1);
		DebugWrite(_T("     FSA2:  "));	VMem::WriteStats(stats.m_FSAHeap2);
		DebugWrite(_T("Coalesce1:  "));	VMem::WriteStats(stats.m_CoalesceHeap1);
		DebugWrite(_T("Coalesce2:  "));	VMem::WriteStats(stats.m_CoalesceHeap2);
		DebugWrite(_T("    Large:  "));	VMem::WriteStats(stats.m_LargeHeap);
		DebugWrite(_T("  Aligned:  "));	VMem::WriteStats(stats.m_AlignedCoalesceHeap + stats.m_AlignedLargeHeap);
		DebugWrite(_T(" Internal:  "));	VMem::WriteStats(stats.m_Internal);
		DebugWrite(_T("    TOTAL:  "));	VMem::WriteStats(stats.m_Total);
	}
#endif

	//------------------------------------------------------------------------
	#if VMEM_SIMULATE_OOM
		struct VMemCoreGlobals
		{
			VMemCoreGlobals()
			:	m_SimulateOOM(0),
				m_InternalHeapSimulateOOM(0)
			{
			}

			RelaxedAtomic32<int> m_SimulateOOM;
			RelaxedAtomic32<int> m_InternalHeapSimulateOOM;
			RelaxedAtomic32<int> m_PhysicalSimulateOOM;
		};
		byte gp_VMemCoreGlobalsMem[sizeof(VMemCoreGlobals)] VMEM_ALIGN_8;
		VMemCoreGlobals* gp_VMemCoreGlobals = NULL;

		void InitialiseOOMGlobals()
		{
			if(!gp_VMemCoreGlobals)
			{
				gp_VMemCoreGlobals = (VMemCoreGlobals*)gp_VMemCoreGlobalsMem;
				VMEM_MEMSET(gp_VMemCoreGlobals, VMEM_ALLOCATED_MEM, sizeof(gp_VMemCoreGlobalsMem));
				new (gp_VMemCoreGlobals) VMemCoreGlobals();
			}
		}

		void DestroyOOMGlobals()
		{
			gp_VMemCoreGlobals->~VMemCoreGlobals();
			gp_VMemCoreGlobals = NULL;
		}

		int GetSimulateOOMCount()						{ return gp_VMemCoreGlobals->m_SimulateOOM; }
		void SetSimulateOOMCount(int value)				{ gp_VMemCoreGlobals->m_SimulateOOM = value; }
		int GetInternalHeapSimulateOOMCount()			{ return gp_VMemCoreGlobals->m_InternalHeapSimulateOOM; }
		void SetInternalHeapSimulateOOMCount(int value)	{ gp_VMemCoreGlobals->m_InternalHeapSimulateOOM = value; }
		int GetPhysicalOOMCount()						{ return gp_VMemCoreGlobals->m_PhysicalSimulateOOM; }
		void SetPhysicalOOMCount(int value)				{ gp_VMemCoreGlobals->m_PhysicalSimulateOOM = value; }
	#endif

	//------------------------------------------------------------------------
	void InitialiseCore()
	{
		#if VMEM_SIMULATE_OOM
			InitialiseOOMGlobals();
		#endif

		VMemSysCreate();
	}

	//------------------------------------------------------------------------
	void UninitialiseCore()
	{
		VMemSysDestroy();

		#if VMEM_SIMULATE_OOM
			DestroyOOMGlobals();
		#endif
	}

	//------------------------------------------------------------------------
	// return true if break handled
	bool Break(const _TCHAR* p_message)
	{
		if(g_MemProDebugBreakFn)
			g_MemProDebugBreakFn();

		if(g_DebugBreakFn)
		{
			g_DebugBreakFn(p_message);
			return true;
		}
		else
		{
			DebugWrite(p_message);
			return false;
		}
	}

	//------------------------------------------------------------------------
	void SetDebugBreakFunction(void (*DebugBreakFn)(const _TCHAR* p_message))
	{
		g_DebugBreakFn = DebugBreakFn;
	}

	//------------------------------------------------------------------------
	void WriteAlloc(const void* p_alloc, size_t size)
	{
		DebugWrite(_T("%6d bytes at 0x%08x "), size, p_alloc);

		DebugWrite(_T("  <"));

		const size_t max_byte_printout = 8;
		size_t byte_count = VMin(max_byte_printout, size);

		// write hex memory
		const byte* p = (byte*)p_alloc;
		for(size_t i=0; i<byte_count; ++i)
		{
			byte c = *p++;
			if(c < 32 || c> 127)
				c = '.';
			DebugWrite(_T("%c"), (char)c);
		}
		DebugWrite(_T(">  "));

		// write chars
		p = (byte*)p_alloc;
		for(size_t i=0; i<byte_count; ++i)
			DebugWrite(_T("%02x "), *p++);
		if(byte_count < size)
			DebugWrite(_T("..."));

		DebugWrite(_T("\n"));
	}
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

