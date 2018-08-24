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


                return window;
            }
            //<-----------------------------------------------------------------------------
        }
    }
}
