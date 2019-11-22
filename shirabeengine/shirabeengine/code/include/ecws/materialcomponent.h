#ifndef __SHIRABE_COMPONENT_MATERIAL_H__
#define __SHIRABE_COMPONENT_MATERIAL_H__

#include <material/material_declaration.h>
#include "ecws/icomponent.h"

namespace engine::ecws
{
	class CMaterialComponent
		: public IComponent
	{
	public_constructors:
        CMaterialComponent();

    public_destructors:
		~CMaterialComponent() override;

	public_methods:
	    [[nodiscard]]
	    SHIRABE_INLINE
	    std::string const &name() const final { return mName; }

		EEngineStatus update(CTimer const &aTimer) final;

		EEngineStatus setMaterialInstance(Shared<material::CMaterialMaster> aMaterialInstance);

		[[nodiscard]]
		SHIRABE_INLINE
		Shared<material::CMaterialMaster> const getMaterialInstance() const { return mMaterialInstance; }

	private_members:
	    std::string                       mName;
	    Shared<material::CMaterialMaster> mMaterialInstance;
	};

}
#endif
