#include "os/osdisplay.h"

namespace engine
{
    namespace os
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        SOSDisplayDescriptor::SOSDisplayDescriptor()
            : monitorHandle(0)
            , name("")
            , bounds()
            , isPrimary(false)
        {}
        //<-----------------------------------------------------------------------------
    }
}
