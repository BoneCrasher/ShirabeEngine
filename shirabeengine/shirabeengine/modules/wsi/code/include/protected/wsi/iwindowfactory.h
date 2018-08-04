#ifndef __SHIRABE_IWINDOWFACTORY_H__
#define __SHIRABE_IWINDOWFACTORY_H__

// #include "Compiler/MSVCWarnings.h"

#include <string>

#include "core/enginestatus.h"
#include "core/enginetypehelper.h"

#include "WSI/IWindow.h"

namespace engine {
  namespace WSI {

    DeclareInterface(IWindowFactory);
      //
      // Interface API
      //
      virtual CStdSharedPtr_t<IWindow> createWindow(
        std::string const&name,
        Rect        const&initialBounds) = 0;

    DeclareInterfaceEnd(IWindowFactory);
    DeclareSharedPointerType(IWindowFactory) // Declare IWindowFactoryPtr 
  }
}

#endif