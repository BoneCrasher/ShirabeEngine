#ifndef __SHIRABE_IWINDOWFACTORY_H__
#define __SHIRABE_IWINDOWFACTORY_H__

// #include "Compiler/MSVCWarnings.h"

#include <string>

#include "Core/EngineStatus.h"
#include "Core/EngineTypeHelper.h"

#include "WSI/IWindow.h"

namespace Engine {
  namespace WSI {

    DeclareInterface(IWindowFactory);
      //
      // Interface API
      //
      virtual Ptr<IWindow> createWindow(
        std::string const&name,
        Rect        const&initialBounds) = 0;

    DeclareInterfaceEnd(IWindowFactory);
    DeclareSharedPointerType(IWindowFactory) // Declare IWindowFactoryPtr 
  }
}

#endif