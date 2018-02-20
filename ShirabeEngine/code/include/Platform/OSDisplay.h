#ifndef __SHIRABE_WINAPIDISPLAY_H__
#define __SHIRABE_WINAPIDISPLAY_H__

#include <Platform/Platform.h>

#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#endif

#include "Core/EngineStatus.h"
#include "Core/EngineTypeHelper.h"

#include "Log/Log.h"
#include "Core/BasicTypes.h"
#include "Core/String.h"

namespace Platform {

	struct OSDisplayDescriptor {
#ifdef PLATFORM_WINDOWS
		HMONITOR     m_monitorHandle;
#endif
		std::string  m_name;
		Engine::Rect m_bounds;
		bool         m_isPrimary;
	};

	DeclareListType(OSDisplayDescriptor, OSDisplayDescriptor);

#ifdef PLATFORM_WINDOWS 
	class WinAPIDisplay {
	private:
		DeclareLogTag(WinAPIDisplay);

		static BOOL CALLBACK handleDisplayMonitor(HMONITOR monitor, HDC hdc, LPRECT rect, LPARAM param) {
			using namespace Engine;

			OSDisplayDescriptorList *pDescriptors = reinterpret_cast<OSDisplayDescriptorList *>(param);

			MONITORINFOEX info;
			info.cbSize = sizeof(MONITORINFOEX);
			if (!GetMonitorInfo(monitor, &info)) {
				Log::Warning(logTag(), String::format("Cannot retrieve monitor info for monitor '%0'", monitor->unused));
				return FALSE;
			} else {
				OSDisplayDescriptor descriptor ={};

				descriptor.m_monitorHandle = monitor;
				descriptor.m_name          = String::toNarrowString(info.szDevice);
				descriptor.m_bounds        = Rect(info.rcMonitor.left,
												 info.rcMonitor.top,
												 (info.rcMonitor.right - info.rcMonitor.left),
												 (info.rcMonitor.bottom - info.rcMonitor.top));
				descriptor.m_isPrimary     = ((info.dwFlags & MONITORINFOF_PRIMARY) == MONITORINFOF_PRIMARY);

				Log::Status(logTag(), String::format("Found %0 monitor '%1' with display area: Location: %2, %3; Size: %4, %5",
					(descriptor.m_isPrimary ? "[primary]" : "[additional]"),
													 descriptor.m_name,
													 descriptor.m_bounds.m_position.x(), descriptor.m_bounds.m_position.y(),
													 descriptor.m_bounds.m_size.x(),     descriptor.m_bounds.m_size.y()));

				pDescriptors->push_back(descriptor);

				return TRUE;
			}
		}
	public:
		static OSDisplayDescriptorList GetDisplays() {
			// Only primary screen
			// int screenWidth       = GetSystemMetrics(SM_CXSCREEN);
			// int screenHeight      = GetSystemMetrics(SM_CYSCREEN);
			// int windowBorderWidth = GetSystemMetrics(SM_CXBORDER);

			OSDisplayDescriptorList displayDescriptors;
			EnumDisplayMonitors(NULL, NULL, &WinAPIDisplay::handleDisplayMonitor, reinterpret_cast<LPARAM>(&displayDescriptors));

			return displayDescriptors;
		}
	};
#endif


	class OSDisplay
#ifdef PLATFORM_WINDOWS
		: public WinAPIDisplay
#endif
	{ };
}

#endif