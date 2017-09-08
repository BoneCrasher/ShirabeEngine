#ifndef __SHIRABE_IRENDERER_H__
#define __SHIRABE_IRENDERER_H__

#include "Core/EngineStatus.h"

#include "Platform/ApplicationEnvironment.h"
#include "Renderer/RendererConfiguration.h"

namespace Engine {
	namespace Renderer {
		using namespace Platform;

		DeclareInterface(IRenderer)
			virtual EEngineStatus initialize(const ApplicationEnvironment&,
											 const RendererConfiguration&
											 /* Insert Resourcemanager pointer here */) = 0;
			virtual EEngineStatus deinitialize()                                        = 0;
			virtual EEngineStatus reinitialize()                                        = 0;

			virtual EEngineStatus pause()                        = 0;
			virtual EEngineStatus resume()                       = 0;
			virtual bool          isPaused() const               = 0;

			virtual EEngineStatus render(/* insert queue type*/) = 0;
		DeclareInterfaceEnd(IRenderer)
		DeclareSharedPointerType(IRenderer)
	}
}

#endif