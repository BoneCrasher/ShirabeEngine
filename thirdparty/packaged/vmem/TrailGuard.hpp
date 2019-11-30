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
#ifndef VMEM_TRAILGUARD_H_INCLUDED
#define VMEM_TRAILGUARD_H_INCLUDED

//------------------------------------------------------------------------
#include "VMemCore.hpp"
#include "VMemStats.hpp"

//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//------------------------------------------------------------------------
namespace VMem
{
	//------------------------------------------------------------------------
	class TrailGuard
	{
	public:
		TrailGuard();

		void Initialise(int size, int check_freq, int alloc_size=-1);

		void* Add(void* p) { return Add(p, m_FixedAllocSize); }

		void* Add(void* p, int size);

		int GetSize() const { return m_Size; }

		void* Shutdown();

		bool Contains(void* p) const;

		void CheckIntegrity() const;

		//------------------------------------------------------------------------
		// data
	private:
		int m_Size;
		int m_CurSize;
		int m_FixedAllocSize;

		void* mp_Head;
		void* mp_Tail;

		int m_FullCheckFreq;
		int m_FullCheckCounter;
	};
}

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
#endif		// #ifndef VMEM_TRAILGUARD_H_INCLUDED

