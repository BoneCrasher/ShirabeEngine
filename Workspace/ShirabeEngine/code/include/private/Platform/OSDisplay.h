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
		HMONITOR     monitorHandle;
#endif
		std::string  name;
		Engine::Rect bounds;
		bool         isPrimary;
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

				descriptor.monitorHandle = monitor;
				descriptor.name          = String::toNarrowString(info.szDevice);
				descriptor.bounds        = Rect(info.rcMonitor.left,
												 info.rcMonitor.top,
												 (info.rcMonitor.right - info.rcMonitor.left),
												 (info.rcMonitor.bottom - info.rcMonitor.top));
				descriptor.isPrimary = ((info.dwFlags & MONITORINFOF_PRIMARY) == MONITORINFOF_PRIMARY);

				Log::Status(logTag(), String::format("Found %0 monitor '%1' with display area: Location: %2, %3; Size: %4, %5",
					(descriptor.isPrimary ? "[primary]" : "[additional]"),
													 descriptor.name,
													 descriptor.bounds.position.x(), descriptor.bounds.position.y(),
													 descriptor.bounds.size.x(),     descriptor.bounds.size.y()));

				pDescriptors->push_back(descriptor);

				return TRUE;
			}
		}
	public:
		static OSDisplayDescriptorList GetDisplays(uint32_t &primaryDisplayIndex) {
			// Only primary screen
			// int screenWidth       = GetSystemMetrics(SM_CXSCREEN);
			// int screenHeight      = GetSystemMetrics(SM_CYSCREEN);
			// int windowBorderWidth = GetSystemMetrics(SM_CXBORDER);

			OSDisplayDescriptorList displayDescriptors;
			EnumDisplayMonitors(NULL, NULL, &WinAPIDisplay::handleDisplayMonitor, reinterpret_cast<LPARAM>(&displayDescriptors));

      uint32_t k=0;
      for(OSDisplayDescriptor const&desc : displayDescriptors) {
        if(desc.isPrimary) {
          primaryDisplayIndex = k;
          break;
        }
        ++k;
      }

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