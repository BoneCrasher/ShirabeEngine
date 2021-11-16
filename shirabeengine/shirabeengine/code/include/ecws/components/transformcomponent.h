#ifndef __SHIRABE_COMPONENT_TRANSFORM_H__
#define __SHIRABE_COMPONENT_TRANSFORM_H__

#include "ecws/componentsystem.h"
#include "buildingblocks/transform.h"

namespace engine::ecws
{
    struct STransformComponentState
    {
        CTransform transform;
    };

	class CTransformComponent
		: public AComponentBase<STransformComponentState>
	{
	public_constructors:
        explicit CTransformComponent(String aName);

    public_destructors:
		~CTransformComponent() override;

	public_methods:
		EEngineStatus update(CTimer const &aTimer) final;

		[[nodiscard]]
		SHIRABE_INLINE
		CTransform const &getTransform() const { return getComponentState().transform; }

        [[nodiscard]]
        SHIRABE_INLINE
        CTransform &getMutableTransform() { return getMutableComponentState().transform; }
	};

}
#endif
