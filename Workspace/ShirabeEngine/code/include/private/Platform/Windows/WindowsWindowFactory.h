#ifndef __SHIRABE_WINDOWSWINDOWFACTORY_H__
#define __SHIRABE_WINDOWSWINDOWFACTORY_H__

#include "Window/IWindowFactory.h"

#define WIN32_LEAN_AND_MEAN // Exclude unnecessary sh**
#include <Windows.h>

namespace Platform {
  namespace Windows {
    using namespace Engine;

    class SHIRABE_LIBRARY_EXPORT WindowsWindowFactory
      : public IWindowFactory {
    public:
      WindowsWindowFactory(HINSTANCE instanceHandle);
      ~WindowsWindowFactory();

      //
      // IWindowFactory implementation
      //
      IWindowPtr createWindow(
        std::string const&name,
        Rect        const&initialBounds);

    private:
      HINSTANCE m_instanceHandle;
    };

  }
}

#endif
