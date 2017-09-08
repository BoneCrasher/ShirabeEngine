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
		HMONITOR     _monitorHandle;
#endif
		std::string  _name;
		Engine::Rect _bounds;
		bool         _isPrimary;
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

				descriptor._monitorHandle = monitor;
				descriptor._name          = String::toNarrowString(info.szDevice);
				descriptor._bounds        = Rect(info.rcMonitor.left,
												 info.rcMonitor.top,
												 (info.rcMonitor.right - info.rcMonitor.left),
												 (info.rcMonitor.bottom - info.rcMonitor.top));
				descriptor._isPrimary     = ((info.dwFlags & MONITORINFOF_PRIMARY) == MONITORINFOF_PRIMARY);

				Log::Status(logTag(), String::format("Found %0 monitor '%1' with display area: Location: %2, %3; Size: %4, %5",
					(descriptor._isPrimary ? "[primary]" : "[additional]"),
													 descriptor._name,
													 descriptor._bounds._position.x(), descriptor._bounds._position.y(),
													 descriptor._bounds._size.x(),     descriptor._bounds._size.y()));

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