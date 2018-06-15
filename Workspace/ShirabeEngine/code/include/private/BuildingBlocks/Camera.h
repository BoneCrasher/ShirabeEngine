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

  enum class CoordinateSystem {
    LH = 1,
    RH = 2
  };

	class Camera {
	public:
		struct FrustumParameters {
			float    fovY;
			uint16_t width;
			uint16_t height;
			float    nearPlaneDistance;
			float    farPlaneDistance;

			static FrustumParameters Default() {
        FrustumParameters p{};
				p.fovY              = (float)M_PI / 4.0f;
				p.width             = 1;
				p.height            = 1;
				p.nearPlaneDistance = 0.0f;
				p.farPlaneDistance  = 1.0f;
				return p;
			}
		};

		struct ProjectionParameters {
			CameraProjectionType projectionType;

			static ProjectionParameters Default() {
        ProjectionParameters p{};
				p.projectionType = CameraProjectionType::Perspective;
				return p;
			}
		};

		Camera();
		Camera(const Camera&);
		Camera(
      CameraViewType       const&,
      FrustumParameters    const&,
      ProjectionParameters const&,
      Vector3D             const&lookAt = Vector3D({ 0.0f, 0.0f, 0.0f }));
		~Camera();

		CameraViewType       const&viewType()             const { return m_viewType; }
		FrustumParameters    const&frustumParameters()    const { return m_frustumParameters; }
		ProjectionParameters const&projectionParameters() const { return m_projectionParameters; }
		Vector3D             const&lookAtTarget()         const { return m_lookAtTarget; }

		inline Matrix4x4 const&world()      const { return m_transform.world(); }
    inline Matrix4x4 const&view()       const { return m_viewMatrix; }
    inline Matrix4x4 const&projection() const { return m_projectionMatrix; }

	private:

		void createViewMatrix(CoordinateSystem const& = CoordinateSystem::RH);
		void createProjectionMatrix(CoordinateSystem const& = CoordinateSystem::RH);

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

