#include <base/string.h>
#include "wsi/x11/x11window.h"
#include "wsi/x11/x11windowfactory.h"

namespace engine
{
    namespace wsi
    {
        namespace x11
        {
            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            CX11WindowFactory::CX11WindowFactory(CStdSharedPtr_t<CX11Display> const &aDisplay)
                : mX11Display(aDisplay)
            {}
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //<
            //<-----------------------------------------------------------------------------
            CX11WindowFactory::~CX11WindowFactory()
            {}
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //<
            //<-----------------------------------------------------------------------------

            CStdSharedPtr_t<IWindow> CX11WindowFactory::createWindow(
                    std::string const &aName,
                    CRect       const &aInitialBounds)
            {
                CStdSharedPtr_t<IWindow> window = nullptr;

                Display *const  display = reinterpret_cast<Display *const>(mX11Display->displayHandle());
                uint64_t const &screen  = mX11Display->primaryScreenIndex();

                XSetWindowAttributes attributes{};
                attributes.background_pixel = XWhitePixel(display, screen);

                uint64_t const  x       = aInitialBounds.position.x();
                uint64_t const  y       = aInitialBounds.position.y();
                uint64_t const  w       = aInitialBounds.size.x();
                uint64_t const  h       = aInitialBounds.size.y();
                Visual  *const  visual  = DefaultVisual(display,screen);
                uint64_t const  d       = DefaultDepth(display, screen);

                try
                {
                    Window const x11Window = XCreateWindow(display, XRootWindow(display, screen), x, y, w, h, 1, d, InputOutput, visual, CWBackPixel, &attributes);

                    window = makeCStdSharedPtr<CX11Window>(display, x11Window, aName, aInitialBounds);
                }
                catch(...)
                {
                    // TBDone: Log
                    return nullptr;
                }

                return window;
            }
            //<-----------------------------------------------------------------------------
        }
    }
}
