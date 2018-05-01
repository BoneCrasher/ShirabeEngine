#include "BuildingBlocks/Camera.h"

#ifdef PLATFORM_WINDOWS 
#include <DirectXMath.h>
#endif

#include "Platform/MathAdapter.h"

namespace Engine {

	Camera::Camera()
		: m_viewType(CameraViewType::FreeCamera)
		, m_frustumParameters(FrustumParameters::Default())
		, m_projectionParameters(ProjectionParameters::Default())
		, m_lookAtTarget({ 0, 0, 0 })
	{
	}

	Camera::Camera(const Camera& cam)
		: m_viewType(cam.viewType())
		, m_frustumParameters(cam.frustumParameters())
		, m_projectionParameters(cam.projectionParameters())
		, m_lookAtTarget(cam.lookAtTarget())
	{
	}

	Camera::Camera(const CameraViewType&       viewType,
				   const FrustumParameters&    frustumParameters,
				   const ProjectionParameters& projectionParameters,
				   const Vector3D&             lookAt)
		: m_viewType(viewType)
		, m_frustumParameters(frustumParameters)
		, m_projectionParameters(projectionParameters)
		, m_lookAtTarget(lookAt)
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

		XMVectorFromVector4D(m_transform.translation(), position);
		XMVectorFromVector4D(m_transform.up(), up);

		switch (m_viewType) {
		case CameraViewType::TargetCamera:
			XMVECTOR focus;
			XMVectorFromVector4D(m_lookAtTarget, focus);
			mat = XMMatrixLookAtLH(position, focus, up);
			break;
		case CameraViewType::FreeCamera:
			XMVECTOR dir;
			XMVectorFromVector4D(m_transform.forward(), dir);
			mat = XMMatrixLookToLH(position, dir, up);
			break;
		}

		Matrix4x4FromXMMatrix(mat, m_viewMatrix);
#endif
	}

	void Camera::createProjectionMatrix() {
#ifdef PLATFORM_WINDOWS
		using namespace DirectX;
		using namespace Platform::Math;

		XMMATRIX mat;
		switch (m_projectionParameters.m_projectionType) {
		case CameraProjectionType::Perspective:
			mat = XMMatrixPerspectiveFovLH(m_frustumParameters.m_fovY,
										   m_frustumParameters.m_width / m_frustumParameters.m_height,
										   m_frustumParameters.m_nearPlaneDistance,
										   m_frustumParameters.m_farPlaneDistance);
			break;
		case CameraProjectionType::Orthographic:
			mat = XMMatrixOrthographicLH(m_frustumParameters.m_width, 
										 m_frustumParameters.m_height, 
										 m_frustumParameters.m_nearPlaneDistance,
										 m_frustumParameters.m_farPlaneDistance);
			break;
		}

		Matrix4x4FromXMMatrix(mat, m_projectionMatrix);
#endif
	}
}