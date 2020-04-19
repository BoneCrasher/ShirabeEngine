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
#include "InternalHeap.hpp"
#include "VMemCore.hpp"

//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
namespace VMem
{
	const int g_InternalHeapRegionSize = 16*1024;

	//------------------------------------------------------------------------
	InternalHeap::InternalHeap(VirtualMem& virtual_mem)
	:	m_BasicCoalesceHeap(g_InternalHeapRegionSize, virtual_mem)
	{
	}

	//------------------------------------------------------------------------
#if VMEM_SIMULATE_OOM
	bool InternalHeap::SimulateOOM()
	{
		int simulate_oom_count = GetInternalHeapSimulateOOMCount();

		if(simulate_oom_count > 0)
		{
			if(simulate_oom_count == 1)
				return true;
			else
				SetInternalHeapSimulateOOMCount(simulate_oom_count - 1);
		}

		return false;
	}
#endif
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

