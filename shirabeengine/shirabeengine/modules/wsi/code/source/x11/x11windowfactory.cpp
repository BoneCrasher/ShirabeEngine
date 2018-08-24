#include <core/string.h>
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

                CStdSharedPtr_t<IWindow> window = makeCStdSharedPtr<CX11Window>(aName, aInitialBounds);

                Display *const display = reinterpret_cast<Display *const>(mX11Display->displayHandle());
                uint64_t const x       = aInitialBounds.position.x();
                uint64_t const y       = aInitialBounds.position.y();
                uint64_t const w       = aInitialBounds.size.x();
                uint64_t const h       = aInitialBounds.size.y();

                try
                {
                    Window const x11Window = XCreateWindow(display, 0, x, y, w, h, 1, CopyFromParent, InputOutput, nullptr, 0, nullptr);
                }
                catch(...)
                {
                    // TBDone: Log
                    return 0;
                }

                return window;
            }
            //<-----------------------------------------------------------------------------
        }
    }
}
