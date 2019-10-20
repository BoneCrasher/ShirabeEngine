#ifndef __SHIRABE_COMPONENT_MATERIAL_H__
#define __SHIRABE_COMPONENT_MATERIAL_H__

#include <material/material_declaration.h>
#include "icomponent.h"

namespace engine {

	class CMaterialComponent
		: public IComponent
	{
	public_constructors:
        CMaterialComponent();

    public_destructors:
		~CMaterialComponent() override;

	public_methods:
		EEngineStatus update(CTimer const &aTimer) override;

	private_members:
	    Shared<material::CMaterialInstance> mMaterialInstance;
	};

}
#endif
