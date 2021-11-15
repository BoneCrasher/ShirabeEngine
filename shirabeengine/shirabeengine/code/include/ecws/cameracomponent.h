#ifndef __SHIRABE_COMPONENT_CAMERA_H__
#define __SHIRABE_COMPONENT_CAMERA_H__

#include "ecws/componentsystem.h"
#include "buildingblocks/camera.h"

namespace engine::ecws
{
    struct SCameraComponentState
    {
        Shared<CCamera> camera;
    };

	class CCameraComponent
		: public AComponentBase<SCameraComponentState>
	{
	public_constructors:
        explicit CCameraComponent(std::string const &aName);

    public_destructors:
		~CCameraComponent() override;

	public_methods:
		EEngineStatus update(CTimer const &aTimer) final;

		[[nodiscard]]
		SHIRABE_INLINE
        Shared<CCamera> const &getCamera() const { return getComponentState().camera; }

        [[nodiscard]]
        SHIRABE_INLINE
        Shared<CCamera> &getMutableCamera() { return getMutableComponentState().camera; }

        SHIRABE_INLINE
        void setCamera(Shared<CCamera> aCamera)
        {
		    getMutableComponentState().camera = std::move(aCamera);
        }
	};

}
#endif
