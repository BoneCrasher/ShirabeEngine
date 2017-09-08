#ifndef __SHIRABE_RENDERERCONFIGURATION_H__
#define __SHIRABE_RENDERERCONFIGURATION_H__

#include "Core/BasicTypes.h"

namespace Engine {
	namespace Renderer {

		struct RendererConfiguration {
			bool           _requestFullscreen;
			bool           _enableVSync;

			Engine::Vec2Dl _preferredWindowSize;     // If !_requestFullscreen --> Which size should the window have?
			Engine::Vec2Dl _preferredBackBufferSize; // The size of the backbuffer to be allocated. Will implicitly be truncated to the max size supported by the full primary display.
		
			Engine::Vec4Dd _frustum; // frustum(x, y, z, w) --> (near, far, fovX, fovY)
		};

	}
}

#endif