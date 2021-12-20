#ifndef __SHIRABE_COMPONENT_CAMERA_INL_H__
#define __SHIRABE_COMPONENT_CAMERA_INL_H__

//<-----------------------------------------------------------------------------
//
//<-----------------------------------------------------------------------------
#define CAMERA_COMPONENT_TEMPLATE_DECL \
    template<CompatibleComponentState_c<SCameraComponentState> TForwardedComponentState, typename... TForwardedSubsystems>

#define CAMERA_COMPONENT_TEMPLATE_SPEC \
    CCameraComponentImpl<TForwardedComponentState, TForwardedSubsystems...>

//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//
//<-----------------------------------------------------------------------------
CAMERA_COMPONENT_TEMPLATE_DECL
CAMERA_COMPONENT_TEMPLATE_SPEC::CCameraComponentImpl(String aName)
    : ASubsystemIntegratedComponentBase<TForwardedComponentState, CCameraSubsystem, TForwardedSubsystems...>(std::move(aName))
{ }
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//
//<-----------------------------------------------------------------------------
CAMERA_COMPONENT_TEMPLATE_DECL
CAMERA_COMPONENT_TEMPLATE_SPEC::~CCameraComponentImpl()
{
    this->getMutableComponentState().camera = nullptr;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//
//<-----------------------------------------------------------------------------
CAMERA_COMPONENT_TEMPLATE_DECL
EEngineStatus CAMERA_COMPONENT_TEMPLATE_SPEC::update(CTimer const &aTimer)
{
    this->getMutableComponentState().camera->update(aTimer);
    return EEngineStatus::Ok;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//
//<-----------------------------------------------------------------------------
CAMERA_COMPONENT_TEMPLATE_DECL
Shared<CCamera> const& CAMERA_COMPONENT_TEMPLATE_SPEC::getCamera() const
{
    return this->getComponentState().camera;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//
//<-----------------------------------------------------------------------------
CAMERA_COMPONENT_TEMPLATE_DECL
Shared<CCamera>& CAMERA_COMPONENT_TEMPLATE_SPEC::getMutableCamera()
{
    return this->getMutableComponentState().camera;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//
//<-----------------------------------------------------------------------------
CAMERA_COMPONENT_TEMPLATE_DECL
void CAMERA_COMPONENT_TEMPLATE_SPEC::setCamera(Shared<CCamera> const &aCamera)
{
    this->getMutableComponentState().camera = aCamera;
}
//<-----------------------------------------------------------------------------

#undef CAMERA_COMPONENT_TEMPLATE_SPEC
#undef CAMERA_COMPONENT_TEMPLATE_DECL

#endif // __SHIRABE_COMPONENT_CAMERA_INL_H__
