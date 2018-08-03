#ifndef __SHIRABE_RENDERERCONFIGURATION_H__
#define __SHIRABE_RENDERERCONFIGURATION_H__

#include "Core/BasicTypes.h"

namespace engine {
	namespace Rendering {

		struct RendererConfiguration {
			bool           requestFullscreen;
			bool           enableVSync;

			engine::TVector2D<uint32_t>
        preferredWindowSize,    // If !_requestFullscreen --> Which size should the window have?
			 preferredBackBufferSize; // The size of the backbuffer to be allocated. Will implicitly be truncated to the max size supported by the full primary display.
		
			engine::Vector4D
        frustum;                // frustum(x, y, z, w) --> (near, far, fovX, fovY)
		};

	}
}

#endif