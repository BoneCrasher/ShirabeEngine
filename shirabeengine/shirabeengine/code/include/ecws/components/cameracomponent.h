#ifndef __SHIRABE_COMPONENT_CAMERA_H__
#define __SHIRABE_COMPONENT_CAMERA_H__

#include "ecws/components/transformcomponent.h"
#include "ecws/componentsystem.h"
#include "ecws/componentbase.h"
#include "buildingblocks/camera.h"

class CCameraSubsystem;

namespace engine::ecws
{
    struct SCameraComponentState
    {
        Shared<CCamera> camera;
    };

    /**
     *
     *
     * @tparam TForwardedComponentState
     * @tparam TForwardedSubsystems
     */
    template<CompatibleComponentState_c<SCameraComponentState> TForwardedComponentState, typename... TForwardedSubsystems>
	class CCameraComponentImpl
		: public ASubsystemIntegratedComponentBase<TForwardedComponentState, CCameraSubsystem, TForwardedSubsystems...>
	{
	public_constructors:
        explicit CCameraComponentImpl(String aName);

    public_destructors:
		~CCameraComponentImpl() override;

	public_methods:
		EEngineStatus update(CTimer const &aTimer) final;

		[[nodiscard]]
        Shared<CCamera> const &getCamera() const;

        [[nodiscard]]
        Shared<CCamera> &getMutableCamera();

        void setCamera(Shared<CCamera> const &aCamera);
	};

    class CCameraComponent
        : public CCameraComponentImpl<SCameraComponentState, class CCameraComponentSystem>
    {
    public_constructors:
        using CCameraComponentImpl<SCameraComponentState, class CCameraComponentSystem>::CCameraComponentImpl;
    };

    #include "ecws/components/cameracomponent.inl"
}
#endif
