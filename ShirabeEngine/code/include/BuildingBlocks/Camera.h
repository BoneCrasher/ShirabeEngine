#ifndef __SHIRABE_CAMERA_H__
#define __SHIRABE_CAMERA_H__

#include "Math/Common.h"
#include "Math/Vector.h"
#include "Math/Matrix.h"
#include "Math/Quaternion.h"

#include "BuildingBlocks/Transform.h"

namespace Engine {
	using namespace Math;

	enum class CameraViewType {
		FreeCamera,
		TargetCamera
	};

	enum class CameraProjectionType {
		Orthographic,
		Perspective
	};	

	class Camera {
	public:
		struct FrustumParameters {
			float m_fovY;
			float m_width;
			float m_height;
			float m_nearPlaneDistance;
			float m_farPlaneDistance;

			static FrustumParameters Default() {
				FrustumParameters p;
				p.m_fovY              = (float)M_PI / 4.0f;
				p.m_width             = 1;
				p.m_height            = 1;
				p.m_nearPlaneDistance = 0.0f;
				p.m_farPlaneDistance  = 1.0f;
				return p;
			}
		};

		struct ProjectionParameters {
			CameraProjectionType m_projectionType;

			static ProjectionParameters Default() {
				ProjectionParameters p;
				p.m_projectionType = CameraProjectionType::Perspective;
				return p;
			}
		};

		Camera();
		Camera(const Camera&);
		Camera(const CameraViewType&,
			   const FrustumParameters&,
			   const ProjectionParameters&,
			   const Vector3D& lookAt = Vector3D(0.0f, 0.0f, 0.0f));
		~Camera();

		const CameraViewType&       viewType()             const { return m_viewType; }
		const FrustumParameters&    frustumParameters()    const { return m_frustumParameters; }
		const ProjectionParameters& projectionParameters() const { return m_projectionParameters; }
		const Vector3D&             lookAtTarget()         const { return m_lookAtTarget; }

		inline const Matrix4x4& world()      const { return m_transform.world(); }
		inline const Matrix4x4& view()       const { return m_viewMatrix; }
		inline const Matrix4x4& projection() const { return m_projectionMatrix; }

	private:

		void createViewMatrix();
		void createProjectionMatrix();

		CameraViewType       m_viewType;
		FrustumParameters    m_frustumParameters;
		ProjectionParameters m_projectionParameters;
		Vector3D             m_lookAtTarget;

		Transform m_transform;

		Matrix4x4 m_viewMatrix;
		Matrix4x4 m_projectionMatrix;
	};
}

#endif

