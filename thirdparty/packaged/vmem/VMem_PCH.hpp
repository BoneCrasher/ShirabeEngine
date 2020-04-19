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
#ifndef VMEM_PCH_H_INCLUDED
#define VMEM_PCH_H_INCLUDED

//------------------------------------------------------------------------
#include <stdlib.h>
#include <new>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#ifdef VMEM_OS_WIN
	#include <tchar.h>
#endif

//-----------------------------------------------------------------------------
#include "VMemDefs.hpp"

//------------------------------------------------------------------------
#ifdef VMEM_ENABLE

//-----------------------------------------------------------------------------
#if defined(VMEM_PLATFORM_WIN)
	#ifdef __UNREAL__
		#include "Windows/AllowWindowsPlatformTypes.h"
	#endif
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <intrin.h>
	#ifdef __UNREAL__
		#include "Windows/HideWindowsPlatformTypes.h"
	#endif
#elif defined(VMEM_PLATFORM_XBOXONE)
	#ifdef __UNREAL__
		#include "Windows/AllowWindowsPlatformTypes.h"
	#endif
	#include <stdio.h>
	#include <windows.h>
	#ifdef __UNREAL__
		#include "Windows/HideWindowsPlatformTypes.h"
	#endif
#elif defined(VMEM_PLATFORM_XBOX360)
	#include <Xtl.h>
#endif

//------------------------------------------------------------------------
#endif		// #ifdef VMEM_ENABLE

//------------------------------------------------------------------------
#endif		// #ifndef VMEM_PCH_H_INCLUDED

