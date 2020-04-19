#ifndef __SHIRABE_COMPONENT_CAMERA_H__
#define __SHIRABE_COMPONENT_CAMERA_H__

#include "ecws/componentbase.h"
#include "buildingblocks/camera.h"

namespace engine::ecws
{
	class CCameraComponent
		: public CComponentBase
	{
	public_constructors:
            CCameraComponent(std::string const &aName);

    public_destructors:
		~CCameraComponent() override;

	public_methods:
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
	    Shared<CCamera> mCamera;
	};

}
#endif
