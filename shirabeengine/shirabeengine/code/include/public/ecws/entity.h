#ifndef __SHIRABE_ENTITY_H__
#define __SHIRABE_ENTITY_H__

#include "core/enginestatus.h"
#include "ECWS/IComponent.h"

namespace engine {

	class Entity {
	public:
		Entity();
		~Entity();

		inline const std::string& name() const { return m_name; }
		inline void  name(const std::string& name) { m_name = name; }

		EEngineStatus addComponent(const IComponentPtr& component);
		EEngineStatus removeComponent(const IComponentPtr& component);

		template<typename TComponent>
		bool hasComponentOfType() const;

		template <typename TComponent>
		const TComponentList<TComponent> getComponentsOfType();

		EEngineStatus update(const Timer& timer);
		
	private:
		std::string m_name;

		IComponentList m_components;
	};
	DeclareSharedPointerType(Entity);
	DeclareListType(EntityPtr, Entity);

	template <typename TComponent>
	bool Entity::hasComponentOfType() const {
		return false;
	}

	template <typename TComponent>
	const TComponentList<TComponent> Entity::getComponentsOfType() {
		return {};
	}
}

#endif