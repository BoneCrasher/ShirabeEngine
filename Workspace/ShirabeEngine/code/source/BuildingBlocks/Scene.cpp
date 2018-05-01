#include "BuildingBlocks/Scene.h"

#include "Log/Log.h"

#include "ECWS/Entity.h"

namespace Engine {

	DeclareLogTag(Scene);

	Scene::Scene()
		: m_componentFactory(nullptr),
		  m_timer(),
		  m_primaryCamera(),
		  m_entities()
	{		
	}

	Scene::~Scene()
	{
	}

	EEngineStatus Scene::initialize() {
		if (!m_componentFactory)
			return EEngineStatus::Ok;

		IComponentPtr meshComponent = m_componentFactory->createComponent(/* id */);

		EntityPtr entity = MakeSharedPointerType<Entity>();
		entity->addComponent(meshComponent);

		m_entities.push_back(entity);

		return EEngineStatus::Ok;
	}

	EEngineStatus Scene::deinitialize() {
		return EEngineStatus::Ok;
	}

	EEngineStatus Scene::update() {
		EEngineStatus status = EEngineStatus::Ok;

		if (CheckEngineError(status = m_timer.update()))
      return status;

		static double elapsed = 0;

		elapsed += m_timer.elapsed();
		if (elapsed >= 1.0) {
			Log::Status("Scene", String::format("Elapsed seconds: %0/%1 / FPS: %2", m_timer.total_elapsed(), m_timer.elapsed(), m_timer.FPS()));
			elapsed -= 1.0;
		}

		EEngineStatus entityUpdateStatus = EEngineStatus::Ok;
		for (EntityPtr& e : m_entities) {
			entityUpdateStatus = e->update(m_timer);
			if (CheckEngineError(entityUpdateStatus)) {
				Log::Error(logTag(), String::format("An error occurred updating entity '%0' [ErrorCode: %1]", e->name(), entityUpdateStatus));
				continue;
			}
		}

		return status;
	}

}