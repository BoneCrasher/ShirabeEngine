#ifndef __SHIRABE_RENDERERCONFIGURATION_H__
#define __SHIRABE_RENDERERCONFIGURATION_H__

#include <core/basictypes.h>

namespace engine
{
    namespace rendering
    {
        /**
         * The SRendererConfiguration struct describes how a renderer should
         * behave and operate.
         */
        struct SRendererConfiguration
        {
        public_members:
            bool                        requestFullscreen;
            bool                        enableVSync;
            engine::CVector2D<uint32_t> preferredWindowSize;     // If !_requestFullscreen --> Which size should the window have?
            engine::CVector2D<uint32_t> preferredBackBufferSize; // The size of the backbuffer to be allocated. Will implicitly be truncated to the max size supported by the full primary display
            engine::CVector4D_t         frustum;                 // frustum(x, y, z, w) --> (near, far, fovX, fovY)
        };

    }
}

#endif
