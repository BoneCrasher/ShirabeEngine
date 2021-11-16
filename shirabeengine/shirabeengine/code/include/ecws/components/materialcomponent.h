#ifndef __SHIRABE_COMPONENT_MATERIAL_H__
#define __SHIRABE_COMPONENT_MATERIAL_H__

#include "materialsystem/declaration.h"
#include "ecws/componentsystem.h"

namespace engine::ecws
{
    struct SMaterialComponentState
    {
        Shared<material::CMaterialInstance> material;
    };

	class CMaterialComponent
		: public AComponentBase<SMaterialComponentState>
	{
	public_constructors:
        explicit CMaterialComponent(String aName);

    public_destructors:
		~CMaterialComponent() override;

	public_methods:
		EEngineStatus update(CTimer const &aTimer) final;

		EEngineStatus setMaterialInstance(Shared<material::CMaterialInstance> aMaterialInstance);

		[[nodiscard]]
		SHIRABE_INLINE
		Weak<material::CMaterialInstance> getMaterialInstance() const { return getComponentState().material; }

		material::CMaterialConfig &getMutableConfiguration();
	};

}
#endif
