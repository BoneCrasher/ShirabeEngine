#ifndef __SHIRABE_COMPONENT_TRANSFORM_H__
#define __SHIRABE_COMPONENT_TRANSFORM_H__

#include "ecws/icomponent.h"
#include "buildingblocks/transform.h"

namespace engine::ecws
{
	class CTransformComponent
		: public IComponent
	{
	public_constructors:
            CTransformComponent();

    public_destructors:
		~CTransformComponent() override;

	public_methods:
	    [[nodiscard]]
	    SHIRABE_INLINE
	    std::string const &name() const final { return mName; }

		EEngineStatus update(CTimer const &aTimer) final;

		[[nodiscard]]
		SHIRABE_INLINE
		CTransform const &getTransform() const { return mTransform; }

        [[nodiscard]]
        SHIRABE_INLINE
        CTransform &getMutableTransform() { return mTransform; }

	private_members:
	    std::string  mName;
	    CTransform   mTransform;
	};

}
#endif
