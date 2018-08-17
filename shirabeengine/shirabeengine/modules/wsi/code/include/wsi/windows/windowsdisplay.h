#ifndef __SHIRABE_WSI_DISPLAY_H__
#define __SHIRABE_WSI_DISPLAY_H__

#include "Log/Log.h"
#include "Platform/Platform.h"
#include "OS/OSDisplay.h"

#include "Windows.h"

namespace engine {
  namespace wsi {

    class WinAPIDisplay {
    private:
      SHIRABE_DECLARE_LOG_TAG(WinAPIDisplay);

      static BOOL CALLBACK handleDisplayMonitor(HMONITOR monitor, HDC hdc, LPRECT rect, LPARAM param) {
        using namespace engine;
        using namespace engine::OS;

        Vector<OSDisplayDescriptor> *pDescriptors = reinterpret_cast<Vector<OSDisplayDescriptor> *>(param);

        MONITORINFOEX info;
        info.cbSize = sizeof(MONITORINFOEX);
        if(!GetMonitorInfo(monitor, &info)) {
          Log::Warning(logTag(), String::format("Cannot retrieve monitor info for monitor '%0'", monitor->unused));
          return FALSE;
        }
        else {
          OSDisplayDescriptor descriptor ={};

          descriptor.monitorHandle = (OSHandle)monitor;
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
            descriptor.bounds.size.x(), descriptor.bounds.size.y()));

          pDescriptors->push_back(descriptor);

          return TRUE;
        }
      }
    public:
      static Vector<OS::OSDisplayDescriptor> GetDisplays(uint32_t &primaryDisplayIndex) 
      {
        using namespace engine::OS;

        // Only primary screen
        // int screenWidth       = GetSystemMetrics(SM_CXSCREEN);
        // int screenHeight      = GetSystemMetrics(SM_CYSCREEN);
        // int windowBorderWidth = GetSystemMetrics(SM_CXBORDER);

        Vector<OSDisplayDescriptor> displayDescriptors;
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

  }
}

#endif