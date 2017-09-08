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
			float _fovY;
			float _width;
			float _height;
			float _nearPlaneDistance;
			float _farPlaneDistance;

			static FrustumParameters Default() {
				FrustumParameters p;
				p._fovY              = (float)M_PI / 4.0f;
				p._width             = 1;
				p._height            = 1;
				p._nearPlaneDistance = 0.0f;
				p._farPlaneDistance  = 1.0f;
				return p;
			}
		};

		struct ProjectionParameters {
			CameraProjectionType _projectionType;

			static ProjectionParameters Default() {
				ProjectionParameters p;
				p._projectionType = CameraProjectionType::Perspective;
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

		const CameraViewType&       viewType()             const { return _viewType; }
		const FrustumParameters&    frustumParameters()    const { return _frustumParameters; }
		const ProjectionParameters& projectionParameters() const { return _projectionParameters; }
		const Vector3D&             lookAtTarget()         const { return _lookAtTarget; }

		inline const Matrix4x4& world()      const { return _transform.world(); }
		inline const Matrix4x4& view()       const { return _viewMatrix; }
		inline const Matrix4x4& projection() const { return _projectionMatrix; }

	private:

		void createViewMatrix();
		void createProjectionMatrix();

		CameraViewType       _viewType;
		FrustumParameters    _frustumParameters;
		ProjectionParameters _projectionParameters;
		Vector3D             _lookAtTarget;

		Transform _transform;

		Matrix4x4 _viewMatrix;
		Matrix4x4 _projectionMatrix;
	};
}

#endif

