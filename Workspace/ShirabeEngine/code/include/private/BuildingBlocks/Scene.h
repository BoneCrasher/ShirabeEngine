#ifndef __SHIRABE_SCENE_H__
#define __SHIRABE_SCENE_H__

#include "Benchmarking/Timer/Timer.h"

#include "BuildingBlocks/Camera.h"

#include "ECWS/Entity.h"
#include "ECWS/IComponentFactory.h"

namespace Engine {

	class Scene {
	public:
		Scene();
		~Scene();

		EEngineStatus initialize();
		EEngineStatus deinitialize();

		EEngineStatus update();
	private:
		IComponentFactoryPtr m_componentFactory;
		
		Timer  m_timer;
		Camera m_primaryCamera;

		EntityList m_entities;
		
	};

}

#endif
