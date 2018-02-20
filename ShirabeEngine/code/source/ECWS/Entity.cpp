#include "ECWS/Entity.h"

#include "Log/Log.h"

namespace Engine {

	DeclareLogTag(Entity);

	Entity::Entity()
	{
	}


	Entity::~Entity()
	{
	}

	EEngineStatus Entity::addComponent(const IComponentPtr& component) {
		return EEngineStatus::Ok;
	}

	EEngineStatus Entity::removeComponent(const IComponentPtr& component) {
		return EEngineStatus::Ok;
	}

	EEngineStatus Entity::update(const Timer& timer) {
		EEngineStatus componentUpdateStatus = EEngineStatus::Ok;
		for (IComponentPtr component : m_components) {
			componentUpdateStatus = component->update(timer);
			if (CheckEngineError(componentUpdateStatus)) {
				Log::Error(logTag(), 
						   String::format("An error occurred updating component '%0' in entity '%1' [ErrorCode: %2]", 
										  component->name(), 
										  this->name(), 
										  componentUpdateStatus));
				continue;
			}
		}

		return componentUpdateStatus;
	}
}