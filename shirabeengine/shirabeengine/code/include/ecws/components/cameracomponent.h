#ifndef __SHIRABE_COMPONENT_CAMERA_H__
#define __SHIRABE_COMPONENT_CAMERA_H__

#include "../componentsystem.h"
#include "../../buildingblocks/camera.h"

class CCameraSubsystem;

namespace engine::ecws
{
    struct SCameraComponentState
    {
        Shared<CCamera> camera;
    };

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
		SHIRABE_INLINE
        Shared<CCamera> const &getCamera() const {  return getComponentState().camera; }

        [[nodiscard]]
        SHIRABE_INLINE
        Shared<CCamera> &getMutableCamera() { return getMutableComponentState().camera; }

        SHIRABE_INLINE
        void setCamera(Shared<CCamera> aCamera)
        {
		    getMutableComponentState().camera = std::move(aCamera);
        }
	};

    class CCameraComponent final
        : public CCameraComponentImpl<SCameraComponentState>
    {};
}
#endif
