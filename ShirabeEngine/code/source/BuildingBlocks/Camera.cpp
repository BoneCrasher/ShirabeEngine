#include "BuildingBlocks/Camera.h"

#ifdef PLATFORM_WINDOWS 
#include <DirectXMath.h>
#endif

#include "Platform/MathAdapter.h"

namespace Engine {

	Camera::Camera()
		: _viewType(CameraViewType::FreeCamera),
		_frustumParameters(FrustumParameters::Default()),
		_projectionParameters(ProjectionParameters::Default()),
		_lookAtTarget({ 0, 0, 0 })
	{
	}

	Camera::Camera(const Camera& cam)
		: _viewType(cam.viewType()),
		  _frustumParameters(cam.frustumParameters()),
		  _projectionParameters(cam.projectionParameters()),
		  _lookAtTarget(cam.lookAtTarget())
	{
	}

	Camera::Camera(const CameraViewType&       viewType,
				   const FrustumParameters&    frustumParameters,
				   const ProjectionParameters& projectionParameters,
				   const Vector3D&             lookAt)
		: _viewType(viewType),
		  _frustumParameters(frustumParameters),
		  _projectionParameters(projectionParameters),
		  _lookAtTarget(lookAt)
	{
	}
	
	Camera::~Camera()
	{
	}

	void Camera::createViewMatrix() {
#ifdef PLATFORM_WINDOWS
		using namespace DirectX;
		using namespace Platform::Math;

		XMMATRIX mat;
		XMVECTOR position;
		XMVECTOR up;

		XMVectorFromVector4D(_transform.translation(), position);
		XMVectorFromVector4D(_transform.up(), up);

		switch (_viewType) {
		case CameraViewType::TargetCamera:
			XMVECTOR focus;
			XMVectorFromVector4D(_lookAtTarget, focus);
			mat = XMMatrixLookAtLH(position, focus, up);
			break;
		case CameraViewType::FreeCamera:
			XMVECTOR dir;
			XMVectorFromVector4D(_transform.forward(), dir);
			mat = XMMatrixLookToLH(position, dir, up);
			break;
		}

		Matrix4x4FromXMMatrix(mat, _viewMatrix);
#endif
	}

	void Camera::createProjectionMatrix() {
#ifdef PLATFORM_WINDOWS
		using namespace DirectX;
		using namespace Platform::Math;

		XMMATRIX mat;
		switch (_projectionParameters._projectionType) {
		case CameraProjectionType::Perspective:
			mat = XMMatrixPerspectiveFovLH(_frustumParameters._fovY,
										   _frustumParameters._width / _frustumParameters._height,
										   _frustumParameters._nearPlaneDistance,
										   _frustumParameters._farPlaneDistance);
			break;
		case CameraProjectionType::Orthographic:
			mat = XMMatrixOrthographicLH(_frustumParameters._width, 
										 _frustumParameters._height, 
										 _frustumParameters._nearPlaneDistance,
										 _frustumParameters._farPlaneDistance);
			break;
		}

		Matrix4x4FromXMMatrix(mat, _projectionMatrix);
#endif
	}
}