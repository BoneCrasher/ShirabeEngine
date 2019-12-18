#ifndef __SHIRABE_COMPONENT_CAMERA_H__
#define __SHIRABE_COMPONENT_CAMERA_H__

#include "ecws/icomponent.h"
#include "buildingblocks/camera.h"

namespace engine::ecws
{
	class CCameraComponent
		: public IComponent
	{
	public_constructors:
            CCameraComponent();

    public_destructors:
		~CCameraComponent() override;

	public_methods:
	    [[nodiscard]]
	    SHIRABE_INLINE
	    std::string const &name() const final { return mName; }

		EEngineStatus update(CTimer const &aTimer) final;

		[[nodiscard]]
		SHIRABE_INLINE
        Shared<CCamera> const &getCamera() const { return mCamera; }

        [[nodiscard]]
        SHIRABE_INLINE
        Shared<CCamera> &getMutableCamera() { return mCamera; }

        SHIRABE_INLINE
        void setCamera(Shared<CCamera> aCamera)
        {
		    mCamera = std::move(aCamera);
        }

	private_members:
	    std::string     mName;
	    Shared<CCamera> mCamera;
	};

}
#endif
