#include "BuildingBlocks/Scene.h"

#include "Log/Log.h"

#include "ECWS/Entity.h"

namespace Engine {

	DeclareLogTag(Scene);

	Scene::Scene()
		: _componentFactory(nullptr),
		  _timer(),
		  _primaryCamera(),
		  _entities()
	{		
	}

	Scene::~Scene()
	{
	}

	EEngineStatus Scene::initialize() {
		if (!_componentFactory)
			return EEngineStatus::Ok;

		IComponentPtr meshComponent = _componentFactory->createComponent(/* id */);

		EntityPtr entity = MakeSharedPointerType<Entity>();
		entity->addComponent(meshComponent);

		_entities.push_back(entity);

		return EEngineStatus::Ok;
	}

	EEngineStatus Scene::deinitialize() {
		return EEngineStatus::Ok;
	}

	EEngineStatus Scene::update() {
		EEngineStatus status = EEngineStatus::Ok;

		if (CheckEngineError(status = _timer.update())) {
			// Log::
			goto _return;
		}

		static double elapsed = 0;

		elapsed += _timer.elapsed();
		if (elapsed >= 1.0) {
			Log::Status("Scene", String::format("Elapsed seconds: %0/%1 / FPS: %2", _timer.total_elapsed(), _timer.elapsed(), _timer.FPS()));
			elapsed -= 1.0;
		}

		EEngineStatus entityUpdateStatus = EEngineStatus::Ok;
		for (EntityPtr& e : _entities) {
			entityUpdateStatus = e->update(_timer);
			if (CheckEngineError(entityUpdateStatus)) {
				Log::Error(logTag(), String::format("An error occurred updating entity '%0' [ErrorCode: %1]", e->name(), entityUpdateStatus));
				continue;
			}
		}

		_return:
		return status;
	}

}