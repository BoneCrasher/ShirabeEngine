#ifndef __SHIRABE_COMPONENT_MATERIAL_H__
#define __SHIRABE_COMPONENT_MATERIAL_H__

#include <material/declaration.h>
#include "ecws/componentbase.h"

namespace engine::ecws
{
	class CMaterialComponent
		: public CComponentBase
	{
	public_constructors:
        CMaterialComponent(std::string const &aName);

    public_destructors:
		~CMaterialComponent() override;

	public_methods:
		EEngineStatus update(CTimer const &aTimer) final;

		EEngineStatus setMaterialInstance(Shared<material::CMaterialInstance> aMaterialInstance);

		[[nodiscard]]
		SHIRABE_INLINE
		Shared<material::CMaterialInstance> const getMaterialInstance() const { return mMaterialInstance; }

		SHIRABE_INLINE
		material::CMaterialConfig &getMutableConfiguration()
        {
		    static material::CMaterialConfig sEmptyConfig = {};

		    if(nullptr == mMaterialInstance)
            {
		        return sEmptyConfig;
            }

		    return mMaterialInstance->getMutableConfiguration();
        }

	private_members:
	    Shared<material::CMaterialInstance> mMaterialInstance;
	};

}
#endif
