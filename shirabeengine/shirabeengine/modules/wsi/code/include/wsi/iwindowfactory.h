#ifndef __SHIRABE_IWINDOWFACTORY_H__
#define __SHIRABE_IWINDOWFACTORY_H__

// #include "Compiler/MSVCWarnings.h"

#include <string>

#include <core/enginestatus.h>
#include <core/enginetypehelper.h>
#include "wsi/iwindow.h"

namespace engine
{
    namespace wsi
    {
        class IWindowFactory
        {
            SHIRABE_DECLARE_INTERFACE(IWindowFactory);

        public_api:
            /**
             * Create a window from a name and initial bounds.
             * @param aName          Name of the window to create.
             * @param aInitialBounds Bounding rectangle of the window to create.
             * @return               A pointer to IWindow if successful or nullptr.
             */
            virtual Shared<IWindow> createWindow(
                    std::string const &aName,
                    CRect       const &aInitialBounds) = 0;
        };

    }
}

#endif
