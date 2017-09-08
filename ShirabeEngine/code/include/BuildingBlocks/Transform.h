#ifndef __SHIRABE_TRANSFORM_H__
#define __SHIRABE_TRANSFORM_H__

#include <atomic> 

#include "Math/Vector.h"
#include "Math/Matrix.h"
#include "Math/Quaternion.h"

namespace Engine {

	using namespace Math;


#define __SHIRABE_USE_QUATERNION_ROTATIONS

	class Transform {
	public:
		Transform();
		~Transform();

		Transform& rotate(const Vector3D& eulerRotation);
		Transform& rotate(const Vector3D& axis, 
						  const float&   phi);
		Transform& rotate(const Quaternion& quaternionRotation);
		Transform& resetRotation(const Vector3D& eulerRotation = Vector3D({ 0, 0, 0 }));
		Transform& resetRotation(const Vector3D& axis = Vector3D({ 1, 0, 0 }),
								 const float&   phi  = 0.0);
		Transform& resetRotation(const Quaternion& quaternionRotation = Quaternion(1, 0, 0, 0));
		Transform& translate(const Vector3D& translation);
		Transform& resetTranslation(const Vector3D& translation = Vector3D({ 0.0, 0.0, 0.0 }));
		Transform& scale(const float& factor);
		Transform& scale(const Vector3D& factors);
		Transform& resetScale(const float& factor = 1.0);
		Transform& resetScale(const Vector3D& factors = Vector3D({ 1.0, 1.0, 1.0 }));

		Transform& reset();

		inline Vector3D forward() const { return Quaternion::eulerFromQuaternion(_localRotationQuaternion * Vector3D::forward()); }
		inline Vector3D right()   const { return Quaternion::eulerFromQuaternion(_localRotationQuaternion * Vector3D::right());   }
		inline Vector3D up()      const { return Quaternion::eulerFromQuaternion(_localRotationQuaternion * Vector3D::up());      }

		const Matrix4x4& local();
		const Matrix4x4& world() const;

		inline const Vector3D& localTranslation() const { return _localTranslation; }
		inline const Vector3D  translation() const { return extractTranslationFromMatrix(_currentWorldTransform); }

		const Matrix4x4& updateWorldTransform(const Matrix4x4& parent);

	private:
		inline void setDirty() { _dirty.store(true);  }
		inline void setClean() { _dirty.store(false); }
		inline bool isDirty() const { return _dirty.load(); }

		inline const Vector3D extractTranslationFromMatrix(const Matrix4x4& m) const {
			return Vector3D({
				m.r03(),
				m.r13(),
				m.r23()
			});
		}

		Vector3D   _localScale;
#ifndef __SHIRABE_USE_QUATERNION_ROTATIONS
		Vector3D   _localEulerRotation;
#else 
		Quaternion _localRotationQuaternion;
#endif
		Vector3D   _localTranslation;

		Matrix4x4 _currentLocalTransform;
		Matrix4x4 _currentWorldTransform;
	
		std::atomic_bool _dirty;
	};

}

#endif