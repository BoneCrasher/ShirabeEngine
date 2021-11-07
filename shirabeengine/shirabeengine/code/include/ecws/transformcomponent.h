#ifndef __SHIRABE_COMPONENT_TRANSFORM_H__
#define __SHIRABE_COMPONENT_TRANSFORM_H__

#include "ecws/componentbase.h"
#include "buildingblocks/transform.h"

namespace engine::ecws
{
	class CTransformComponent
		: public CComponentBase
	{
	public_constructors:
        explicit CTransformComponent(std::string const &aName);

    public_destructors:
		~CTransformComponent() override;

	public_methods:
		EEngineStatus update(CTimer const &aTimer) final;

		[[nodiscard]]
		SHIRABE_INLINE
		CTransform const &getTransform() const { return mTransform; }

        [[nodiscard]]
        SHIRABE_INLINE
        CTransform &getMutableTransform() { return mTransform; }

	private_members:
	    CTransform mTransform;
	};

}
#endif
