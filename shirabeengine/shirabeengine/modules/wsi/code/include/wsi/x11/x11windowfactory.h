#ifndef __SHIRABE_X11_WINDOWFACTORY_H__
#define __SHIRABE_X11_WINDOWFACTORY_H__

#include "wsi/iwindowfactory.h"
#include "wsi/x11/x11display.h"

namespace engine
{
    namespace wsi
    {
        namespace x11
        {
            using namespace engine;

            /**
             * The CX11WindowFactory implementing IWindowFactory is capable
             * of creating IWindow-conformant x11 window instances, usable
             * from within a window manager.
             */
            class SHIRABE_LIBRARY_EXPORT CX11WindowFactory
                    : public IWindowFactory
            {
            public_constructors:
                /**
                 * Create a window factory attached to a specific display.
                 *
                 * @param aDisplay The x11 display to attach to.
                 */
                CX11WindowFactory(Shared<CX11Display> const &aDisplay);

            public_destructors:
                /**
                 * Destroy and run... quickly...
                 */
                virtual ~CX11WindowFactory() final;

            public_methods:
                /**
                 * Create a new window instance for a given name and initial bounds.
                 *
                 * @param aName          The name of the new window to create.
                 * @param aInitialBounds The initial bounds of the new window.
                 * @return               A pointer to a new x11 window or nullptr on error.
                 */
                Shared<IWindow> createWindow(
                        std::string const &aName,
                        CRect       const &aInitialBounds);

            private_members:
                Shared<CWSIDisplay> mX11Display;
            };

        }
    }
}

#endif
