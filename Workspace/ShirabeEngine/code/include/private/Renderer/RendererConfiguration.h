#ifndef __SHIRABE_RENDERERCONFIGURATION_H__
#define __SHIRABE_RENDERERCONFIGURATION_H__

#include "Core/BasicTypes.h"

namespace Engine {
	namespace Renderer {

		struct RendererConfiguration {
			bool           requestFullscreen;
			bool           enableVSync;

			Engine::TVector2D<uint32_t>
        preferredWindowSize,    // If !_requestFullscreen --> Which size should the window have?
			 preferredBackBufferSize; // The size of the backbuffer to be allocated. Will implicitly be truncated to the max size supported by the full primary display.
		
			Engine::Vector4D
        frustum;                // frustum(x, y, z, w) --> (near, far, fovX, fovY)
		};

	}
}

#endif